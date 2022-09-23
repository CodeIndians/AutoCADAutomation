/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 */

#pragma once
#include "ReportingBase.h"

class ReportingPanelData : public ReportingBase
{
private:
	ofstream csvfile;
	struct ExcelSchema
	{
		std::string cableLength;
		std::string braceId;
		std::string braceQty;
		std::string braceWallConn;
		std::string braceTO;
		std::string braceAngle;
		std::string bottConnFF;
		std::string braceDesignWind;
		std::string liftInsert;
		std::string FcReqdLift;
		std::string noRequired;
		std::string panelId;
		std::string dscJobNumber;
		std::string project;
		std::string date;
		std::string by;
		std::string chk;
		std::string rebarCoverExterior;
		std::string rebarCoverInterior;
		
		ExcelSchema() : 
			cableLength("NA"),
			braceId("NA"),
			braceQty("NA"),
			braceWallConn("NA"),
			braceTO("NA"),
			braceAngle("NA"),
			bottConnFF("NA"),
			braceDesignWind("NA"),
			liftInsert("NA"),
			FcReqdLift("NA"),
			noRequired("NA"),
			panelId("NA"),
			dscJobNumber("NA"),
			project("NA"),
			date("NA"),
			by("NA"),
			chk("NA"),
			rebarCoverExterior("NA"),
			rebarCoverInterior("NA")
			{
			}
	};
	ExcelSchema headerObject;
protected:
	void InitializeHeader();
	void PrintHeader();
	void PrintData(Panel& panel);
	void PrintLine(ExcelSchema& excelObject);
	void UpdateExcelDataFromPanel(ExcelSchema& excelObject, Panel& panel);
public:
	explicit ReportingPanelData(std::list<Panel>& vecPanels);
	~ReportingPanelData();
};

