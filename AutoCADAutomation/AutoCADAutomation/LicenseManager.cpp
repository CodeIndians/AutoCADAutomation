
/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 8-Oct-2022 Satish - Licensing
 */

#include "LicenseManager.h"
#include <sstream>

using namespace std;
LicenseManager::LicenseManager()
{
}

bool LicenseManager::isValidLicense()
{

	// Get the allowed time from the license file
	TCHAR szPath[MAX_PATH];

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
	{
		PathAppend(szPath, _T("\\Autodesk\\AutoCAD 2021\\R24.0\\enu\\Support\\License.ini"));
	}
	std::ifstream myfile(szPath);
	std::ofstream outFile;
	string line;
	int base = 10;
	char* end;
	_bstr_t b(szPath);
	long long  millisec_since_epoch;
	long long savedTime;

	if (myfile.is_open()) {
		getline(myfile, line);
		myfile.close();
	}
	else
	{
		return false;
	}

	string strTime = line.substr(0, line.find("-"));
	savedTime = strtoll(strTime.c_str(), &end, base);
	
	// Get the current time
	time_t seconds_past_epoch = time(0); // current time
	std::stringstream sstr;
	sstr << seconds_past_epoch;
	long long currentTime = strtoll(sstr.str().c_str(), &end, base);

	// Get Saved Mac addresss
	int nPos = line.find("-");
	string strEncryptedmac = line.substr(nPos + 1, (line.length() -  nPos));
	string strDecryptepdMac = DecryptMacID(strEncryptedmac);

	// Get actual mac address
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

	if ((strcmp(strMacID.c_str(), strDecryptepdMac.c_str()) == 0) && (savedTime > currentTime))
	{
		return true;
	}
	else
	{
		return false;
	}

}

std::string LicenseManager::DecryptMacID(std::string strKey)
{
	// License is of the form xxxxxxxx-xxxxxxxx-xxxxxxxx-xxxxxxxx-xxxxxxxx
	// iterate 4 characters each and then

	std::map<std::string, std::string> licenseMap;
	licenseMap["qwe1"] = "0";
	licenseMap["-"] = "-";
	licenseMap["wer2"] = "1";
	licenseMap["ert3"] = "2";
	licenseMap["rty4"] = "3";
	licenseMap["tyu5"] = "4";
	licenseMap["yui6"] = "5";
	licenseMap["uio7"] = "6";
	licenseMap["iop8"] = "7";
	licenseMap["opa9"] = "8";
	licenseMap["pas0"] = "9";
	licenseMap["1QAZ"] = "A";
	licenseMap["2WSX"] = "B";
	licenseMap["3EDC"] = "C";
	licenseMap["4RFV"] = "D";
	licenseMap["5TGB"] = "E";
	licenseMap["6YHN"] = "F";

	string retString;
	int j = 1;
	for (int i = 0; i < strKey.length(); i += 4)
	{
		if (j == 3)
		{
			retString += "-";
			j = 1;
			i -= 3;
			continue;
		}
		retString += licenseMap[ strKey.substr(i, 4)];
		j++;
	}
	return retString;
}
