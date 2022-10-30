
/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 25-Oct-2022	SatishD	- Initial Creation
 */

#pragma once
#include "ReportingBase.h"
#include <vector>
class ReportingRevealsClearance : public ReportingBase
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

public:
	explicit ReportingRevealsClearance(std::list<Panel>& vecPanels);
	~ReportingRevealsClearance();
};
