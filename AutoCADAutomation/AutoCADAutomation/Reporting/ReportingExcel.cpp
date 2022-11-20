/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 * 
 * 22-Sep-2022 Satish D - Panels having 7 Brace Inserts
 * 
 */

#include "ReportingExcel.h"

void ReportingExcel::InitializeHeader()
{
	headerObject.name = "Panel Name";
	headerObject.panelWidth = "Panel Width";
	headerObject.panelHeight = "Panel Height Below F.F" + delim + "Panel Height Above F.F" + delim+ "Over all Panel Height";
	headerObject.thickness = "Panel Thk";
	for (int i = 0; i < 6; i++)
	{
		std::string openingName = "Opening " + std::to_string(i+1);
		headerObject.openings[i] = openingName + "Type" + delim + openingName + " W" + delim + openingName + " H" + delim + openingName + " X Loc" + delim + openingName + "Y Loc";
	}
	headerObject.riggingType = "Rigging Type";
	for (int i = 0; i < 16; i++)
	{
		std::string num  = std::to_string(i + 1);
		std::string lift = "Lift Insert ( L" + num + " )";
		headerObject.liftInserts[i] = lift + "X Loc" + delim +lift + "Y Loc";
	}
	for (int i = 0; i < 7; i++)
	{
		std::string num = std::to_string(i + 1);
		std::string brace = "Brace Insert ( B" + num + " )";
		headerObject.braceInserts[i] = brace + "X Loc" + delim + brace + "Y Loc";
	}
	for (int i = 0; i < 20; i++)
	{
		std::string reveal = "Reveal " + std::to_string(i + 1);
		headerObject.reveals[i] = reveal + " X1" + delim + reveal + " Y1" + delim + reveal + " X2" + delim + reveal + "Y2";
	}
}

void ReportingExcel::PrintHeader()
{
	PrintLine(headerObject);
}

void ReportingExcel::PrintData(Panel &panel)
{
	ExcelSchema dataObject;
	UpdateExcelDataFromPanel(dataObject, panel);
	PrintLine(dataObject);
}

void ReportingExcel::PrintLine(ExcelSchema& excelObject)
{
	csvfile << excelObject.name << delim;
	csvfile << excelObject.panelWidth << delim;
	csvfile << excelObject.panelHeight << delim;
	csvfile << excelObject.thickness << delim;
	for (int i = 0; i < 6; i++)
	{
		csvfile << excelObject.openings[i] << delim;
	}
	csvfile << excelObject.riggingType << delim;
	for (int i = 0; i < 16; i++)
	{
		csvfile << excelObject.liftInserts[i] << delim;
	}
	for (int i = 0; i < 7; i++)
	{
		csvfile << excelObject.braceInserts[i] << delim;
	}
	for (int i = 0; i < 20; i++)
	{
		csvfile << excelObject.reveals[i] << delim;
	}
	csvfile << std::endl;
}

void ReportingExcel::UpdateExcelDataFromPanel(ExcelSchema& excelObject, Panel& panel)
{
	excelObject.name = panel.getPanelName();
	excelObject.thickness = panel.getPanelThickNess();
	excelObject.riggingType = panel.getRiggingType();
	excelObject.panelWidth = Utilities::getUtils()->inchesToFeet(panel.getPanelWidth());
	excelObject.panelHeight = Utilities::getUtils()->inchesToFeet(panel.getPanelHeightBelowFF()) + delim + Utilities::getUtils()->inchesToFeet(panel.getPanelHeightAboveFF()) + delim + Utilities::getUtils()->inchesToFeet(panel.getPanelHeight());
	int index = 0;
	for (auto& opening : panel.vecOpenings)
	{
		excelObject.openings[index] = panel.getOpeningType(opening) + delim + Utilities::getUtils()->inchesToFeet(Utilities::getUtils()->getBoundsWidth(opening)) + delim + Utilities::getUtils()->inchesToFeet(Utilities::getUtils()->getBoundsHeight(opening)) + delim + std::to_string(opening.first.first - panel.getInternalPanelBounds().first.first) + delim + std::to_string(opening.first.second - panel.getInternalPanelBounds().first.second);
		index++;
	}
	index = 0;
	for (auto& liftInsert : panel.vecLiftInserts)
	{
		excelObject.liftInserts[index] = std::to_string(liftInsert.first.first - panel.getInternalPanelBounds().first.first) + delim + std::to_string(liftInsert.first.second - panel.getInternalPanelBounds().first.second);
		index++;
	}
	index = 0;
	for (auto& braceInsert : panel.vecBraceInserts)
	{
		excelObject.braceInserts[index] = std::to_string(braceInsert.first - panel.getInternalPanelBounds().first.first) + delim + std::to_string(braceInsert.second - panel.getInternalPanelBounds().first.second);
		index++;
	}
	index = 0;
	for (auto& reveal : panel.vecReveals)
	{
		excelObject.reveals[index] = std::to_string(reveal.first.first - panel.getInternalPanelBounds().first.first) + delim + std::to_string(reveal.first.second - panel.getInternalPanelBounds().first.second) + delim + std::to_string(reveal.second.first - panel.getInternalPanelBounds().first.first) + delim + std::to_string(reveal.second.second - panel.getInternalPanelBounds().first.second);
		index++;
	}
}

ReportingExcel::ReportingExcel(std::list<Panel>& vecPanels)
{
	mPanels = vecPanels;
	auto location = fileLocation + drawingName + "_AbsolutePositions_" + currentTime() + ".csv";
	csvfile.open(location, std::ios_base::app);
	InitializeHeader();
}

ReportingExcel::~ReportingExcel()
{
	csvfile.close();
}
