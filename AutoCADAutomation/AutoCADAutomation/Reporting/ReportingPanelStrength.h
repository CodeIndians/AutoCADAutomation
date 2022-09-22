
/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 * 
 * 22-Sep-2022	Satish D - ABA-4 - Panel Strength
 */

#pragma once
#include "ReportingBase.h"
class ReportingPanelStrength : public ReportingBase
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
	Utilities* m_utils;;
public:
	explicit ReportingPanelStrength(std::list<Panel>& vecPanels);
	~ReportingPanelStrength();
};
