/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
*/

#include "RebarNotesHelper.h"

using namespace std;

RebarNotesHelper::RebarNotesHelper(std::list<Panel>& vecPanels)
{
	this->m_PanelsList = vecPanels;

	std::wstring name = curDoc()->fileName();
	std::string str(name.begin(), name.end());
	str = str.substr(str.find_last_of("\\") + 1);
	std:: string drawingName = str.substr(0, str.find_last_of("."));

	m_location = "C:\\AutoCADReports\\"+ drawingName + "_Reinforcements_Data_" + ".csv";
}

RebarNotesHelper::~RebarNotesHelper()
{
	this->m_PanelsList.clear();
	m_csvfile.close();
	DeleteFileA(m_location.c_str());
}

void RebarNotesHelper::PlaceLabels()
{
	
	/*
	* Steps:
	* 1. Create CSV and ask the user to enter the reinforcement values
	* 2. Compute the label placement points based on the number of legs
	* 3. Place relevant labels based on the CSV conditions
	*/

	CreateCSVFile();

	bool bDataIsCaptured = ParseCSVFile();

	if (!bDataIsCaptured)
	{
		::MessageBoxA(NULL, "Data has not been entered into the csv file", "Rebar Table", MB_OK | MB_ICONERROR);
	}
	for (auto& panel : m_PanelsList)
	{
		int iPanel = 1; 
		int iLegNo = 1;
		vector<AcGePoint3d> connectPoints; // points where label points are located.
		vector<AcGePoint3d> internalConnectPoints; // point from which line has to be drawn from the panel.

		ComputeLabelConnectPoints(panel, connectPoints, internalConnectPoints);

		if (panel.vecRebarLabelsOutsideInternalPanel.size() > 0)
		{
			vector<string> rebarLabels;
			int iLabelCount = 0;
			for (auto& rebarLabel : panel.vecRebarLabelsOutsideInternalPanel)
			{
				if (rebarLabel.first.find("CENTERED") != rebarLabel.first.npos)
				{
					if (rebarLabels.size() > 0)
					{
						ProcessRebars(rebarLabels, m_VecContent[iPanel], iLegNo, panel, connectPoints,internalConnectPoints);

						// empty the collection for the next label
						iLegNo++;
						rebarLabels.clear();
					}
				}

				else if (rebarLabel.first.find("BOT. FACE") != rebarLabel.first.npos || rebarLabel.first.find("TOP FACE") != rebarLabel.first.npos)
				{
					rebarLabels.push_back(rebarLabel.first);
				}

				iLabelCount++;

				if (iLabelCount == panel.vecRebarLabelsOutsideInternalPanel.size())
				{
					ProcessRebars(rebarLabels, m_VecContent[iPanel], iLegNo, panel, connectPoints, internalConnectPoints);
				}

			}
		}
		iPanel++;
	}
	return;
}

void RebarNotesHelper::CreateCSVFile()
{
	m_csvfile.open(m_location, std::ios_base::app);
	
	// Header
	m_csvfile << "Panel Name, Leg-1, Leg-2, Leg-3, Leg-4" << "\n";

	// Contents
	for (auto& panel : m_PanelsList)
	{
		// Check if panel has rebar labels. 
		if (panel.vecRebarLabelsOutsideInternalPanel.size() > 0)
		{
			std::string  line = panel.getPanelName() + ",-,-,-,-\n";
			m_csvfile << line;
		}
	}

	m_csvfile.close();

	std::string strMessage = "Please enter the Reinforcement values in " + m_location +" And then click OK";
	std::string strTitle = "Rebar Table";

	::MessageBoxA(NULL, strMessage.c_str(), strTitle.c_str(), MB_OK);

	return;
}

