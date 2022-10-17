/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 * 16-Oct-2022 Raghu - 2.2 Similar Rigging
 */
#include "ReportSimilarRigging.h"
#include <algorithm>

void ReportSimilarRigging::generateRiggingMap()
{
	for (auto& panel : mPanels)
	{
		if( panel.getUniqueRiggingType() != "NA")
			similarRiggingMap.insert(std::pair<std::string, std::string>(panel.getUniqueRiggingType(), panel.getPanelName()));
	}
}

void ReportSimilarRigging::InitializeHeader()
{
	// not implemented
}

void ReportSimilarRigging::PrintHeader()
{
	csvfile << "Similar Rigging" << std::endl;
}

void ReportSimilarRigging::PrintData(Panel& panel)
{
	// Not implemented
}

void ReportSimilarRigging::PrintLine()
{
	// using this to print all the data 
	std::multimap<std::string, std::string>::iterator it;
	std::string prevKey = similarRiggingMap.begin()->first;
	std::string resultStack;
	for (it = similarRiggingMap.begin(); it != similarRiggingMap.end(); ++it)
	{
		// this means that we have stacked up the similar rigging
		// output and refresh the stack again
		if (prevKey != it->first)
		{
			//remove the last delim
			resultStack = resultStack.substr(0, resultStack.length() - 1);

			//output the data into the file 
			auto numDelims = std::count_if(resultStack.begin(), resultStack.end(), [](char c) {return c == ','; });
			if( numDelims > 0)
				csvfile << resultStack << std::endl;
			// clear the result stack
			resultStack = it->second + delim;
		}
		else
		{
			resultStack += it->second + delim;
		}
		prevKey = it->first;
	}
	//remove the last delim
	resultStack = resultStack.substr(0, resultStack.length() - 1);

	auto numDelims = std::count_if(resultStack.begin(), resultStack.end(), [](char c) {return c == ','; });
	if (numDelims > 0)
		csvfile << resultStack << std::endl;
}

// Since the result is not a csv file, overriding the function from the base class
void ReportSimilarRigging::ReportData()
{
	PrintHeader();
	PrintLine();
}

ReportSimilarRigging::ReportSimilarRigging(std::list<Panel>& vecPanels)
{
	mPanels = vecPanels;
	auto location = fileLocation + drawingName + "_SimilarRigging_" + currentTime() + ".csv";
	csvfile.open(location, std::ios_base::app);
	InitializeHeader();
	generateRiggingMap();
}

ReportSimilarRigging::~ReportSimilarRigging()
{
	csvfile.close();
}