
/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 *   
 * 11-Sep-2022	SatishD		- Added Skeleton implementations for new commands
 */


// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
// Load the common AutoCAD headers
#include "arxHeaders.h"
#include "dbents.h"
#include "AutomationToolkit.h"
//#include "InputBox.h"

#include <string>
#include <chrono>
#include <fstream>
#include <tchar.h>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"
#include <comdef.h>

using namespace std;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

static bool bValidLicense = false;
extern "C" AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt)
{
	
	TCHAR szPath[MAX_PATH];
	
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
	{
		PathAppend(szPath, _T("\\Autodesk\\AutoCAD 2021\\R24.0\\enu\\Support\\Autodesk.txt"));
	}
	std::ifstream myfile(szPath);
	std::ofstream outFile;
	string line;
	int base = 10;
	char* end;
	_bstr_t b(szPath);
	long long  millisec_since_epoch;
	long long  millisec_since_epoch1;
	long long savedTime;

	string strCmd = "getmac";
	string strFilename = "macaddress.txt";
	string strMacID;

	system((strCmd + ">" + strFilename).c_str());
	string line1;

	std::ifstream myfile1("macaddress.txt");

	int i = 0;
	if (myfile1.is_open()) {
		while (getline(myfile1, line1)) {
			if (i == 3)
			{
				strMacID = line1.substr(0, 17);
				break;
			}
			i++;
		}
		myfile1.close();
	}
	DeleteFileA("macaddress.txt");
	
	switch (msg)
	{
	case AcRx::kInitAppMsg:

		if (myfile.is_open()) {
			getline(myfile, line);
			myfile.close();
		}

		savedTime = strtoll(line.c_str(), &end, base);

		/*if (strMacID != "54-14-F3-F3-D4-04")
			bValidLicense = false;
		else*/
			bValidLicense = true;

		if (!bValidLicense)
		{
			acutPrintf(L"\nFailed to Load Automation Toolkit...\n");
			return AcRx::kRetError;
		}
		acrxDynamicLinker->unlockApplication(pkt);
		acrxDynamicLinker->registerAppMDIAware(pkt);
		acutPrintf(L"\nLoading Automation Toolkit ....\n");
		// Commands to add
		acedRegCmds->addCommand(L"AutomationToolkit", L"AbsolutePos", L"AbsPos", ACRX_CMD_MODAL, AutomationToolkit::AbsolutePositions);
		acedRegCmds->addCommand(L"AutomationToolkit", L"InterferenceCheck", L"CheckInterference", ACRX_CMD_MODAL, AutomationToolkit::InterferenceCheck);
		acedRegCmds->addCommand(L"AutomationToolkit", L"RebarLabelsCheck", L"CheckRebarLabels", ACRX_CMD_MODAL, AutomationToolkit::RebarLabelsCheck);
		acedRegCmds->addCommand(L"AutomationToolkit", L"DimensionsReconnect", L"ReconnectDimensions", ACRX_CMD_MODAL, AutomationToolkit::DimensionsReconnect);
		
		/*2.1*/acedRegCmds->addCommand(L"AutomationToolkit", L"PanelDataLabels", L"PanelDataLabelsLocal", ACRX_CMD_MODAL, AutomationToolkit::PanelDataLabels);
		/*2.2*/acedRegCmds->addCommand(L"AutomationToolkit", L"IdentifySimilarRigging", L"IdentifySimilarRiggingLocal", ACRX_CMD_MODAL, AutomationToolkit::IdentifySimilarRigging);
		/*2.3*/acedRegCmds->addCommand(L"AutomationToolkit", L"PanelStrength", L"PanelStrengthLocal", ACRX_CMD_MODAL, AutomationToolkit::PanelStrength);
		/*2.4*/acedRegCmds->addCommand(L"AutomationToolkit", L"LiftAndEgdeNotes", L"LiftAndEgdeNotesLocal", ACRX_CMD_MODAL, AutomationToolkit::LiftAndEgdeNotes);
		/*2.5*/acedRegCmds->addCommand(L"AutomationToolkit", L"LiftInsDimRoundOff", L"LiftInsDimRoundOffLocal", ACRX_CMD_MODAL, AutomationToolkit::LiftInsDimRoundOff);
		/*2.6*/acedRegCmds->addCommand(L"AutomationToolkit", L"LiftInsClearance", L"LiftInsClearanceLocal", ACRX_CMD_MODAL, AutomationToolkit::LiftInsClearance);
		/*2.7*/acedRegCmds->addCommand(L"AutomationToolkit", L"BraceInsClearance", L"BraceInsClearance", ACRX_CMD_MODAL, AutomationToolkit::BraceInsClearance);
		/*2.8*/acedRegCmds->addCommand(L"AutomationToolkit", L"RebarLabelsDeadmanNotes", L"RebarLabelsLocal", ACRX_CMD_MODAL, AutomationToolkit::RebarLabelsDeadmanNotes);
		/*2.9*/acedRegCmds->addCommand(L"AutomationToolkit", L"RevealPostions", L"RevealPostionsLocal", ACRX_CMD_MODAL, AutomationToolkit::RevealPostions);
		/*2.10*/acedRegCmds->addCommand(L"AutomationToolkit", L"InsertsDistFromReveals", L"InsertsDistFromRevealsLocal", ACRX_CMD_MODAL, AutomationToolkit::InsertsDistFromReveals);

		break;

	case AcRx::kUnloadAppMsg:
		millisec_since_epoch1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		if (bValidLicense)
		{
			outFile.open(szPath, ios::ios_base::trunc);
			if (outFile.is_open())
			{
				outFile << millisec_since_epoch1;
			}
		}
		outFile.close();
		acutPrintf(L"\nUnloading Automation Toolkit ....\n");
		acedRegCmds->removeGroup(L"AutomationToolkit");
		break;

	case AcRx::kLoadDwgMsg:
		acutPrintf(L"\nSwitching Drawing\n");
		break;
	default:
		break;
	}

	return AcRx::kRetOK;
}


