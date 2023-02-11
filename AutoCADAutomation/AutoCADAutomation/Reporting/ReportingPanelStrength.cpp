/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 * 
 * 22-Sep-2022 Satish D	- ABA-4 - Panel Strength
 */

#include "ReportingPanelStrength.h"

void ReportingPanelStrength::InitializeHeader()
{
	headerObject.name = "Panel Name";
	headerObject.status = "Panel Strength";
}

void ReportingPanelStrength::PrintHeader()
{
	PrintLine(headerObject);
}

void ReportingPanelStrength::PrintData(Panel& panel)
{
	ExcelSchema dataObject;
	UpdateExcelDataFromPanel(dataObject, panel);
	PrintLine(dataObject);
}

void ReportingPanelStrength::PrintLine(ExcelSchema& excelObject)
{
	csvfile << excelObject.name << delim;
	csvfile << excelObject.status << delim;
	csvfile << std::endl;
}

void ReportingPanelStrength::UpdateExcelDataFromPanel(ExcelSchema& excelObject, Panel& panel)
{
	excelObject.name = panel.getPanelName();

	BOUNDS bPanelBounds = panel.getInternalPanelBounds();
	
	double dPanelXLow = bPanelBounds.first.first;
	double dPanelYLow = bPanelBounds.first.second;
	double dPanelXHigh = bPanelBounds.second.first;
	double dPanelYHigh = bPanelBounds.second.second;
	bool bWeakPanel = false;
	COORDINATES cgPoint = panel.GetCG().first;

	//Panel strength computation
	for (auto& opening : panel.vecOpenings)
	{
		// Check if the panel is coinciding the bottom edge.
		if (m_utils->approximatelyEqual(opening.first.second, dPanelYLow) || (opening.first.second < dPanelYLow))
		{
			if (m_utils->approximatelyEqual(opening.first.first, dPanelXLow) || m_utils->approximatelyEqual(opening.second.first , dPanelXHigh))
			{
				// if opening width is greater than 40% of bottom leg width - weak panel
				if ((opening.first.first - opening.second.first) / (dPanelXLow - dPanelXHigh) > 0.4)
				{
					bWeakPanel = true;
					break;
				}
			}
			
			// compute the opening with 1 feet extended side 
			BOUNDS oneFeetExtendedBound = opening;
			oneFeetExtendedBound.first.first -= 12;
			oneFeetExtendedBound.second.first += 12;
			oneFeetExtendedBound.second.second += 12;

			if (m_utils->boundCheck(oneFeetExtendedBound, cgPoint))
			{
				bWeakPanel = true;
				break;
			}
		}
	}
	excelObject.status = bWeakPanel? "Weak": "Strong";
}

ReportingPanelStrength::ReportingPanelStrength(std::list<Panel>& vecPanels)
{
	m_utils = Utilities::getUtils();
	mPanels = vecPanels;
	auto location = fileLocation + drawingName + "_PanelStrength_" + currentTime() + ".csv";
	csvfile.open(location, std::ios_base::app);
	InitializeHeader();
}

ReportingPanelStrength::~ReportingPanelStrength()
{
	csvfile.close();
}