bool RebarNotesHelper::ParseCSVFile()
{
	// Read each  line and create a map of Panel and legs
	bool bDataEntered = false;
	std::vector<std::string> row;
	std::string line, word;

	std::fstream file(m_location);
	if (file.is_open())
	{
		while (getline(file, line))
		{
			row.clear();

			std::stringstream str(line);

			while (getline(str, word, ','))
				row.push_back(word);
			m_VecContent.push_back(row);
		}
		
		int iSize = m_VecContent.size();
		
		// Validate if the data has been entered, else return false;
		for (int k = 1; k < iSize ; k++)
		{
			for (int i = 1; i < 5; i++)
			{
				if (m_VecContent[k][i] != "-")
				{
					bDataEntered = true;
					break;
				}
			}
		}
	}
	else
	{
		return false;
	}

	return bDataEntered;

}

void RebarNotesHelper::ComputeLabelConnectPoints(Panel& panel, vector<AcGePoint3d>& connectPoints, vector<AcGePoint3d>& internalConnectPoints)
{
	// Connect points are computed from a fixed detail label and are placed at the def point corners 
	COORDINATES anchorPositon = panel.getAnchorPositionForRebarPlacement();

	BOUNDS panelBounds = panel.getInternalPanelBounds();

	COORDINATES leftMidPoint;
	COORDINATES internalLeftMidPoint;
	leftMidPoint.first = anchorPositon.first - 230.0f;
	leftMidPoint.second = anchorPositon.second + 260.0f;

	COORDINATES bottomMidPoint;
	COORDINATES internalBottomMidPoint;
	bottomMidPoint.first = anchorPositon.first - 95.0f;
	bottomMidPoint.second = anchorPositon.second + 70.0f;

	COORDINATES rightMidPoint;
	COORDINATES internalrightMidPoint;
	rightMidPoint.first = anchorPositon.first + 440.0f;
	rightMidPoint.second = anchorPositon.second + 260.0f;

	// calculate the number of strips
	int numStrips = panel.vecRebarLabelsOutsideInternalPanel.size() / 2;

	if (numStrips > 0)
	{
		std::vector<BOUNDS> panelOpenings;

		// collect all the openings
		for (auto& opening : panel.vecOpenings)
		{
			panelOpenings.push_back(opening);
		}
		for (auto& opening : panel.vecFutureOpenings)
		{
			panelOpenings.push_back(opening);
		}

		// sort based on y-position of the lowest bound
		std::sort(panelOpenings.begin(), panelOpenings.end(), [](const BOUNDS& bound1, const BOUNDS& bound2) {
			return bound1.first.second < bound2.first.second;
			});

		// two strips case 
		if (numStrips == 2 && panelOpenings.size() > 0)
		{
			auto opening = panelOpenings[0];

			// Find a point between opening and left panel bound
			internalLeftMidPoint.first = (panelBounds.first.first + opening.first.first) / 2;
			internalLeftMidPoint.second = (opening.first.second + opening.second.second) / 2;

			// Find a point between opening and right panel bound
			internalrightMidPoint.first = (panelBounds.second.first + opening.second.first) / 2;
			internalrightMidPoint.second = (opening.first.second + opening.second.second) / 2;
		}

		// three strips case
		if (numStrips == 3 && panelOpenings.size() > 1)
		{
			auto leftOpening = panelOpenings[0];
			auto rightOpening = panelOpenings[1];

			// sort the two openings based on x position
			if (leftOpening.first.first > rightOpening.first.first)
			{
				leftOpening = panelOpenings[1];
				rightOpening = panelOpenings[0];
			}

			// Find a point between first opening and left panel bound
			internalLeftMidPoint.first = (panelBounds.first.first + leftOpening.first.first) / 2;
			internalLeftMidPoint.second = (leftOpening.first.second + leftOpening.second.second) / 2;

			//Find a point between two openings. Pick Y co-ordinate based on the smallest opening
			internalBottomMidPoint.first = (leftOpening.second.first + rightOpening.first.first) / 2;
			if((leftOpening.second.second - leftOpening.first.second) > (rightOpening.second.second - rightOpening.first.second))
				internalBottomMidPoint.second = (rightOpening.first.second + rightOpening.second.second) / 2;
			else
				internalBottomMidPoint.second = (leftOpening.first.second + leftOpening.second.second) / 2;

			// Find a point between second opening and right panel bound
			internalrightMidPoint.first = (panelBounds.second.first + rightOpening.second.first) / 2;
			internalrightMidPoint.second = (rightOpening.first.second + rightOpening.second.second) / 2;
		}

		// push two connect points ( left and right ) 
		if (numStrips == 2)
		{
			connectPoints.push_back(AcGePoint3d(leftMidPoint.first, internalLeftMidPoint.second, 0));
			internalConnectPoints.push_back(AcGePoint3d(internalLeftMidPoint.first, internalLeftMidPoint.second, 0));

			connectPoints.push_back(AcGePoint3d(rightMidPoint.first, internalrightMidPoint.second, 0));
			internalConnectPoints.push_back(AcGePoint3d(internalrightMidPoint.first, internalrightMidPoint.second, 0));
		}

		// push three connect points ( left, bottom and right )
		if (numStrips == 3)
		{
			connectPoints.push_back(AcGePoint3d(leftMidPoint.first, internalLeftMidPoint.second, 0));
			internalConnectPoints.push_back(AcGePoint3d(internalLeftMidPoint.first, internalLeftMidPoint.second, 0));

			connectPoints.push_back(AcGePoint3d(internalBottomMidPoint.first, bottomMidPoint.second, 0));
			internalConnectPoints.push_back(AcGePoint3d(internalBottomMidPoint.first, internalBottomMidPoint.second, 0));

			connectPoints.push_back(AcGePoint3d(rightMidPoint.first, internalrightMidPoint.second, 0));
			internalConnectPoints.push_back(AcGePoint3d(internalrightMidPoint.first, internalrightMidPoint.second, 0));
		}
	}
}

