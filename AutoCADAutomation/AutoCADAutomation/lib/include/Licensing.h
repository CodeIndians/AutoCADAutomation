#pragma once

#include<map>
#include<string>

using namespace std;

namespace Licensingns
{
	class Licensing
	{
	public:
		static bool validateLicense(string& macID, string& LicenseKey);
		static void Initmap();
		static std::map<std::string, string>  opMap_;
	};
}