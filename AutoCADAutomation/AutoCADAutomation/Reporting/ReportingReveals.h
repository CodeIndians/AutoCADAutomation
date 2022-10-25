/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 * 
 * 25-Oct-2022 Satish D - Intial creation
 * 
 */

#pragma once
#include "ReportingBase.h"

class ReportingReveals :public ReportingBase
{
private:
	ofstream csvfile;
	struct ExcelSchema
	{
		std::string name;
		std::string reveals[20];
		ExcelSchema() : name("")
		{
			for (int i = 0; i < 20; i++)
				reveals[i] = ",,,";
		}
	};
	ExcelSchema headerObject;
protected:
	void InitializeHeader();
	void PrintHeader();
	void PrintData(Panel &panel);
	void PrintLine(ExcelSchema& excelObject);
	void UpdateExcelDataFromPanel(ExcelSchema& excelObject, Panel& panel);
public:
	explicit ReportingReveals(std::list<Panel>& vecPanels);
	~ReportingReveals();
};

