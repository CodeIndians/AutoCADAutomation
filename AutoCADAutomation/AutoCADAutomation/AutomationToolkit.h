/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
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
	static void CollectPanelInformation( bool dimensions = false);
	static std::list<Panel> vecPanels;
};