void RebarNotesHelper::PlaceBoxAndLines(int iLegNo, std::vector<AcGePoint3d>& connectPoints, std::vector<AcGePoint3d>& internalConnectPoints, AcDbBlockTableRecordPointer& pBTR)
{
	AcGePoint3d lineAnchorPoint = connectPoints[iLegNo - 1];
	AcGePoint3d internalConnectPoint = internalConnectPoints[iLegNo - 1];

	AcDbLine* interalConnectLine = new AcDbLine();
	interalConnectLine->setStartPoint(internalConnectPoint - AcGeVector3d(5, 5, 0));
	interalConnectLine->setEndPoint(internalConnectPoint + AcGeVector3d(5, 5, 0));
	pBTR->appendAcDbEntity(interalConnectLine);
	interalConnectLine->close();

	// create line boxes for 2 strips 
	if (connectPoints.size() == 2)
	{
		double xFarOffset = -10;
		double xNearOffset = 60;
		if (iLegNo == 2)
		{
			xFarOffset = 60;
			xNearOffset = -10;
		}
		AcDbLine* iExternalConnectLine = new AcDbLine();
		iExternalConnectLine->setStartPoint(internalConnectPoint);
		iExternalConnectLine->setEndPoint(lineAnchorPoint + AcGeVector3d(xFarOffset, 0, 0));
		pBTR->appendAcDbEntity(iExternalConnectLine);
		iExternalConnectLine->close();

		AcDbLine* outerVerticalLine = new AcDbLine();
		outerVerticalLine->setStartPoint(lineAnchorPoint + AcGeVector3d(xFarOffset, 0, 0));
		outerVerticalLine->setEndPoint(lineAnchorPoint + AcGeVector3d(xFarOffset, 300, 0));
		pBTR->appendAcDbEntity(outerVerticalLine);
		outerVerticalLine->close();

		AcDbLine* innerVerticalLine = new AcDbLine();
		innerVerticalLine->setStartPoint(lineAnchorPoint + AcGeVector3d(xNearOffset, 0, 0));
		innerVerticalLine->setEndPoint(lineAnchorPoint + AcGeVector3d(xNearOffset, 300, 0));
		pBTR->appendAcDbEntity(innerVerticalLine);
		innerVerticalLine->close();

		AcDbLine* closingHorizontalLine = new AcDbLine();
		closingHorizontalLine->setStartPoint(lineAnchorPoint + AcGeVector3d(xNearOffset, 300, 0));
		closingHorizontalLine->setEndPoint(lineAnchorPoint + AcGeVector3d(xFarOffset, 300, 0));
		pBTR->appendAcDbEntity(closingHorizontalLine);
		closingHorizontalLine->close();
	}
	// create line boxes for 3 strips
	else if (connectPoints.size() == 3)
	{
		// left and right rebars
		if (iLegNo == 1 || iLegNo == 3)
		{
			double xFarOffset = -10;
			double xNearOffset = 60;
			// change offsets for right rebar
			if (iLegNo == 3)
			{
				xFarOffset = 60;
				xNearOffset = -10;
			}
			AcDbLine* iExternalConnectLine = new AcDbLine();
			iExternalConnectLine->setStartPoint(internalConnectPoint);
			iExternalConnectLine->setEndPoint(lineAnchorPoint + AcGeVector3d(xFarOffset, 0, 0));
			pBTR->appendAcDbEntity(iExternalConnectLine);
			iExternalConnectLine->close();

			AcDbLine* outerVerticalLine = new AcDbLine();
			outerVerticalLine->setStartPoint(lineAnchorPoint + AcGeVector3d(xFarOffset, 0, 0));
			outerVerticalLine->setEndPoint(lineAnchorPoint + AcGeVector3d(xFarOffset, 300, 0));
			pBTR->appendAcDbEntity(outerVerticalLine);
			outerVerticalLine->close();

			AcDbLine* innerVerticalLine = new AcDbLine();
			innerVerticalLine->setStartPoint(lineAnchorPoint + AcGeVector3d(xNearOffset, 0, 0));
			innerVerticalLine->setEndPoint(lineAnchorPoint + AcGeVector3d(xNearOffset, 300, 0));
			pBTR->appendAcDbEntity(innerVerticalLine);
			innerVerticalLine->close();

			AcDbLine* closingHorizontalLine = new AcDbLine();
			closingHorizontalLine->setStartPoint(lineAnchorPoint + AcGeVector3d(xNearOffset, 300, 0));
			closingHorizontalLine->setEndPoint(lineAnchorPoint + AcGeVector3d(xFarOffset, 300, 0));
			pBTR->appendAcDbEntity(closingHorizontalLine);
			closingHorizontalLine->close();
		}
		// bottom rebar
		else if (iLegNo == 2)
		{
			double yFarOffset = -55;
			double yNearOffset = 10;

			AcDbLine* iExternalConnectLine = new AcDbLine();
			iExternalConnectLine->setStartPoint(internalConnectPoint);
			iExternalConnectLine->setEndPoint(lineAnchorPoint + AcGeVector3d(0, yFarOffset, 0));
			pBTR->appendAcDbEntity(iExternalConnectLine);
			iExternalConnectLine->close();

			AcDbLine* outerVerticalLine = new AcDbLine();
			outerVerticalLine->setStartPoint(lineAnchorPoint + AcGeVector3d(0, yFarOffset, 0));
			outerVerticalLine->setEndPoint(lineAnchorPoint + AcGeVector3d(300, yFarOffset, 0));
			pBTR->appendAcDbEntity(outerVerticalLine);
			outerVerticalLine->close();

			AcDbLine* innerVerticalLine = new AcDbLine();
			innerVerticalLine->setStartPoint(lineAnchorPoint + AcGeVector3d(0, yNearOffset, 0));
			innerVerticalLine->setEndPoint(lineAnchorPoint + AcGeVector3d(300, yNearOffset, 0));
			pBTR->appendAcDbEntity(innerVerticalLine);
			innerVerticalLine->close();

			AcDbLine* closingHorizontalLine = new AcDbLine();
			closingHorizontalLine->setStartPoint(lineAnchorPoint + AcGeVector3d(300, yFarOffset, 0));
			closingHorizontalLine->setEndPoint(lineAnchorPoint + AcGeVector3d(300, yNearOffset, 0));
			pBTR->appendAcDbEntity(closingHorizontalLine);
			closingHorizontalLine->close();
		}
	}
}

