/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 * 
 * 22-Sep-2022 Satish D - Panels having 7 Brace Inserts
 */

#pragma once
#include "ReportingBase.h"

class ReportingExcel :public ReportingBase
{
private:
	ofstream csvfile;
	struct ExcelSchema
	{
		std::string name;
		std::string panelWidth;
		std::string panelHeight;
		std::string thickness;
		std::string openings[6];
		std::string riggingType;
		std::string liftInserts[16];
		std::string braceInserts[7];
		std::string reveals[22];
		ExcelSchema() : name(""), panelWidth(""), panelHeight(",,"), thickness(""), riggingType("")
		{
			for (int i = 0; i < 6; i++)
				openings[i] = ",,,,";
			for (int i = 0; i < 16; i++)
				liftInserts[i] = ",";
			for (int i = 0; i < 7; i++)
				braceInserts[i] = ",";
			for (int i = 0; i < 22; i++)
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
	explicit ReportingExcel(std::list<Panel>& vecPanels);
	~ReportingExcel();
};

