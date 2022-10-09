/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 */

#pragma once
#include "CollectionHelper.h"

class PanelBuilder
{
private:
	Panel* ptrPanel;
public:
	PanelBuilder(Panel &panel);
	void buildLiftInserts(std::list<CIRCLE>& liftInserts);
	void buildBraceInserts(std::list<COORDINATES>& braceInserts);
	void buildOpenings(std::list<BOUNDS>& openings, std::list<COORDINATES> futureTextCordinates);
	void buildLabels(std::list<LABELTEXT>& textLabels);
	void buildDeadManLabels(std::list<BOUNDS>& deadManLabels);
	void buildInternalPanel(std::list<BOUNDS>& internalPanels, std::list<COORDINATES> &internalPanelLines);
	void updatePanel();

	//dimensions
	void buildOpeningDimensions(std::list<BOUNDS> horizantalBounds, std::list<BOUNDS> verticalBounds);
	void buildBraceDimensions(std::list<BOUNDS> horizantalBounds, std::list<BOUNDS> verticalBounds);
	void buildLiftDimensions(std::list<BOUNDS> horizantalBounds, std::list<BOUNDS> verticalBounds);
	void buildPanelDimensions(std::list<BOUNDS> horizantalBounds, std::list<BOUNDS> verticalBounds);

	void buildLiftDimensionsAfterReconnect(std::list<AcDbRotatedDimension*> horizantalDims, std::list<AcDbRotatedDimension*> verticalDims);
};
