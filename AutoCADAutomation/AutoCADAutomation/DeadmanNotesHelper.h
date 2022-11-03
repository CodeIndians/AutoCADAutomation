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

class DeadmanNotesHelper
{
public:
	explicit DeadmanNotesHelper(std::list<Panel>& vecPanels);
	~DeadmanNotesHelper();
	void PlaceLabels();

protected:
	void ProcessPanel();
private:
	std::list<Panel> m_PanelsList;
};

