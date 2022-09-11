/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 */

#pragma once
extern "C" char* InputBox(char* Prompt, char* Title = (char*)"", char* Default = (char*)"");
extern "C" char* PasswordBox(char* Prompt, char* Title = (char*)"", char* Default = (char*)"");
extern "C" bool ValidateSerialkey(char* SerialKey);
extern "C" bool ValidateLicense();