/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 * 
 * 22-Sep-2022 Satish D			- ABA-4 - Panel Strength
 * 
 * 23-Sep-2022	SatishD			- ABA-7/8 - Lift Insert and brace Insert Clearance
 */

#include "ReportingInsertClerance.h"

void ReportingInsertClearance::InitializeHeader()
{
	headerObject.name = "Panel Name";
	headerObject.status = "status";
}

void ReportingInsertClearance::PrintHeader()
{
	PrintLine(headerObject);
}

void ReportingInsertClearance::PrintData(Panel& panel)
{
	ExcelSchema dataObject;
	UpdateExcelDataFromPanel(dataObject, panel);
	PrintLine(dataObject);
}

void ReportingInsertClearance::PrintLine(ExcelSchema& excelObject)
{
	csvfile << excelObject.name << delim;
	csvfile << excelObject.status << delim;
	csvfile << std::endl;
}

void ReportingInsertClearance::UpdateExcelDataFromPanel(ExcelSchema& excelObject, Panel& panel)
{
	excelObject.name = panel.getPanelName();

	bool bGoodPanel = true;

	BOUNDS bPanelBounds = panel.getInternalPanelBounds();
	double dPanelXLow = bPanelBounds.first.first;
	double dPanelYLow = bPanelBounds.first.second;
	double dPanelXHigh = bPanelBounds.second.first;
	double dPanelYHigh = bPanelBounds.second.second;

	std::vector<COORDINATES> vecInsertCoordinates;

	// Get the Inserts coordinated based on type
	if (m_InsertType == "LIFT_INSERTS")
	{
		for (auto& liftInserts : panel.vecLiftInserts)
		{
			vecInsertCoordinates.push_back(liftInserts.first);
		}
		for (auto& liftInserts : panel.vecEdgeLifts)
		{
			vecInsertCoordinates.push_back(liftInserts.first);
		}
	}
	else
	{
		for (auto& braceInserts : panel.vecBraceInserts)
		{
			vecInsertCoordinates.push_back(braceInserts);
		}
	}

	// Compare each insert with the panel boundary as well as the openings boundary
	for (auto& inserts : vecInsertCoordinates)
	{
		// Build a custom range for panel
		double dPanelXLow_Range = dPanelXLow + m_PanelClearance;
		double dPanelXHigh_Range = dPanelXHigh - m_PanelClearance;
		double dPanelYLow_Range = dPanelYLow + m_PanelClearance;
		double dPanelYHigh_Range = dPanelYHigh - m_PanelClearance;

		// if within panel - good else bad
		if (!(inserts.first > dPanelXLow_Range && inserts.first < dPanelXHigh_Range && inserts.second > dPanelYLow_Range && inserts.second < dPanelYHigh_Range))
		{
			bGoodPanel = false;
			break;
		}

		for (auto& opening : panel.vecOpenings)
		{
			double dLowX = opening.first.first - m_PanelClearance;
			double dHighX = opening.second.first + m_PanelClearance;
			double dLowY = opening.first.second - m_PanelClearance;
			double dHighY = opening.second.second + m_PanelClearance;

			if (inserts.first > dLowX && inserts.first < dHighX && inserts.second > dLowY && inserts.second < dHighY)
			{
				bGoodPanel = false;
				break;
			}
		}
		/*// Horizontal Distance
		if ((abs((inserts.first - dPanelXLow)) < m_PanelClearance) || (abs((inserts.first - dPanelXHigh)) < m_PanelClearance))
		{
			bGoodPanel = false;
			break;
		}

		// Vertical Distance
		if ((abs((inserts.second - dPanelYLow)) < m_PanelClearance) || (abs((inserts.second - dPanelYHigh)) < m_PanelClearance))
		{
			bGoodPanel = false;
			break;
		}*/
		/*
		for (auto& opening : panel.vecOpenings)
		{
			// Horizontal Distance
			if ((abs((inserts.first - opening.first.first)) < m_OpeningClearance) || (abs((inserts.first - opening.second.first)) < m_OpeningClearance))
			{
				bGoodPanel = false;
				break;
			}

			// Vertical Distance
			if ((abs((inserts.second - opening.first.second)) < m_OpeningClearance) || (abs((inserts.second - opening.second.second)) < m_OpeningClearance))
			{
				bGoodPanel = false;
				break;
			}
		}*/

		if (!bGoodPanel)
			break;

	}
	excelObject.status = bGoodPanel ? "Good": "Bad";
}

ReportingInsertClearance::ReportingInsertClearance(std::list<Panel>& vecPanels , std::string strInsertType, int Panelclearance, int OpeningClearance)
{
	m_InsertType = strInsertType;
	m_PanelClearance = Panelclearance;
	m_OpeningClearance = OpeningClearance;

	m_utils = Utilities::getUtils();
	mPanels = vecPanels;
	auto location = fileLocation + drawingName + "_" + strInsertType + "_Clerance_" + currentTime() + ".csv";
	csvfile.open(location, std::ios_base::app);
	InitializeHeader();
}

ReportingInsertClearance::~ReportingInsertClearance()
{
	csvfile.close();
}