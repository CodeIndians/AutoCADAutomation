/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 25-Oct-2022	SatishD	- Initial Creation
 * 
 */

#include "ReportingRevealsClearance.h"

void ReportingRevealsClearance::InitializeHeader()
{
	headerObject.name = "Panel Name";
	headerObject.status = "Reveal-Insert Clearance Status";
}

void ReportingRevealsClearance::PrintHeader()
{
	PrintLine(headerObject);
}

void ReportingRevealsClearance::PrintData(Panel& panel)
{
	ExcelSchema dataObject;
	UpdateExcelDataFromPanel(dataObject, panel);
	PrintLine(dataObject);
}

void ReportingRevealsClearance::PrintLine(ExcelSchema& excelObject)
{
	csvfile << excelObject.name << delim;
	csvfile << excelObject.status << delim;
	csvfile << std::endl;
}

void ReportingRevealsClearance::UpdateExcelDataFromPanel(ExcelSchema& excelObject, Panel& panel)
{
	excelObject.name = panel.getPanelName();

	bool bGoodPanel = true;

	std::vector<COORDINATES> vecInsertCoordinates;

	for (auto& liftInserts : panel.vecLiftInserts)
	{
		vecInsertCoordinates.push_back(liftInserts.first);
	}

	for (auto& liftInserts : panel.vecEdgeLifts)
	{
		vecInsertCoordinates.push_back(liftInserts.first);
	}

	for (auto& braceInserts : panel.vecBraceInserts)
	{
		vecInsertCoordinates.push_back(braceInserts);
	}

	// Compare each insert with each reveal on the panel
	for (auto& inserts : vecInsertCoordinates)
	{

		for (auto& reveal : panel.vecReveals)
		{
			// Vertical Distance
			if ((abs((inserts.second - reveal.first.second)) < 9) || (abs((inserts.second - reveal.second.second)) < 9))
			{
				bGoodPanel = false;
				break;
			}
		}

		if (!bGoodPanel)
			break;

	}
	excelObject.status = bGoodPanel ? "Good" : "Bad";
}

ReportingRevealsClearance::ReportingRevealsClearance(std::list<Panel>& vecPanels)
{
	m_utils = Utilities::getUtils();
	mPanels = vecPanels;
	auto location = fileLocation + drawingName + "_"  + "Reveals_Clerance_" + currentTime() + ".csv";
	csvfile.open(location, std::ios_base::app);
	InitializeHeader();
}

ReportingRevealsClearance::~ReportingRevealsClearance()
{
	csvfile.close();
}