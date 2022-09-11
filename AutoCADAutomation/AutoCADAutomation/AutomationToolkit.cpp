
/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 */

#include "AutomationToolkit.h"


std::list<Panel> AutomationToolkit::vecPanels;

void AutomationToolkit::AbsolutePositions()
{
	// Collect Panel Data
	CollectPanelInformation();

	// print data
	ReportingBase* report = new ReportingExcel(vecPanels);
	report->ReportData();
	delete report;
	
	// Display the AutoCAD Text Window
	acedTextScr();

}

void AutomationToolkit::InterferenceCheck()
{
	try
	{
		// Collect Panel Data
		CollectPanelInformation();
		
		// print data
		ReportingBase* report = new ReportingInterference(vecPanels);
		report->ReportData();
		delete report;

		acutPrintf(L"InterferenceCheck completed Successfully");
	}
	catch (...)
	{
		acutPrintf(L"InterferenceCheck failed");
	}
}

void AutomationToolkit::RebarLabelsCheck()
{
	try
	{
		CollectPanelInformation();

		// print data
		ReportingBase* report = new ReportingRebar(vecPanels);
		report->ReportData();
		delete report;

	}
	catch (...)
	{
		acutPrintf(L"Rebar validation failed");
	}
}

void AutomationToolkit::DimensionsReconnect()
{
	
	try
	{
		CollectPanelInformation(true);

	}
	catch (...)
	{
		acutPrintf(L"Dimensions reconnect failed");
	}

}

void AutomationToolkit::CollectPanelInformation(bool dimensions)
{
	vecPanels.clear();

	// Get the current database
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();

	// Get the current space object
	AcDbBlockTableRecord* pBlockTableRecord;
	Acad::ErrorStatus es = acdbOpenObject(pBlockTableRecord, pDb->currentSpaceId(), AcDb::kForRead);

	// Create a new block iterator that will be used to step through each object
	AcDbBlockTableRecordIterator* pItr;

	pBlockTableRecord->newIterator(pItr);

	// Create a variable AcDbEntity type which is a generic
	// object to represent a Line, Circle, Arc, among other objects
	AcDbEntity* pEnt;

	// Step through each object in the current space
	for (pItr->start(); !pItr->done(); pItr->step())
	{
		// Get the entity and open it for read
		pItr->getEntity(pEnt, AcDb::kForWrite);

		if (pEnt == nullptr)
			continue;

		CollectionHelper::CollectDefPoints(pEnt, vecPanels);

		if (!dimensions)
		{
			CollectionHelper::CollectLiftInserts(pEnt);
			CollectionHelper::CollectBraceInserts(pEnt);
			CollectionHelper::CollectOpenings(pEnt);
			CollectionHelper::CollectInternalPanels(pEnt);
			CollectionHelper::CollectAllTextLabels(pEnt);
		}

		if (dimensions)
		{
			CollectionHelper::CollectLiftInserts(pEnt);
			CollectionHelper::CollectBraceInserts(pEnt);
			// NOTE: Collecting dimensions will remove them. This cannot be done again
			//Collect Dimensions
			CollectionHelper::CollectInternalPanels(pEnt);
			CollectionHelper::CollectOpeningDimensions(pEnt);
			CollectionHelper::CollectBraceDimensions(pEnt);
			CollectionHelper::CollectLiftDimensions(pEnt);
			CollectionHelper::CollectPanelDimensions(pEnt);
		}
		pEnt->close();
	}
	// Close the current space object
	pBlockTableRecord->close();

	// Remove the block iterator object from memory
	delete pItr;

	CollectionHelper::PopulatePanelData(vecPanels);

	//clear collection data when the panels are filled in the current diagram
	CollectionHelper::ClearCollectionData(); 
}
