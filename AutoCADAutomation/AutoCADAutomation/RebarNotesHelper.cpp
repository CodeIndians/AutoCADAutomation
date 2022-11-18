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
		vector<AcGePoint3d>connectPoints;

		ComputeLabelConnectPoints(panel, connectPoints);

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
						ProcessRebars(rebarLabels, m_VecContent[iPanel], iLegNo, panel, connectPoints);

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
					ProcessRebars(rebarLabels, m_VecContent[iPanel], iLegNo, panel, connectPoints);
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

void RebarNotesHelper::ComputeLabelConnectPoints(Panel& panel, vector<AcGePoint3d>& connectPoints)
{
	BOUNDS bPanelBounds = panel.getInternalPanelBounds();
	double dPanelXLow = bPanelBounds.first.first;
	double dPanelYLow = bPanelBounds.first.second;


	connectPoints.push_back(AcGePoint3d((dPanelXLow - 198), (dPanelYLow + 108), 0));
	//connectPoints.push_back(AcGePoint3d((dPanelXLow - 202), (dPanelYLow + 95), 0));
	//connectPoints.push_back(AcGePoint3d((dPanelXLow - 120), (dPanelYLow + 414), 0));
	//connectPoints.push_back(AcGePoint3d((dPanelXLow + 18), (dPanelYLow + 95), 0));

	connectPoints.push_back(AcGePoint3d((dPanelXLow - 78), (dPanelYLow - 45), 0));
	//connectPoints.push_back(AcGePoint3d((dPanelXLow - 87), (dPanelYLow - 42), 0));
	//connectPoints.push_back(AcGePoint3d((dPanelXLow + 223), (dPanelYLow -121), 0));
	//connectPoints.push_back(AcGePoint3d((dPanelXLow + 223), (dPanelYLow + 71), 0));

	connectPoints.push_back(AcGePoint3d((dPanelXLow + 413), (dPanelYLow + 108), 0));
	//connectPoints.push_back(AcGePoint3d((dPanelXLow + 403), (dPanelYLow + 95), 0));
	//connectPoints.push_back(AcGePoint3d((dPanelXLow + 486), (dPanelYLow + 416), 0));
	//connectPoints.push_back(AcGePoint3d((dPanelXLow + 301), (dPanelYLow + 95), 0));

	// Rectangles

}

void RebarNotesHelper::PlaceBoxAndLines(int iLegNo, double dPanelXLow, double dPanelYLow, AcDbBlockTableRecordPointer& pBTR)
{
	switch (iLegNo) {

	case 1:
	{
		AcDbLine * pLine1 = new AcDbLine();
		pLine1->setStartPoint(AcGePoint3d((dPanelXLow - 202), (dPanelYLow + 95), 0));
		pLine1->setEndPoint(AcGePoint3d((dPanelXLow - 202), (dPanelYLow + 414), 0));
		pBTR->appendAcDbEntity(pLine1);
		pLine1->close();

		AcDbLine* pLine2 = new AcDbLine();
		pLine2->setStartPoint(AcGePoint3d((dPanelXLow - 202), (dPanelYLow + 414), 0));
		pLine2->setEndPoint(AcGePoint3d((dPanelXLow - 120), (dPanelYLow + 414), 0));
		pBTR->appendAcDbEntity(pLine2);
		pLine2->close();

		AcDbLine* pLine3 = new AcDbLine();
		pLine3->setStartPoint(AcGePoint3d((dPanelXLow - 120), (dPanelYLow + 414), 0));
		pLine3->setEndPoint(AcGePoint3d((dPanelXLow - 120), (dPanelYLow + 95), 0));
		pBTR->appendAcDbEntity(pLine3);
		pLine3->close();

		AcDbLine* pLine4 = new AcDbLine();
		pLine4->setStartPoint(AcGePoint3d((dPanelXLow - 202), (dPanelYLow + 95), 0));
		pLine4->setEndPoint(AcGePoint3d((dPanelXLow + 18), (dPanelYLow + 95), 0));
		pBTR->appendAcDbEntity(pLine4);
		pLine4->close();

		AcDbLine* pLine5 = new AcDbLine();
		pLine5->setStartPoint(AcGePoint3d((dPanelXLow + 16), (dPanelYLow + 93), 0));
		pLine5->setEndPoint(AcGePoint3d((dPanelXLow + 20), (dPanelYLow + 97), 0));
		pBTR->appendAcDbEntity(pLine5);
		pLine5->close();
	}
		break;

	case 2:
	{
		AcDbLine * pLine1 = new AcDbLine();
		pLine1->setStartPoint(AcGePoint3d((dPanelXLow - 87), (dPanelYLow - 42), 0));
		pLine1->setEndPoint(AcGePoint3d((dPanelXLow - 87), (dPanelYLow - 121), 0));
		pBTR->appendAcDbEntity(pLine1);
		pLine1->close();

		AcDbLine* pLine2 = new AcDbLine();
		pLine2->setStartPoint(AcGePoint3d((dPanelXLow - 87), (dPanelYLow - 121), 0));
		pLine2->setEndPoint(AcGePoint3d((dPanelXLow + 223), (dPanelYLow - 121), 0));
		pBTR->appendAcDbEntity(pLine2);
		pLine2->close();

		AcDbLine* pLine3 = new AcDbLine();
		pLine3->setStartPoint(AcGePoint3d((dPanelXLow + 223), (dPanelYLow - 121), 0));
		pLine3->setEndPoint(AcGePoint3d((dPanelXLow + 223), (dPanelYLow + 71), 0));
		pBTR->appendAcDbEntity(pLine3);
		pLine3->close();

		AcDbLine* pLine4 = new AcDbLine();
		pLine4->setStartPoint(AcGePoint3d((dPanelXLow - 87), (dPanelYLow - 42), 0));
		pLine4->setEndPoint(AcGePoint3d((dPanelXLow + 223), (dPanelYLow - 42), 0));
		pBTR->appendAcDbEntity(pLine4);
		pLine4->close();

		AcDbLine* pLine5 = new AcDbLine();
		pLine5->setStartPoint(AcGePoint3d((dPanelXLow + 221), (dPanelYLow + 69), 0));
		pLine5->setEndPoint(AcGePoint3d((dPanelXLow + 225), (dPanelYLow + 73), 0));
		pBTR->appendAcDbEntity(pLine5);
		pLine5->close();
	}
		break;

	case 3:
	{
		AcDbLine* pLine1 = new AcDbLine();
		pLine1->setStartPoint(AcGePoint3d((dPanelXLow + 403), (dPanelYLow + 95), 0));
		pLine1->setEndPoint(AcGePoint3d((dPanelXLow + 403), (dPanelYLow + 414), 0));
		pBTR->appendAcDbEntity(pLine1);
		pLine1->close();

		AcDbLine* pLine2 = new AcDbLine();
		pLine2->setStartPoint(AcGePoint3d((dPanelXLow + 403), (dPanelYLow + 414), 0));
		pLine2->setEndPoint(AcGePoint3d((dPanelXLow + 486), (dPanelYLow + 414), 0));
		pBTR->appendAcDbEntity(pLine2);
		pLine2->close();

		AcDbLine* pLine3 = new AcDbLine();
		pLine3->setStartPoint(AcGePoint3d((dPanelXLow + 486), (dPanelYLow + 414), 0));
		pLine3->setEndPoint(AcGePoint3d((dPanelXLow + 486), (dPanelYLow + 95), 0));
		pBTR->appendAcDbEntity(pLine3);
		pLine3->close();

		AcDbLine* pLine4 = new AcDbLine();
		pLine4->setStartPoint(AcGePoint3d((dPanelXLow + 486), (dPanelYLow + 95), 0));
		pLine4->setEndPoint(AcGePoint3d((dPanelXLow + 301), (dPanelYLow + 95), 0));
		pBTR->appendAcDbEntity(pLine4);
		pLine4->close();

		AcDbLine* pLine5 = new AcDbLine();
		pLine5->setStartPoint(AcGePoint3d((dPanelXLow + 299), (dPanelYLow + 93), 0));
		pLine5->setEndPoint(AcGePoint3d((dPanelXLow + 303), (dPanelYLow + 97), 0));
		pBTR->appendAcDbEntity(pLine5);
		pLine5->close();
	}
		break;

	default:
		break;
	}
}

