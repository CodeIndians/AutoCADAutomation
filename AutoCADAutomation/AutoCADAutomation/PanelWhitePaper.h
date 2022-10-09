/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 8-Oct-2022 Raghu - 2.1 Panel Details
 */

#pragma once
#include <string>
#include <vector>

typedef std::pair<double, double> COORDINATES;
typedef std::pair<std::string, COORDINATES> LABELTEXT;

class PanelWhitePaper
{
private:
	std::vector<LABELTEXT> mWhitePanelTexts;
	std::vector<LABELTEXT> mWhitePanelTextsSecondLevel;
	std::string mDate;
	std::string mDscJobNo;
	std::string mProject;
	std::string mBy;
	std::string mChk;
	void updateDate();
	void updateDscJobNo();
	void updateProject();
	void updateBy();
	void updateChk();
public:
	explicit PanelWhitePaper();
	~PanelWhitePaper();
	void addWhitePanelTexts(LABELTEXT& text);
	void addWhitePanelTextsSecondLevel(LABELTEXT& text);
	void updateWhitePaperData();

	// white paper data getter functions 
	std::string getDate() { return mDate; }
	std::string getDscJobNo() { return mDscJobNo; }
	std::string getProject() { return mProject; }
	std::string getBy() { return mBy; }
	std::string getChk() { return mChk; }

};

