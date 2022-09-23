/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 * 11-Sep-2022	SatishD		    - Added Skeleton implementations for new commands
 * 13-Sep-2022	Slanka		    - ABA-6: Round off lifting insert to 1”
 * 22-Sep-2022	Satish D		- ABA-4 - Panel Strength
 * 23-Sep-2022	SatishD			- ABA-7/8 - Lift Insert and brace Insert Clearance
 */

#pragma once
#include<vector>
#include "PanelBuilder.h"
#include "Reporting/ReportingExcel.h"
#include "Reporting/ReportingInterference.h"
#include "Reporting/ReportingRebar.h"
#include "Reporting/ReportingUnroundedLiftInserts.h"
#include "Reporting/ReportingPanelStrength.h"
#include "Reporting/ReportingInsertClerance.h"
#include "InputBox.h"
#include <string>


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

