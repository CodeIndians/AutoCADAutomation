/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 */

#include "ReportingInterference.h"

void ReportingInterference::InitializeHeader()
{
	headerObject.name = "Panel Name";
	headerObject.status = "Interference Status";
}

void ReportingInterference::PrintHeader()
{
	PrintLine(headerObject);
}

void ReportingInterference::PrintData(Panel& panel)
{
	ExcelSchema dataObject;
	UpdateExcelDataFromPanel(dataObject, panel);
	PrintLine(dataObject);
}

void ReportingInterference::PrintLine(ExcelSchema& excelObject)
{
	csvfile << excelObject.name << delim;
	csvfile << excelObject.status << delim;
	csvfile << std::endl;
}

void ReportingInterference::UpdateExcelDataFromPanel(ExcelSchema& excelObject, Panel& panel)
{
	excelObject.name = panel.getPanelName();
	excelObject.status = panel.isInterferenceDetected() ? "Bad" : "Good";
}

ReportingInterference::ReportingInterference(std::list<Panel>& vecPanels)
{
	mPanels = vecPanels;
	auto location = fileLocation + drawingName + "_Inserts_Collision_" + currentTime() + ".csv";
	csvfile.open(location, std::ios_base::app);
	InitializeHeader();
}

ReportingInterference::~ReportingInterference()
{
	csvfile.close();
}
