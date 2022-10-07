
/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */
 /* -----------------------Revision History------------------------------------------
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 * 11-Sep-2022	SatishD		- Added Skeleton implementations for new commands
 * 13-Sep-2022	Slanka		    - ABA-6: Round off lifting insert to 1”
 * 22-Sep-2022 Satish D			- ABA-4 - Panel Strength
 * 23-Sep-2022 Satish D			- ABA-7/8 - Lift Insert and brace Insert Clearance
 * 8-Oct-2022 Raghu - 2.1 Panel Details
 */

#include "AutomationToolkit.h"

std::list<Panel> AutomationToolkit::vecPanels;
AcDbVoidPtrArray AutomationToolkit::entSet;
AcDbVoidPtrArray AutomationToolkit::entSetSecondLevel;


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

void AutomationToolkit::CollectPanelDetailsInformationFromWhitePaper(PanelWhitePaper& whitePaper)
{
	// Get the current database
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();

	//Get the layout manager
	AcApLayoutManager* pLayoutMngr = dynamic_cast<AcApLayoutManager*>( acdbHostApplicationServices()->layoutManager() );

	AcString oldLayoutName;

	//save the active layout for switching it to later
	pLayoutMngr->getActiveLayoutName(oldLayoutName,true);

	//switch to white paper layout
	pLayoutMngr->setCurrentLayout(L"1");

	// Get the current space object
	AcDbBlockTableRecord* pBlockTableRecord;
	Acad::ErrorStatus es = acdbOpenObject(pBlockTableRecord, pDb->currentSpaceId(), AcDb::kForWrite);
	auto spaceId = pDb->currentSpaceId();

	 //Create a new block iterator that will be used to step through each object
	 AcDbBlockTableRecordIterator* pItr;

	 pBlockTableRecord->newIterator(pItr);

	// Create a variable AcDbEntity type which is a generic
	// object to represent a Line, Circle, Arc, among other objects
	AcDbEntity* pEnt;
	
	// Step through each object in the current space
	for (pItr->start(); !pItr->done(); pItr->step())
	{
		AcDbObjectId objId;
		// Get the entity and open it for read
		pItr->getEntity(pEnt, AcDb::kForWrite);
		pItr->getEntityId(objId);

		if (pEnt == nullptr)
			continue;
		
		// TODO: Collection logic needs to be placed inside collection helpers
		if ((wcscmp(pEnt->layer(), L"0") == 0) && (wcscmp(pEnt->isA()->name(), L"AcDbBlockReference") == 0))
		{
			AcDbBlockReference* blockref = (AcDbBlockReference*)pEnt;
			blockref->explode(entSet);
			for (auto& ent : entSet)
			{
				AcDbEntity* internalEntity = (AcDbEntity*)ent;
				if (wcscmp(internalEntity->isA()->name(), L"AcDbText") == 0)
				{
					auto textEntity = (AcDbText*)internalEntity;
					ACHAR* text = textEntity->textString();
					std::wstring ws(text);
					std::string str(ws.begin(), ws.end());
					LABELTEXT labelText;
					labelText.first = str;
					labelText.second.first = textEntity->position().x;
					labelText.second.second  = textEntity->position().y;
					whitePaper.addWhitePanelTexts(labelText);
				}
				if (wcscmp(internalEntity->isA()->name(), L"AcDbMText") == 0)
				{
					auto textEntity = (AcDbMText*)internalEntity;
					ACHAR* text = textEntity->text();
					std::wstring ws(text);
					std::string str(ws.begin(), ws.end());
					LABELTEXT labelText;
					labelText.first = str;
					labelText.second.first = textEntity->location().x;
					labelText.second.second = textEntity->location().y;
					whitePaper.addWhitePanelTexts(labelText);
				}
				// second level explode to get info inside nested block references up to level 2.
				if (wcscmp(internalEntity->isA()->name(), L"AcDbBlockReference") == 0)
				{
					AcDbBlockReference* blockrefsecondlevel = (AcDbBlockReference*)internalEntity;
					blockrefsecondlevel->explode(entSetSecondLevel);
					auto size = entSetSecondLevel.length();
					for (auto& ent : entSetSecondLevel)
					{
						AcDbEntity* internalEntitySecondLevel = (AcDbEntity*)ent;
						if (wcscmp(internalEntitySecondLevel->isA()->name(), L"AcDbMText") == 0)
						{
							auto textEntity = (AcDbMText*)internalEntitySecondLevel;
							ACHAR* text = textEntity->text();
							auto visibility = textEntity->visibility();
							std::wstring ws(text);
							std::string str(ws.begin(), ws.end());
							LABELTEXT labelText;
							labelText.first = str;
							labelText.second.first = textEntity->location().x;
							labelText.second.second = textEntity->location().y;
							if(visibility == kVisible)
								whitePaper.addWhitePanelTextsSecondLevel(labelText);
						}
						internalEntitySecondLevel->close();
						delete internalEntitySecondLevel;
					}
					entSetSecondLevel.removeAll();
					entSetSecondLevel.setLogicalLength(0);
				}
				internalEntity->close();
				delete internalEntity;
			}
			entSet.removeAll();
			entSet.setLogicalLength(0);
		}
		pEnt->close();
	}

	pLayoutMngr->setCurrentLayout(oldLayoutName);

	//Close the current space object
	pBlockTableRecord->close();
	
	// Remove the block iterator object from memory
	delete pItr;
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
		PanelWhitePaper whitePaper;
		CollectPanelInformation();
		CollectPanelDetailsInformationFromWhitePaper(whitePaper);
		whitePaper.updateWhitePaperData();
		// print data
		ReportingBase* report = new ReportingPanelData(vecPanels, whitePaper);
		report->ReportData();
		delete report;

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
		CollectPanelInformation(false);
		char title[] = "Enter distances in inches for Lift Insert Clearance validation. Default Values are \n \n Lift Insert and Panel = 24 Inches \n Lift Insert and Openings = 15 inches";
		char propmt[] = "Lift Insert Clearance";
		char defalut[] = "24,15";
		char* result = InputBox(title, propmt, defalut);

		std::string str(result);
		
		auto pos = str.find(",");
		int j = atoi(str.substr(0, pos).c_str());
		int i = atoi(str.substr(pos + 1 , str.size() - 1).c_str());

		// print data
		ReportingBase* report = new ReportingInsertClearance(vecPanels, "LIFT_INSERTS", j, i);
		report->ReportData();
		delete report;

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
/// Purpose : This method is used to detect if the distance between teh brace insert and opening and panel boundary as per the user entered valuers
///
/// </summary>

void AutomationToolkit::BraceInsClearance()
{

	try
	{
		CollectPanelInformation(false);

		char title[] = "Enter distances in inches for Brace Insert Clearance validation. \n \n  Default Values are \n Brace Insert and Panel = 24 Inches \n Brace Insert and Openings = 15 inches";
		char propmt[] = "Brace Insert Clearance";
		char defalut[] = "24,15";
		char* result = InputBox(title, propmt, defalut);

		std::string str(result);

		auto pos = str.find(",");
		int j = atoi(str.substr(0, pos).c_str());
		int i = atoi(str.substr(pos + 1, str.size() - 1).c_str());

		// print data
		ReportingBase* report = new ReportingInsertClearance(vecPanels, "BRACE_INSERTS" , j , i);
		report->ReportData();
		delete report;

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