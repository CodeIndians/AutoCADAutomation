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

#include "Utilities.h"

Utilities* Utilities::s_Utilities = nullptr;

Utilities* Utilities::getUtils()
{
	if (s_Utilities == nullptr)
		s_Utilities = new Utilities();
	return s_Utilities;
}

std::string Utilities::getFraction(double value)
{
	if (value > 0.0 && value < 1.0)
	{
		if (value <= 1.0 / 16)
			return " ";
		if (value <= 2.0 / 16)
			return " 1/8";
		if (value <= 3.0 / 16)
			return " 1/8";
		if (value <= 4.0 / 16)
			return " 1/4";
		if (value <= 5.0 / 16)
			return " 1/4";
		if (value <= 6.0 / 16)
			return " 3/8";
		if (value <= 7.0 / 16)
			return " 3/8";
		if (value <= 8.0 / 16)
			return " 1/2";
		if (value <= 9.0 / 16)
			return " 1/2";
		if (value <= 10.0 / 16)
			return " 5/8";
		if (value <= 11.0 / 16)
			return " 5/8";
		if (value <= 12.0 / 16)
			return " 3/4";
		if (value <= 13.0 / 16)
			return " 3/4";
		if (value <= 14.0 / 16)
			return " 7/8";
		if (value <= 15.0 / 16)
			return " 7/8";
	}
	return "";
}

bool Utilities::boundCheck(BOUNDS& bound, COORDINATES& element)
{
	auto elementX = element.first;
	auto elementY = element.second;
	if (bound.first.first <= elementX && bound.second.first >= elementX)   // x coordinate check
		if (bound.first.second <= elementY && bound.second.second >= elementY) // y coordinate check
			return true;
	return false;
}

double Utilities::getBoundsWidth(BOUNDS& bound)
{
	return std::abs(bound.first.first - bound.second.first);
}

double Utilities::getBoundsHeight(BOUNDS& bound)
{
	return std::abs(bound.first.second - bound.second.second);
}

COORDINATES Utilities::getMidPoint(BOUNDS& bound)
{
	COORDINATES midPoint;
	midPoint.first = (bound.first.first + bound.second.first) / 2;
	midPoint.second = (bound.first.second + bound.second.second) / 2;
	return midPoint;
}

std::string Utilities::inchesToFeet(double value)
{
	int integerPart = (int)value;
	float decimalPart = value - (float)integerPart;
	int feet = integerPart / 12;
	int inches = integerPart % 12;
	std::string fraction = Utilities::getUtils()->getFraction(decimalPart);
	return std::to_string(feet) + "' " + std::to_string(inches) + fraction + "\"";
}

bool Utilities::approximatelyEqual(float a, float b, float epsilon )
{
	return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

double Utilities::DistanceBetweenPoints(BOUNDS& bound)
{
	return sqrt(pow((bound.first.first - bound.second.first), 2) + pow((bound.first.second - bound.second.second), 2));

}