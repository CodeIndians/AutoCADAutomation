/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 8-Oct-2022 Raghu - 2.1 Panel Details
 */

#include "PanelWhitePaper.h"

void PanelWhitePaper::updateDate()
{
	
	for (auto& text : mWhitePanelTexts)
	{
		if (text.first.size() == 8 && text.first[2] == '-' && text.first[5] == '-')
		{
			mDate = text.first;
			break;
		}
	}
}

void PanelWhitePaper::updateDscJobNo()
{
	std::string validateString = "DSC JOB NO.: ";
	for (auto& detailLabel : mWhitePanelTexts)
	{
		auto pos = detailLabel.first.find(validateString) + validateString.length();
		if (detailLabel.first.find(validateString) != std::string::npos)
		{
			mDscJobNo = detailLabel.first.substr(pos);
		}
	}
}

void PanelWhitePaper::updateProject()
{
	std::string validateString = "PROJECT: ";
	for (auto& detailLabel : mWhitePanelTexts)
	{
		auto pos = detailLabel.first.find(validateString) + validateString.length();
		if (detailLabel.first.find(validateString) != std::string::npos)
		{
			mProject = detailLabel.first.substr(pos);
		}
	}
}

void PanelWhitePaper::updateBy()
{
	//BY: ("ITS", (7.3893048380611406, 0.81337389360216483))
	for (auto& text : mWhitePanelTexts)
	{
		if (text.second.first > 7.3 && text.second.first < 7.45)
		{
			if (text.second.second > 0.79 && text.second.second < 0.9)
			{
				mBy = text.first;
			}
		}
	}
}

void PanelWhitePaper::updateChk()
{
	//CHK: ("TTG", (7.3049779836624058, 0.87587389360216483))
	for (auto& text : mWhitePanelTextsSecondLevel)
	{
		if (text.second.first > 7.3 && text.second.first < 7.4)
		{
			if (text.second.second > 0.8 && text.second.second < 0.9)
			{
				mChk = text.first;
			}
		}
	}
}

PanelWhitePaper::PanelWhitePaper() : mDate(""), mProject(""), mDscJobNo(""), mBy(""), mChk("")
{

}

PanelWhitePaper::~PanelWhitePaper()
{
	mWhitePanelTexts.clear();
	mWhitePanelTextsSecondLevel.clear();
}

void PanelWhitePaper::addWhitePanelTexts(LABELTEXT& text)
{
	if (std::find(mWhitePanelTexts.begin(), mWhitePanelTexts.end(), text) == mWhitePanelTexts.end())
	{
		mWhitePanelTexts.push_back(text);
	}
}

void PanelWhitePaper::addWhitePanelTextsSecondLevel(LABELTEXT& text)
{
	if (std::find(mWhitePanelTextsSecondLevel.begin(), mWhitePanelTextsSecondLevel.end(), text) == mWhitePanelTextsSecondLevel.end())
	{
		mWhitePanelTextsSecondLevel.push_back(text);
	}
}

void PanelWhitePaper::updateWhitePaperData()
{
	updateDate();
	updateDscJobNo();
	updateProject();
	updateBy();
	updateChk();
}


