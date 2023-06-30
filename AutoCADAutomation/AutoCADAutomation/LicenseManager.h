/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 8-Oct-2022 Satish - Licensing
 */

#pragma once
#include <string>


class LicenseManager
{
public:
    // Define the Request model
    struct RequestModel
    {
        std::string HostName;
        std::string IPAddress;
        std::string ProductName;
    };

    // Define the Response model
    struct ResponseModel
    {
        bool ValidLicense;
    };
	LicenseManager();
	static bool isValidLicense();
    static std::string GetLocalIPAddress();
private:
};