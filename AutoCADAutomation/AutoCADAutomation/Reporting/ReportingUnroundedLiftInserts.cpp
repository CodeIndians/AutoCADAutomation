/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 13-Sep-2022	Slanka	- ABA-6: Round off lifting insert to 1”
 */

#include "ReportingUnroundedLiftInserts.h"

void ReportingUnroundedLiftInserts::InitializeHeader()
{
	headerObject.name = "Panel Name";
	headerObject.status = "RoundOff Lifting Insert Status";
}

void ReportingUnroundedLiftInserts::PrintHeader()
{
	PrintLine(headerObject);
}

void ReportingUnroundedLiftInserts::PrintData(Panel& panel)
{
	ExcelSchema dataObject;
	if(panel.vecLiftDimHorPoints.size() == 0)
		UpdateExcelDataFromPanelAfterReconnect(dataObject, panel);
	else
		UpdateExcelDataFromPanel(dataObject, panel);

	PrintLine(dataObject);
}

void ReportingUnroundedLiftInserts::PrintLine(ExcelSchema& excelObject)
{
	csvfile << excelObject.name << delim;
	csvfile << excelObject.status << delim;
	csvfile << std::endl;
}

void ReportingUnroundedLiftInserts::UpdateExcelDataFromPanelAfterReconnect(ExcelSchema& excelObject, Panel& panel)
{
	COORDINATES midPoint1, midPoint2, diffPoint;
	std::string diffInFeet;
	std::vector<COORDINATES> tmpvecLiftDimHorPoints;

	try
	{
		excelObject.name = panel.getPanelName();

		COORDINATES elem1, elem2;
		BOUNDS boundElem;

		// Collect all the midpoints of the angled lines from vecLiftDimHorPoints list
		std::vector<COORDINATES> midPoints;
		std::vector<COORDINATES> dimLines;

		for (int dim = 0; dim < panel.vecLiftDimHorPointsAfterReconnect.size(); dim++)
		{
			elem1 = std::make_pair(panel.vecLiftDimHorPointsAfterReconnect[dim]->xLine1Point().x, panel.vecLiftDimHorPointsAfterReconnect[dim]->xLine1Point().y);
			elem2 = std::make_pair(panel.vecLiftDimHorPointsAfterReconnect[dim]->dimLinePoint().x, panel.vecLiftDimHorPointsAfterReconnect[dim]->dimLinePoint().y);
			midPoints.push_back(elem1);
		}

		if (midPoints.size() == 0 || midPoints.size() == 1)
		{
			excelObject.status = "Good";
			return;
		}

		// Sort all the mid-points based on the X coordinates in ascending order
		std::sort(midPoints.begin(), midPoints.end());

		// Now get the rotated dimension along with its measurement which has the most left X coordinates
		double valInches = 0.00f;
		for (int dim = 0; dim < panel.vecLiftDimHorPointsAfterReconnect.size(); dim++)
		{
			if (midPoints[0].first == panel.vecLiftDimHorPointsAfterReconnect[dim]->xLine1Point().x &&
				midPoints[0].second == panel.vecLiftDimHorPointsAfterReconnect[dim]->xLine1Point().y)
			{
				panel.vecLiftDimHorPointsAfterReconnect[dim]->measurement(valInches);
				break;
			}
		}

		// Convert the inches into feet and mark the panel as good or bad
		diffInFeet = Utilities::getUtils()->inchesToFeet(valInches);
		if (diffInFeet.find('/') != std::string::npos)
			excelObject.status = "Bad";
		else
			excelObject.status = "Good";
	}
	catch (...)
	{
		acutPrintf(L"Update of round-off lifting inserts failed");
	}
}

void ReportingUnroundedLiftInserts::UpdateExcelDataFromPanel(ExcelSchema& excelObject, Panel& panel)
{
	std::vector<std::string> fractionalLiftInsertDims;
	COORDINATES midPoint1, midPoint2, diffPoint;
	std::string diffInFeet;
	int dim1 = 0, dim2 = 1;

	try
	{
		excelObject.name = panel.getPanelName();

		// Collect all the midpoints of the angled lines from vecLiftDimHorPoints list
		std::vector<COORDINATES> midPoints;
		std::vector<double> midPointYs;

		for (int line = 0; line < panel.vecLiftDimHorPoints.size(); line++)
		{
			midPoints.push_back(Utilities::getUtils()->getMidPoint(panel.vecLiftDimHorPoints[line]));
			midPointYs.push_back(Utilities::getUtils()->getMidPoint(panel.vecLiftDimHorPoints[line]).second);
		}

		if (midPoints.size() == 0 || midPoints.size() == 1 || midPointYs.size() == 0 || midPointYs.size() == 1)
		{
			excelObject.status = "Good";
			return;
		}

		// Sort all the mid-points based on the X coordinates in ascending order
		std::sort(midPoints.begin(), midPoints.end());

		// Now extract all the Ys and sort them in descending order
		std::sort(midPointYs.rbegin(), midPointYs.rend());

		// Now get the two COORDINATES which has the largest Y coordinates and smallest X coordinates
		int firstPairIndex = -1, secondPairIndex = -1;
		for (int x = 0; x < midPoints.size(); x++)
		{
			if (firstPairIndex != -1 && secondPairIndex != -1)
				break;

			if (midPoints[x].second == midPointYs[0])
			{
				if (firstPairIndex == -1)
				{
					firstPairIndex = x;
					continue;
				}
				
				if (firstPairIndex != -1 && secondPairIndex == -1)
				{
					secondPairIndex = x;
					continue;
				}
			}
		}

		// Now, calculate the distance between these 2 pairs and check if it is a fractional value
		diffPoint.first = midPoints[secondPairIndex].first - midPoints[firstPairIndex].first;
		diffPoint.second = midPoints[secondPairIndex].second - midPoints[firstPairIndex].second;

		diffInFeet = Utilities::getUtils()->inchesToFeet(diffPoint.first);
		if (diffInFeet.find('/') != std::string::npos)
			excelObject.status = "Bad";
		else
			excelObject.status = "Good";
	}
	catch (...)
	{
		acutPrintf(L"Update of round-off lifting inserts failed");
	}
}

ReportingUnroundedLiftInserts::ReportingUnroundedLiftInserts(std::list<Panel>& vecPanels)
{
	mPanels = vecPanels;
	auto location = fileLocation + drawingName + "_RoundOffLiftInserts_" + currentTime() + ".csv";
	csvfile.open(location, std::ios_base::app);
	InitializeHeader();
}

ReportingUnroundedLiftInserts::~ReportingUnroundedLiftInserts()
{
	csvfile.close();
}