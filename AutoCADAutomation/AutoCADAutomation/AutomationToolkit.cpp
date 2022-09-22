
/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 * 11-Sep-2022	SatishD		    - Added Skeleton implementations for new commands
 * 13-Sep-2022	Slanka		    - ABA-6: Round off lifting insert to 1”
 * 22-Sep-2022 Satish D	- ABA-4 - Panel Strength
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
			CollectionHelper::CollectLiftDimensionsAfterReconnect(pEnt);
			CollectionHelper::CollectPanelDimensions(pEnt);
			CollectionHelper::CollectAllTextLabels(pEnt);
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

/// <summary>
/// 
/// Method : PanelDataLabels
/// Author :
/// Purpose : This method is used to collect labels from Data area of a Panel
///
/// </summary>

void AutomationToolkit::PanelDataLabels()
{

	try
	{

	}
	catch (...)
	{
		acutPrintf(L"PanelDatalabels Command failed");
	}

}

/// <summary>
/// 
/// Method : IdentifySimilarRigging
/// Author :
/// Purpose : This method is used to identify similar rigging based on geometry
///
/// </summary>

void AutomationToolkit::IdentifySimilarRigging()
{

	try
	{

	}
	catch (...)
	{
		acutPrintf(L"IdentifySimilarRigging Command failed");
	}

}

/// <summary>
/// 
/// Method : PanelStrength
/// Author :
/// Purpose : This method is used to identify if the bottom leg of panel is less than 60% of the total width
///
/// </summary>

void AutomationToolkit::PanelStrength()
{

	try
	{
		CollectPanelInformation(false);

		// print data
		ReportingBase* report = new ReportingPanelStrength(vecPanels);
		report->ReportData();
		delete report;

	}
	catch (...)
	{
		acutPrintf(L"PanelStrength Command failed");
	}

}

/// <summary>
/// 
/// Method : LiftAndEgdeNotes
/// Author :
/// Purpose : This method is used to add notes with a leader lines
///
/// </summary>

void AutomationToolkit::LiftAndEgdeNotes()
{

	try
	{

	}
	catch (...)
	{
		acutPrintf(L"LiftAndEgdeNotes Command failed");
	}

}

/// <summary>
/// 
/// Method : LiftInsDimRoundOff
/// Author :
/// Purpose : This method is used to identify panels that have Lift Insert dimensions not being rounded off.
///
/// </summary>

void AutomationToolkit::LiftInsDimRoundOff()
{

	try
	{
		CollectPanelInformation(true);

		// print data
		ReportingBase* report = new ReportingUnroundedLiftInserts(vecPanels);
		report->ReportData();
		delete report;
	}
	catch (...)
	{
		acutPrintf(L"LiftInsDimRoundOff Command failed");
	}

}

/// <summary>
/// 
/// Method : LiftInsClearance
/// Author :
/// Purpose : This method is used to detect if the distance between teh lift insert and opening as per the user entered valuers
///
/// </summary>

void AutomationToolkit::LiftInsClearance()
{

	try
	{

	}
	catch (...)
	{
		acutPrintf(L"LiftInsClearance Command failed");
	}

}

/// <summary>
/// 
/// Method : BraceInsClearance
/// Author :
/// Purpose : This method is used to detect if the distance between teh brace insert and opening as per the user entered valuers
///
/// </summary>

void AutomationToolkit::BraceInsClearance()
{

	try
	{

	}
	catch (...)
	{
		acutPrintf(L"BraceInsClearance Command failed");
	}

}

/// <summary>
/// 
/// Method : RebarLabels
/// Author :
/// Purpose : This method is used to validate Rebar label and place appropriate deadman notes 
/// 
/// </summary>

void AutomationToolkit::RebarLabelsDeadmanNotes()
{

	try
	{

	}
	catch (...)
	{
		acutPrintf(L"RebarLabels Command failed");
	}

}


/// <summary>
/// 
/// Method : RevealPostions
/// Author :
/// Purpose : This method is used to enter the reveal position information in the position infoirmation csv
/// 
/// </summary>

void AutomationToolkit::RevealPostions()
{

	try
	{

	}
	catch (...)
	{
		acutPrintf(L"RevealPostions Command failed");
	}

}

/// <summary>
/// 
/// Method : InsertsDistFromReveals
/// Author :
/// Purpose : This method is used to validate the distance between revels and lift and brace inserts is greater than 9"
/// 
/// </summary>

void AutomationToolkit::InsertsDistFromReveals()
{

	try
	{

	}
	catch (...)
	{
		acutPrintf(L"InsertsDistFromReveals Command failed");
	}

}