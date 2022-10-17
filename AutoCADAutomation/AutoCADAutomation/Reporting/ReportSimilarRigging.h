/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 * 16-Oct-2022 Raghu - 2.2 Similar Rigging
 */

#pragma once
#include "ReportingBase.h"

class ReportSimilarRigging : public ReportingBase
{
private:
	ofstream csvfile;
	std::multimap < std::string, std::string > similarRiggingMap;
	void generateRiggingMap();
protected:
	void InitializeHeader();
	void PrintHeader();
	void PrintData(Panel& panel);
	void PrintLine();
public:
	void ReportData();
	explicit ReportSimilarRigging(std::list<Panel>& vecPanels);
	~ReportSimilarRigging();
};

