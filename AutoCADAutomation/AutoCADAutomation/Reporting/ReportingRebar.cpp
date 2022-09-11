#include "ReportingRebar.h"

void ReportingRebar::InitializeHeader()
{
	headerObject.name = "Panel Name";
	headerObject.status = "Rebar Status";
}

void ReportingRebar::PrintHeader()
{
	PrintLine(headerObject);
}

void ReportingRebar::PrintData(Panel& panel)
{
	ExcelSchema dataObject;
	UpdateExcelDataFromPanel(dataObject, panel);
	PrintLine(dataObject);
}

void ReportingRebar::PrintLine(ExcelSchema& excelObject)
{
	csvfile << excelObject.name << delim;
	csvfile << excelObject.status << delim;
	csvfile << std::endl;
}

void ReportingRebar::UpdateExcelDataFromPanel(ExcelSchema& excelObject, Panel& panel)
{
	excelObject.name = panel.getPanelName();
	excelObject.status = panel.areRebarTextsMatching() ? "Good" : "Bad";
}

ReportingRebar::ReportingRebar(std::list<Panel>& vecPanels)
{
	mPanels = vecPanels;
	auto location = fileLocation + drawingName + "_Rebars_" + currentTime() + ".csv";
	csvfile.open(location, std::ios_base::app);
	InitializeHeader();
}

ReportingRebar::~ReportingRebar()
{
	csvfile.close();
}