void RebarNotesHelper::ProcessRebars(vector<string> labels, vector<string> contents, int iLegNo, Panel& panel, vector<AcGePoint3d>& connectPoints)
{

	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTableRecordPointer pBTR
	(
		acdbSymUtil()->blockModelSpaceId(pDb),
		AcDb::kForWrite
	);


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
			mtext->setLocation(connectPoints[iLegNo - 1]);
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
			if (iLegNo != 2)
				mtext->setRotation(1.5707);
			pBTR->appendAcDbEntity(mtext);
			mtext->close();

			PlaceBoxAndLines(iLegNo, dPanelXLow, dPanelYLow, pBTR);
		}
		else if (iDiff1 <= 0 && iDiff2 <= 0)
		{
			// Has both TOP and Bottom Reinforcement and Entered values are both less than or equal to 0
			// No Additional reinforcement required

			AcDbMText* mtext = new AcDbMText;
			mtext->setLocation(AcGePoint3d((dPanelXLow - 198), (dPanelYLow + 108), 0));
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

			if (iLegNo != 2)
				mtext->setRotation(1.5707);;
			pBTR->appendAcDbEntity(mtext);

			PlaceBoxAndLines(iLegNo, dPanelXLow, dPanelYLow, pBTR);

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
			mtext->setLocation(connectPoints[iLegNo - 1]);
			mtext->setTextHeight(6.0f);
			mtext->setWidth(400);

			std::string tempString = "              MIN REINFORCEMENT REQUIRED FOR LIFTING\n";
			tempString += labels[0];
			tempString += " \n";
			tempString += "                   ------------------------------------------------------------------------ \n";
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
			if (iLegNo != 2)
				mtext->setRotation(1.5707);
			pBTR->appendAcDbEntity(mtext);
			mtext->close();

			PlaceBoxAndLines(iLegNo, dPanelXLow, dPanelYLow, pBTR);
		}
		else if (iDiff1 <= 0 )
		{
			// Has both TOP and Bottom Reinforcement and Entered values are both less than or equal to 0
			// No Additional reinforcement required

			AcDbMText* mtext = new AcDbMText;
			mtext->setLocation(AcGePoint3d((dPanelXLow - 198), (dPanelYLow + 108), 0));
			mtext->setTextHeight(6.0f);
			mtext->setWidth(400);


			std::string tempString = "              MIN REINFORCEMENT REQUIRED FOR LIFTING\n";
			tempString += labels[0];
			tempString += " \n";
			tempString += "                   ------------------------------------------------------------------------ \n";
			tempString += "  NO ADDITIONAL REINFORCEMENT REQUIRED PER STRUCTURAL DRAWINGS \n";
			tempString += "                      CONTRACTOR TO VERIFY \n";
			std::wstring widestr = std::wstring(tempString.begin(), tempString.end());

			mtext->setContents(widestr.c_str());

			if (iLegNo != 2)
				mtext->setRotation(1.5707);

			pBTR->appendAcDbEntity(mtext);

			PlaceBoxAndLines(iLegNo, dPanelXLow, dPanelYLow, pBTR);

			mtext->close();

		}
	}
	pBTR->close();

	return;
}

