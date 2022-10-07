/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 * 
 * 8-Oct-2022 Raghu - 2.1 Panel Details
 */

#include "ReportingPanelData.h"

void ReportingPanelData::InitializeHeader()
{
	headerObject.cableLength = "Cable Length Main:";
	headerObject.braceId = "Brace ID:";
	headerObject.braceQty = "Brace QTY:";
	headerObject.braceWallConn = "Brace wall conn:";
	headerObject.braceTO = "Brace TO";
	headerObject.braceAngle = "Brace Angle";
	headerObject.bottConnFF = "Bott. Conn. DIST. FROM F.F.:";
	headerObject.braceDesignWind = "Brace Design Wind:";
	headerObject.liftInsert = "Lift Insert:";
	headerObject.FcReqdLift = "F’c ReQ’d At Lift:";
	headerObject.noRequired = "No. Req’D:";
	headerObject.panelId = "PANEL I.D.:";
	headerObject.dscJobNumber = "DSC JOB NO:";
	headerObject.project = "Project:";
	headerObject.date = "Date:";
	headerObject.by = "By:";
	headerObject.chk = "CHK:";
	headerObject.rebarCoverExterior = "Rebar Cover Exterior";
	headerObject.rebarCoverInterior = "Rebar Cover Interior";
}

void ReportingPanelData::PrintHeader()
{
	PrintLine(headerObject);
}

void ReportingPanelData::PrintData(Panel& panel)
{
	ExcelSchema dataObject;
	UpdateExcelDataFromPanel(dataObject, panel);
	PrintLine(dataObject);
}

void ReportingPanelData::PrintLine(ExcelSchema& excelObject)
{
	csvfile << excelObject.cableLength << delim;
	csvfile << excelObject.braceId << delim;
	csvfile << excelObject.braceQty << delim;
	csvfile << excelObject.braceWallConn << delim;
	csvfile << excelObject.braceTO << delim;
	csvfile << excelObject.braceAngle << delim;
	csvfile << excelObject.bottConnFF << delim;
	csvfile << excelObject.braceDesignWind << delim;
	csvfile << excelObject.liftInsert << delim;
	csvfile << excelObject.FcReqdLift << delim;
	csvfile << excelObject.noRequired << delim;
	csvfile << excelObject.panelId << delim;
	csvfile << excelObject.dscJobNumber << delim;
	csvfile << excelObject.project << delim;
	csvfile << excelObject.date << delim;
	csvfile << excelObject.by << delim;
	csvfile << excelObject.chk << delim;
	csvfile << excelObject.rebarCoverExterior << delim;
	csvfile << excelObject.rebarCoverInterior << delim;
	
	csvfile << std::endl;
}

void ReportingPanelData::UpdateExcelDataFromPanel(ExcelSchema& excelObject, Panel& panel)
{
	excelObject.cableLength = panel.getPanelParameter("CABLE LENGTH MAIN");
	excelObject.braceId = panel.getPanelParameter("BRACE ID");
	excelObject.braceQty = panel.getPanelParameter("BRACE QTY.");
	excelObject.braceWallConn = panel.getPanelParameter("BRACE WALL CONN.");
	excelObject.braceTO = panel.getPanelParameter("BRACE TO");
	excelObject.braceAngle = panel.getPanelParameter("BRACE ANGLE");
	excelObject.bottConnFF = panel.getPanelParameter("BOTT. CONN. DIST. FROM F.F.");
	excelObject.braceDesignWind = panel.getPanelParameter("BRACE DESIGN WIND");
	excelObject.liftInsert = panel.getPanelParameter("LIFT INSERT");
	excelObject.FcReqdLift = panel.getPanelParameter("f'c REQ'D AT LIFT");
	excelObject.noRequired = panel.getNumRequired();
	excelObject.panelId = panel.getPanelName();
	excelObject.dscJobNumber = mWhitePaper.getDscJobNo();
	excelObject.project = mWhitePaper.getProject();
	excelObject.date = mWhitePaper.getDate();
	excelObject.by = mWhitePaper.getBy();
	excelObject.chk = mWhitePaper.getChk();
	excelObject.rebarCoverExterior = panel.getRebarCoverExterior();
	excelObject.rebarCoverInterior = panel.getRebarCoverInterior();
}

ReportingPanelData::ReportingPanelData(std::list<Panel>& vecPanels, PanelWhitePaper& whitePaperPanel)
{
	mPanels = vecPanels;
	mWhitePaper = whitePaperPanel;
	auto location = fileLocation + drawingName + "_PanelDetails_" + currentTime() + ".csv";
	csvfile.open(location, std::ios_base::app);
	InitializeHeader();
}

ReportingPanelData::~ReportingPanelData()
{
	csvfile.close();
}
