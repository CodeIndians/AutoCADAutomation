/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 13-Sep-2022	Slanka	- ABA-6: Round off lifting insert to 1”
 */

#pragma once
#include "ReportingBase.h"

class ReportingUnroundedLiftInserts : public ReportingBase
{
private:
	ofstream csvfile;
	struct ExcelSchema
	{
		std::string name;
		std::string status;
	};
	ExcelSchema headerObject;
protected:
	void InitializeHeader();
	void PrintHeader();
	void PrintData(Panel& panel);
	void PrintLine(ExcelSchema& excelObject);
	void UpdateExcelDataFromPanel(ExcelSchema& excelObject, Panel& panel);
	void UpdateExcelDataFromPanelAfterReconnect(ExcelSchema& excelObject, Panel& panel);
public:
	explicit ReportingUnroundedLiftInserts(std::list<Panel>& vecPanels);
	~ReportingUnroundedLiftInserts();
};