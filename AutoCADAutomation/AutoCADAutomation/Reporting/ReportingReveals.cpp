/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 25-Oct-2022	SatishD/Raghu	- Initial Creation
 * 
 */

#include "ReportingReveals.h"

void ReportingReveals::InitializeHeader()
{
	headerObject.name = "Panel Name";
	for (int i = 0; i < 20; i++)
	{
		std::string reveal = "Reveal " + std::to_string(i + 1);
		headerObject.reveals[i] = reveal + " X1" + delim + reveal + " Y1" + delim + reveal + " X2" + delim + reveal + "Y2";
	}
}

void ReportingReveals::PrintHeader()
{
	PrintLine(headerObject);
}

void ReportingReveals::PrintData(Panel &panel)
{
	ExcelSchema dataObject;
	UpdateExcelDataFromPanel(dataObject, panel);
	PrintLine(dataObject);
}

void ReportingReveals::PrintLine(ExcelSchema& excelObject)
{
	csvfile << excelObject.name << delim;
	for (int i = 0; i < 20; i++)
	{
		csvfile << excelObject.reveals[i] << delim;
	}

	csvfile << std::endl;
}

void ReportingReveals::UpdateExcelDataFromPanel(ExcelSchema& excelObject, Panel& panel)
{
	excelObject.name = panel.getPanelName();
	int index = 0;
	for (auto& reveal : panel.vecReveals)
	{
		excelObject.reveals[index] = std::to_string(reveal.first.first ) + delim + std::to_string (reveal.first.second) +  delim + std::to_string(reveal.second.first) + delim + std::to_string ( reveal.second.second);
		index++;
	}
}

ReportingReveals::ReportingReveals(std::list<Panel>& vecPanels)
{
	mPanels = vecPanels;
	auto location = fileLocation + drawingName + "_RevealsInformation_" + currentTime() + ".csv";
	csvfile.open(location, std::ios_base::app);
	InitializeHeader();
}

ReportingReveals::~ReportingReveals()
{
	csvfile.close();
}
