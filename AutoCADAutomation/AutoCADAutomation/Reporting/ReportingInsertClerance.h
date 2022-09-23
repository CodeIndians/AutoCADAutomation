
/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 * 
 * 22-Sep-2022	Satish D		- ABA-7/8 - Insert Clearance
 *
 * 23-Sep-2022	SatishD			- ABA-7/8 - Lift Insert and brace Insert Clearance
 */

#pragma once
#include "ReportingBase.h"
#include <vector>
class ReportingInsertClearance : public ReportingBase
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
	Utilities* m_utils;
	std::string m_InsertType;
	int m_PanelClearance;
	int m_OpeningClearance;

public:
	explicit ReportingInsertClearance(std::list<Panel>& vecPanels , std::string strInsertType, int Panelclearance, int OpeningClearance);
	~ReportingInsertClearance();
};
