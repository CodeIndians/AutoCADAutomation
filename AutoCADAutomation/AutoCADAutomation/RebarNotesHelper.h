/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 * 22-Sep-2022 Satish D	- ABA-4 - Panel Strength
 * 8-Oct-2022 Raghu - 2.1 Panel Details
 * 16-Oct-2022 Raghu - 2.2 Similar Rigging
 * 25-Oct-2022 SatishD - 2.9 Reveal positions
 */

#pragma once
#include "Panel.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using std::ofstream;

class RebarNotesHelper
{
public:
	explicit RebarNotesHelper(std::list<Panel>& vecPanels);
	~RebarNotesHelper();
	void PlaceLabels();
	void CreateCSVFile();
	bool ParseCSVFile();
	void ComputeLabelConnectPoints(Panel& panel, std::vector<AcGePoint3d>& connectPoints, std::vector<AcGePoint3d>& internalConnectPoints);
	void PlaceBoxAndLines(int iLegNo, std::vector<AcGePoint3d>& connectPoints, std::vector<AcGePoint3d>& internalConnectPoints, AcDbBlockTableRecordPointer& pBTR);
	void ProcessRebars(std:: vector<std::string> labels, std::vector<std::string> contents, int iLegNo, Panel& panel, std::vector<AcGePoint3d>& connectPoints, std::vector<AcGePoint3d>& internalConnectPoints);

private:
	struct RebarLeg
	{
		bool bHasTop;
		bool bHasBottom;
		int topReinforcement;
		int bottomReinforcement;
		int userValue;
	};
	ofstream m_csvfile;
	std::string m_location;
	std::list<Panel> m_PanelsList;
	std::vector<std::vector<std::string>> m_VecContent;
};

