

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


