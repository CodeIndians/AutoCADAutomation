/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 8-Oct-2022 Satish - Licensing
 */

#pragma once
#include<map>
#include<string>
#include <fstream>
#include <tchar.h>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"
#include <comdef.h>

class LicenseManager
{
public:
	LicenseManager();
	static bool isValidLicense();
	static std::string DecryptMacID(std::string strKey);
private:
};