void RebarNotesHelper::ProcessRebars(vector<string> labels, vector<string> contents, int iLegNo, Panel& panel, vector<AcGePoint3d>& connectPoints, std::vector<AcGePoint3d>& internalConnectPoints)
{
	AcGeVector3d labelOffset = AcGeVector3d(0,0,0);
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTableRecordPointer pBTR
	(
		acdbSymUtil()->blockModelSpaceId(pDb),
		AcDb::kForWrite
	);
	
	if (connectPoints.size() == 2)
	{
		labelOffset = AcGeVector3d(0, 15, 0);
	}
	else if (connectPoints.size() == 3)
	{
		if( iLegNo == 1 || iLegNo == 3)
			labelOffset = AcGeVector3d(0, 15, 0);
		else if (iLegNo == 2)
			labelOffset = AcGeVector3d(15, 0, 0);
	}


	BOUNDS bPanelBounds = panel.getInternalPanelBounds();
	double dPanelXLow = bPanelBounds.first.first;
	double dPanelYLow = bPanelBounds.first.second;

	// Having both BOTTOM and top Reinforcements
	if (labels.size() == 2)
	{
		int iUserValue;
		try
		{
			iUserValue = stoi(contents[iLegNo]);
		}
		catch (...)
		{
			return;
		}
		int iBotFace = -1;
		int iTopFace = -1 ;

		for (auto k = 0; k < 2; k++)
		{
			if (labels[k].find("BOT") != labels[k].npos)
			{
				iBotFace = stoi(labels[k].substr(1, 2));
			}
			else
			{
				iTopFace = stoi(labels[k].substr(1, 2));
			}
		}

		int iDiff1 = iBotFace - iUserValue;
		int iDiff2 = iTopFace - iUserValue;

		if (iDiff1 > 0 && iDiff2 > 0)
		{
			// Has both TOP and Bottom Reinforcement and Entered values are both greater than 0. subtract label text from given value and add it below

			AcDbMText* mtext = new AcDbMText;
			mtext->setLocation(connectPoints[iLegNo - 1] + labelOffset);
			mtext->setTextHeight(6.0f);
			mtext->setWidth(400);
				
			std::string tempString = "              MIN REINFORCEMENT REQUIRED FOR LIFTING\n";
			tempString += labels[0];
			tempString += " \n";
			tempString += labels[1];
			tempString += " \n";
			tempString += "                   ------------------------------------------------------------------------ \n";
			tempString += "       USE (" + to_string(iDiff1) + ")-#";
			tempString += labels[0].substr(5, 1);
			tempString += "'s BOTTOM FACE IN ADDITION TO REINFORCEMENT \n";
			tempString += "        USE (" + to_string(iDiff2) + ")-#";
			tempString += labels[1].substr(5, 1);
			tempString += "'s TOP FACE IN ADDITION TO REINFORCEMENT \n";
			tempString += "      SHOWN ON STRUCTURAL DRAWINGS. CONTRACTOR TO VERIFY \n";
			std::wstring widestr = std::wstring(tempString.begin(), tempString.end());

			mtext->setContents(widestr.c_str());

			// Labels are to be rotated only if the labels are on side
			if (iLegNo != 2 || connectPoints.size() == 2)
				mtext->setRotation(1.5707);
			pBTR->appendAcDbEntity(mtext);
			mtext->close();

			PlaceBoxAndLines(iLegNo, connectPoints,internalConnectPoints, pBTR);
		}
		else if (iDiff1 <= 0 && iDiff2 <= 0)
		{
			// Has both TOP and Bottom Reinforcement and Entered values are both less than or equal to 0
			// No Additional reinforcement required

			AcDbMText* mtext = new AcDbMText;
			mtext->setLocation(connectPoints[iLegNo - 1] + labelOffset);
			mtext->setTextHeight(6.0f);
			mtext->setWidth(400);
			

			std::string tempString = "              MIN REINFORCEMENT REQUIRED FOR LIFTING\n";
			tempString += labels[0];
			tempString += " \n";
			tempString += labels[1];
			tempString += " \n";
			tempString += "                   ------------------------------------------------------------------------ \n";
			tempString += "  NO ADDITIONAL REINFORCEMENT REQUIRED PER STRUCTURAL DRAWINGS \n";
			tempString += "                      CONTRACTOR TO VERIFY \n";
			std::wstring widestr = std::wstring(tempString.begin(), tempString.end());

			mtext->setContents(widestr.c_str());

			if (iLegNo != 2 || connectPoints.size() == 2)
				mtext->setRotation(1.5707);;
			pBTR->appendAcDbEntity(mtext);

			PlaceBoxAndLines(iLegNo, connectPoints,internalConnectPoints, pBTR);

			mtext->close();

		}

	}

	else if (labels.size() == 1)
	{
		int iUserValue;
		try
		{
			iUserValue = stoi(contents[iLegNo]);
		}
		catch (...)
		{
			return;
		}
		
		int iFace = stoi(labels[0].substr(1, 2));

		int iDiff1 = iFace - iUserValue;

		if (iDiff1 > 0 )
		{

			AcDbMText* mtext = new AcDbMText;
			mtext->setLocation(connectPoints[iLegNo - 1] + labelOffset);
			mtext->setTextHeight(6.0f);
			mtext->setWidth(400);

			std::string tempString = "              MIN REINFORCEMENT REQUIRED FOR LIFTING\n";
			tempString += labels[0];
			tempString += "                   ------------------------------------------------- \n";
			tempString += "       USE (" + to_string(iDiff1) + ")-#";
			tempString += labels[0].substr(5, 1);
			tempString += "'s";
			
			if (labels[0].find("BOT") != labels[0].npos)
			{
				tempString += " BOTTOM FACE IN ADDITION TO REINFORCEMENT \n";
			}
			else
			{
				tempString += " TOP FACE IN ADDITION TO REINFORCEMENT \n";
			}

			tempString += "      SHOWN ON STRUCTURAL DRAWINGS. CONTRACTOR TO VERIFY \n";
			std::wstring widestr = std::wstring(tempString.begin(), tempString.end());

			mtext->setContents(widestr.c_str());

			// Labels are to be rotated only if the labels are on side
			if (iLegNo != 2 || connectPoints.size() == 2)
				mtext->setRotation(1.5707);
			pBTR->appendAcDbEntity(mtext);
			mtext->close();

			PlaceBoxAndLines(iLegNo, connectPoints, internalConnectPoints,pBTR);
		}
		else if (iDiff1 <= 0 )
		{
			// Has both TOP and Bottom Reinforcement and Entered values are both less than or equal to 0
			// No Additional reinforcement required

			AcDbMText* mtext = new AcDbMText;
			mtext->setLocation(connectPoints[iLegNo - 1] + labelOffset);
			mtext->setTextHeight(6.0f);
			mtext->setWidth(400);


			std::string tempString = "              MIN REINFORCEMENT REQUIRED FOR LIFTING\n";
			tempString += labels[0];
			tempString += "                   ------------------------------------------------- \n";
			tempString += "  NO ADDITIONAL REINFORCEMENT REQUIRED PER STRUCTURAL DRAWINGS \n";
			tempString += "                      CONTRACTOR TO VERIFY \n";
			std::wstring widestr = std::wstring(tempString.begin(), tempString.end());

			mtext->setContents(widestr.c_str());

			if (iLegNo != 2 || connectPoints.size() == 2)
				mtext->setRotation(1.5707);

			pBTR->appendAcDbEntity(mtext);

			PlaceBoxAndLines(iLegNo, connectPoints,internalConnectPoints, pBTR);

			mtext->close();

		}
	}
	pBTR->close();

	return;
}

