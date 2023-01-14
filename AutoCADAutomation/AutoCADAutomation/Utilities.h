/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 *  * 
 * 22-Sep-2022 Satish D	- ABA-4 - Panel Strength
 */

#pragma once
#include <string>

typedef std::pair<double, double> COORDINATES;
typedef std::pair<COORDINATES, double> CIRCLE;
typedef std::pair< COORDINATES, COORDINATES> BOUNDS;

class Utilities
{
private:
	static Utilities* s_Utilities;
	Utilities(){}
public:
	static Utilities* getUtils();
	Utilities(Utilities& util) = delete;
	void operator=(const Utilities&) = delete;
	std::string getFraction(double value);
	bool boundCheck(BOUNDS& bound, COORDINATES& element); // utility function to check whether the element is within bound
	double getBoundsWidth(BOUNDS& bound);
	double getBoundsHeight(BOUNDS& bound);
	COORDINATES getMidPoint(BOUNDS& bound);
	std::string inchesToFeet(double value);		// convert from feet to inches fractional
	bool approximatelyEqual(double a, double b, double epsilon = 0.0001f);
	double DistanceBetweenPoints(BOUNDS& bound);
};

