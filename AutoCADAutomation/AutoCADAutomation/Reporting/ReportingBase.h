/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 */

#pragma once
#include "../Panel.h"
#include <fstream>
#include <filesystem>

using std::ofstream;
namespace fs = std::filesystem;

class ReportingBase
{
protected:
	const std::string fileLocation = "C:\\AutoCADReports\\";
	const std::string delim = ",";
	std::list<Panel> mPanels;
	std::string drawingName;
	std::string currentTime()
	{
		std::time_t now = std::time(0);
		std::tm* now_tm = std::localtime(&now);
		char buf[42];
		std::strftime(buf, 42, "%d%m%Y%H%M%S", now_tm);
		return buf;
	}
	virtual void InitializeHeader() = 0; // header object initializer
	virtual void PrintHeader() = 0;
	virtual void PrintData(Panel &panel) = 0;
public :
	virtual void ReportData() 
	{
		PrintHeader();
		for (auto& panel : mPanels)
		{
			PrintData(panel);
		}
	}
	ReportingBase()
	{
		if (!fs::is_directory(fileLocation) || !fs::exists(fileLocation)) { // Check if AutoCADReports folder exists
			fs::create_directory(fileLocation); // create AutoCADReports folder
		}
		std::wstring name = curDoc()->fileName();
		std::string str(name.begin(), name.end());
		str = str.substr(str.find_last_of("\\") + 1);
		drawingName = str.substr(0, str.find_last_of("."));
	}
	virtual ~ReportingBase() { }
};

