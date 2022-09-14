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
	UpdateExcelDataFromPanel(dataObject, panel);
	PrintLine(dataObject);
}

void ReportingUnroundedLiftInserts::PrintLine(ExcelSchema& excelObject)
{
	csvfile << excelObject.name << delim;
	csvfile << excelObject.status << delim;
	csvfile << std::endl;
}

void ReportingUnroundedLiftInserts::UpdateExcelDataFromPanel(ExcelSchema& excelObject, Panel& panel)
{
	std::vector<std::string> fractionalLiftInsertDims;
	COORDINATES midPoint1, midPoint2, diffPoint;
	std::string diffInFeet;
	int dim1 = 0, dim2 = 1;

	try
	{
		excelObject.status = "Good";
		excelObject.name = panel.getPanelName();
		/*for (dim1 = 0, dim2 = 1; dim2 <= panel.vecLiftDimHorPoints.size() - 1; dim1++, dim2++)
		{*/
			if (panel.vecLiftDimHorPoints.size() != 0 && panel.vecLiftDimHorPoints.size() != 1)
			{
				midPoint1 = Utilities::getUtils()->getMidPoint(panel.vecLiftDimHorPoints[dim1]);
				midPoint2 = Utilities::getUtils()->getMidPoint(panel.vecLiftDimHorPoints[dim2]);
				diffPoint.first = midPoint2.first - midPoint1.first;
				diffPoint.second = midPoint2.second - midPoint1.second;
				diffInFeet = Utilities::getUtils()->inchesToFeet(diffPoint.first);
				if (diffInFeet.find('/') != std::string::npos)
				{
					fractionalLiftInsertDims.push_back(diffInFeet);
					excelObject.status = "Bad";
					/*break;*/
				}
			}
		/*}*/
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