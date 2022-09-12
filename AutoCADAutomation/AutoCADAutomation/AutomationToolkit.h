/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 *  
 * 11-Sep-2022	SatishD		- Added Skeleton implementations for new commands
 */

#pragma once
#include<vector>
#include "PanelBuilder.h"
#include "Reporting/ReportingExcel.h"
#include "Reporting/ReportingInterference.h"
#include "Reporting/ReportingRebar.h"

class AutomationToolkit
{

public:
	static void AbsolutePositions();
	static void InterferenceCheck();
	static void RebarLabelsCheck();
	static void DimensionsReconnect();

	//Phase-2 Methods - START
	static void PanelDataLabels();
	static void IdentifySimilarRigging();
	static void PanelStrength();
	static void LiftAndEgdeNotes();
	static void LiftInsDimRoundOff();
	static void LiftInsClearance();
	static void BraceInsClearance();
	static void RebarLabelsDeadmanNotes();
	static void RevealPostions();
	static void InsertsDistFromReveals();
	//Phase-2 Methods - END

	static void CollectPanelInformation( bool dimensions = false);
	static std::list<Panel> vecPanels;
};

