/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 * 
 * 8-Oct-2022 Raghu - 2.1 Panel Details
 * 25-Oct-2022 SatishD - 2.9 Reveal positions
 */

#pragma once
#include "Panel.h"

class CollectionHelper
{
private:
	static std::list<CIRCLE> vecLiftInserts;	// lift inserts
	static std::list<CIRCLE> vecEdgeLifts;	//edge lifts
	static std::list<COORDINATES> vecBraceInserts;  // brace inserts
	static std::list<BOUNDS> vecOpenings; // valid openings
	static std::list<BOUNDS> vecReveals; // Reveals
	static std::list<COORDINATES> vecFutureOpenings; // text position "FUTURE"
	static std::list<LABELTEXT> vecTextLabels;	// text labels with text and coordinates
	static std::list<BOUNDS> vecDeadManLabels; // deadman leader labels
	static std::list<BOUNDS> vecInternalPanels; // internal panel rectangles
	static std::list<COORDINATES> vecInternalPanelLines; // internal panel lines

	//Dimensions 
	static std::list<BOUNDS> vecHorDimOpeningPlaceHolders;
	static std::list<BOUNDS> vecVerDimOpeningPlaceHolders;
	static std::list<BOUNDS> vecHorDimBracePlaceHolders;
	static std::list<BOUNDS> vecVerDimBracePlaceHolders;
	static std::list<BOUNDS> vecHorDimLiftPlaceHolders;
	static std::list<BOUNDS> vecVerDimLiftPlaceHolders;
	static std::list<BOUNDS> vecHorDimPanelPlaceHolders;
	static std::list<BOUNDS> vecVerDimPanelPlaceHolders;

	static std::list<AcDbRotatedDimension*> vecHorDimLiftPlaceHoldersAfterReconnect;
	static std::list<AcDbRotatedDimension*> vecVerDimLiftPlaceHoldersAfterReconnect;
	
	static void CollectCircles(AcDbEntity* entity, std::list<COORDINATES>& circles);
	static void CollectCircles(AcDbEntity* entity, std::list<CIRCLE>& circles);
	static void CollectArcs(AcDbEntity* entity, std::list<CIRCLE>& arcs);
	static void CollectRectanglesFromPolyLines(AcDbEntity* entity, std::list<BOUNDS>& rectangles);
	static void CollectHorizontalRecFromPolylines(AcDbEntity* entity, std::list<BOUNDS>& rectangles);
	static void CollectRectanglesFromBlockReferences(AcDbEntity* entity, std::list<BOUNDS>& rectangles);
	static void CollectLines(AcDbEntity* entity, std::list<COORDINATES>& points);
	static void CollectLinesWithAnAngle(AcDbEntity* entity, std::list<BOUNDS>& lineBounds); // acute angle lines
	static void CollectLinesWithAnAngle(AcDbEntity* entity, std::list<BOUNDS>& acuteLines, std::list<BOUNDS>& obtuseLines);
	static void CollectTextCordinates(AcDbEntity* entity, std::list<COORDINATES>& textCoordinates, bool isFuture = false);
	static void CollectTextLabels(AcDbEntity* entity, std::list<LABELTEXT>& listtextLabels);
	static void CollectTextLabelsFromLeaders(AcDbEntity* entity, std::list<BOUNDS>& listLeaderBounds);
	static void CreatePanelsFromBlockReferences(AcDbEntity* entity, std::list<Panel>& panels);
public:
	static void CollectLiftInserts(AcDbEntity* entity);
	static void CollectBraceInserts(AcDbEntity* entity);
	static void CollectOpenings(AcDbEntity* entity);
	static void CollectReveals(AcDbEntity* entity);
	static void CollectDefPoints(AcDbEntity* entity, std::list<Panel>& listDefPanel);
	static void CollectInternalPanels(AcDbEntity* entity);
	static void CollectAllTextLabels(AcDbEntity* entity);

	//Dimensions
	static void CollectOpeningDimensions(AcDbEntity* entity);
	static void CollectBraceDimensions(AcDbEntity* entity);
	static void CollectLiftDimensions(AcDbEntity* entity);
	static void CollectPanelDimensions(AcDbEntity* entity);
	static void CollectLiftDimensionsAfterReconnect(AcDbEntity* entity);

	static void PopulatePanelData(std::list<Panel>& panels);
	static void ClearCollectionData();
};

