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

#include "CollectionHelper.h"
#include "PanelBuilder.h"

std::list<CIRCLE> CollectionHelper::vecLiftInserts;	//lift inserts
std::list<COORDINATES> CollectionHelper::vecBraceInserts;  // brace inserts
std::list<BOUNDS> CollectionHelper::vecReveals; // Reveals collection
std::list<BOUNDS> CollectionHelper::vecOpenings; // valid openings
std::list<COORDINATES> CollectionHelper::vecFutureOpenings; // text position "FUTURE"
std::list<LABELTEXT> CollectionHelper::vecTextLabels;	// text labels with text and coordinates
std::list<BOUNDS> CollectionHelper::vecDeadManLabels;	// deadman label bounds
std::list<BOUNDS> CollectionHelper::vecInternalPanels; // internal panel rectangles
std::list<COORDINATES> CollectionHelper::vecInternalPanelLines; // Internal Panel lines

//Dimensions Reconnect
std::list<BOUNDS> CollectionHelper::vecHorDimOpeningPlaceHolders;
std::list<BOUNDS> CollectionHelper::vecVerDimOpeningPlaceHolders;
std::list<BOUNDS> CollectionHelper::vecHorDimBracePlaceHolders;
std::list<BOUNDS> CollectionHelper::vecVerDimBracePlaceHolders;
std::list<BOUNDS> CollectionHelper::vecHorDimLiftPlaceHolders;
std::list<BOUNDS> CollectionHelper::vecVerDimLiftPlaceHolders;
std::list<BOUNDS> CollectionHelper::vecHorDimPanelPlaceHolders;
std::list<BOUNDS> CollectionHelper::vecVerDimPanelPlaceHolders;

//Dimensions after reconnect
std::list<AcDbRotatedDimension*> CollectionHelper::vecHorDimLiftPlaceHoldersAfterReconnect;
std::list<AcDbRotatedDimension*> CollectionHelper::vecVerDimLiftPlaceHoldersAfterReconnect;

void CollectionHelper::CollectCircles(AcDbEntity* entity, std::list<COORDINATES>& circles)
{
	AcDbCircle* circle = (AcDbCircle*)entity;
	if (circle)
	{
		COORDINATES coordinatepair;
		auto centerPoint = circle->center();
		coordinatepair.first = centerPoint.x;
		coordinatepair.second = centerPoint.y;
		circles.push_back(coordinatepair);
	}
}

void CollectionHelper::CollectCircles(AcDbEntity* entity, std::list<CIRCLE>& circles)
{
	AcDbCircle* circle = (AcDbCircle*)entity;
	if (circle)
	{
		CIRCLE circleElement;
		auto centerPoint = circle->center();
		circleElement.first.first = centerPoint.x;
		circleElement.first.second = centerPoint.y;
		circleElement.second = circle->radius();
		circles.push_back(circleElement);
	}
}

void CollectionHelper::CollectRectanglesFromPolyLines(AcDbEntity* entity, std::list<BOUNDS>& rectangles)
{
	AcDbPolyline* rectangle = (AcDbPolyline*)entity;
	if (rectangle)
	{
		BOUNDS bound;
		AcDbExtents* boundInfo = new AcDbExtents();
		rectangle->bounds(*boundInfo);
		bound.first.first = boundInfo->minPoint().x;
		bound.first.second = boundInfo->minPoint().y;
		bound.second.first = boundInfo->maxPoint().x;
		bound.second.second = boundInfo->maxPoint().y;
		rectangles.push_back(bound);
		delete boundInfo;
	}
}

void CollectionHelper::CollectHorizontalRecFromPolylines(AcDbEntity* entity, std::list<BOUNDS>& rectangles)
{
	AcDbPolyline* rectangle = (AcDbPolyline*)entity;
	if (rectangle)
	{
		BOUNDS bound;
		AcDbExtents* boundInfo = new AcDbExtents();
		rectangle->bounds(*boundInfo);
		bound.first.first = boundInfo->minPoint().x;
		bound.first.second = boundInfo->minPoint().y;
		bound.second.first = boundInfo->maxPoint().x;
		bound.second.second = boundInfo->maxPoint().y;

		if((abs(bound.first.first - bound.second.first) > abs(bound.first.second - bound.second.second)) && (abs(bound.first.second - bound.second.second) < 10.0))
			rectangles.push_back(bound);
		                                                                          
		delete boundInfo;
	}
}

void CollectionHelper::CollectRectanglesFromBlockReferences(AcDbEntity* entity, std::list<BOUNDS>& rectangles)
{
	AcDbBlockReference* rectangle = (AcDbBlockReference*)entity;
	if (rectangle)
	{
		BOUNDS bound;
		AcDbExtents* boundInfo = new AcDbExtents();
		rectangle->bounds(*boundInfo);
		bound.first.first = boundInfo->minPoint().x;
		bound.first.second = boundInfo->minPoint().y;
		bound.second.first = boundInfo->maxPoint().x;
		bound.second.second = boundInfo->maxPoint().y;
		rectangles.push_back(bound);
		delete boundInfo;
	}
}

void CollectionHelper::CollectLines(AcDbEntity* entity, std::list<COORDINATES>& points)
{
	AcDbLine* line = (AcDbLine*)entity;
	if (line)
	{
		AcGePoint3d* start = new AcGePoint3d();
		AcGePoint3d* end = new AcGePoint3d();
		line->getStartPoint(*start);
		line->getEndPoint(*end);
		COORDINATES posStart, posEnd;
		posStart.first = start->x;
		posStart.second = start->y;
		points.push_back(posStart);
		posEnd.first = end->x;
		posEnd.second = end->y;
		points.push_back(posEnd);
		delete start;
		delete end;
	}
}

void CollectionHelper::CollectLinesWithAnAngle(AcDbEntity* entity, std::list<BOUNDS>& lineBounds)
{
	AcDbLine* line = (AcDbLine*)entity;
	if (line)
	{
		AcGePoint3d* start = new AcGePoint3d();
		AcGePoint3d* end = new AcGePoint3d();
		AcGePoint3d* temp;
		line->getStartPoint(*start);
		line->getEndPoint(*end);
		if (start->x > end->x)
		{
			temp = start;
			start = end;
			end = temp;
		}
		if ((start->x < end->x) && (start->y < end->y)) // acute angled lines
		{
			BOUNDS bound;
			bound.first.first = start->x;
			bound.first.second = start->y;
			bound.second.first = end->x;
			bound.second.second = end->y;
			lineBounds.push_back(bound);
		}
		delete start;
		delete end;
	}
}

void CollectionHelper::CollectLinesWithAnAngle(AcDbEntity* entity, std::list<BOUNDS>& acuteLines, std::list<BOUNDS>& obtuseLines)
{
	AcDbLine* line = (AcDbLine*)entity;
	if (line)
	{
		AcGePoint3d* start = new AcGePoint3d();
		AcGePoint3d* end = new AcGePoint3d();
		AcGePoint3d* temp;
		line->getStartPoint(*start);
		line->getEndPoint(*end);
		if (start->x > end->x)
		{
			temp = start;
			start = end;
			end = temp;
		}
		BOUNDS bound;
		bound.first.first = start->x;
		bound.first.second = start->y;
		bound.second.first = end->x;
		bound.second.second = end->y;
		if ((start->x < end->x) && (start->y < end->y)) // acute angled lines
		{
			acuteLines.push_back(bound);
		}
		else if ((start->x < end->x) && (start->y > end->y)) // obtuse angled lines
		{
			obtuseLines.push_back(bound);
		}
		delete start;
		delete end;
	}
}

void CollectionHelper::CollectTextCordinates(AcDbEntity* entity, std::list<COORDINATES>& textCoordinates, bool isFuture)
{
	AcDbText* text = (AcDbText*)entity;
	if (text)
	{
		if (isFuture )
		{
			if (wcscmp(text->textString(), L"FUTURE") == 0)
			{
				COORDINATES coordinate;
				auto pos = text->position();
				coordinate.first = pos.x;
				coordinate.second = pos.y;
				textCoordinates.push_back(coordinate);
			}
		}
		else
		{
			COORDINATES coordinate;
			auto pos = text->position();
			coordinate.first = pos.x;
			coordinate.second = pos.y;
			textCoordinates.push_back(coordinate);
		}
	}
}

void CollectionHelper::CollectTextLabels(AcDbEntity* entity, std::list<LABELTEXT>& listTextLabels)
{
	AcDbText* textLabels = (AcDbText*)entity;

	if (textLabels)
	{
		ACHAR* text = textLabels->textString();

		std::wstring ws(text);
		std::string str(ws.begin(), ws.end());

		COORDINATES cText;
		cText.first = textLabels->position().x;
		cText.second = textLabels->position().y;

		listTextLabels.push_back(LABELTEXT(str, cText));
	}
}

// Just store the bounds, storing text is not needed since we need to know only if the 
// leader is inside the panel or outside the internal panel
void CollectionHelper::CollectTextLabelsFromLeaders(AcDbEntity* entity, std::list<BOUNDS>& listLeaderBounds)
{
	AcDbLeader* textLeader = (AcDbLeader*)entity;

	if (textLeader)
	{
		BOUNDS bound;
		AcDbExtents* boundInfo = new AcDbExtents();
		textLeader->bounds(*boundInfo);
		bound.first.first = boundInfo->minPoint().x;
		bound.first.second = boundInfo->minPoint().y;
		bound.second.first = boundInfo->maxPoint().x;
		bound.second.second = boundInfo->maxPoint().y;
		listLeaderBounds.push_back(bound);
		delete boundInfo;
	}
}

void CollectionHelper::CreatePanelsFromBlockReferences(AcDbEntity* entity, std::list<Panel>& panels)
{
	if ((wcscmp(entity->layer(), L"DEFPOINTS") == 0) && (wcscmp(entity->isA()->name(), L"AcDbBlockReference") == 0))
	{
		//AcDbBlockReference
		AcDbBlockReference* rectangle = (AcDbBlockReference*)entity;
		if (rectangle)
		{
			BOUNDS bound;
			AcDbExtents* boundInfo = new AcDbExtents();
			rectangle->bounds(*boundInfo);
			bound.first.first = boundInfo->minPoint().x;
			bound.first.second = boundInfo->minPoint().y;
			bound.second.first = boundInfo->maxPoint().x;
			bound.second.second = boundInfo->maxPoint().y;
			panels.push_back(Panel(bound));
			delete boundInfo;
		}
	}
}

void CollectionHelper::CollectLiftInserts(AcDbEntity* entity)
{
	if ((wcscmp(entity->layer(), L"LIFT_INSERT") == 0) && (wcscmp(entity->isA()->name(), L"AcDbCircle") == 0))
		CollectCircles(entity, vecLiftInserts);
}

void CollectionHelper::CollectBraceInserts(AcDbEntity* entity)
{
	if ((wcscmp(entity->layer(), L"BRACE_INSERT") == 0) && (wcscmp(entity->isA()->name(), L"AcDbCircle") == 0))
		CollectCircles(entity, vecBraceInserts);
}

void CollectionHelper::CollectOpenings(AcDbEntity* entity)
{
	//collect openings
	if (wcscmp(entity->layer(), L"OPENING") == 0)
	{
		// collect rectangles - closed openings
		if (wcscmp(entity->isA()->name(), L"AcDbLine") == 0)
			CollectLinesWithAnAngle(entity, vecOpenings);
		else if (wcscmp(entity->isA()->name(), L"AcDbText") == 0)
			CollectTextCordinates(entity, vecFutureOpenings, true);
	}
}

void CollectionHelper::CollectReveals(AcDbEntity* entity)
{
	//collect openings
	if (wcscmp(entity->layer(), L"REVEAL") == 0)
	{
		acutPrintf(entity->isA()->name());
		// collect rectangles - closed openings
		if (wcscmp(entity->isA()->name(), L"AcDbPolyline") == 0)
			CollectHorizontalRecFromPolylines(entity, vecReveals);
	}
}

void CollectionHelper::CollectDefPoints(AcDbEntity* entity, std::list<Panel>& listDefPanels)
{
	if ((wcscmp(entity->layer(), L"DEFPOINTS") == 0) && (wcscmp(entity->isA()->name(), L"AcDbBlockReference") == 0))
		CreatePanelsFromBlockReferences(entity, listDefPanels);
}

void CollectionHelper::CollectInternalPanels(AcDbEntity* entity)
{
	if ((wcscmp(entity->layer(), L"PANEL") == 0) )
	{
		if(wcscmp(entity->isA()->name(), L"AcDbPolyline") == 0)
			CollectRectanglesFromPolyLines(entity, vecInternalPanels);
		if (wcscmp(entity->isA()->name(), L"AcDbLine") == 0)
			CollectLines(entity, vecInternalPanelLines);
	}
}

void CollectionHelper::CollectAllTextLabels(AcDbEntity* entity)
{
	if ((wcscmp(entity->layer(), L"REINFORCING_TEXT") == 0) && (wcscmp(entity->isA()->name(), L"AcDbText") == 0))
		CollectTextLabels(entity, vecTextLabels);
	if ((wcscmp(entity->layer(), L"PANEL_TEXT") == 0) && ((wcscmp(entity->isA()->name(), L"AcDbLeader") == 0) || (wcscmp(entity->isA()->name(), L"AcDbMLeader") == 0)) )
	{
		CollectTextLabelsFromLeaders(entity, vecDeadManLabels);
	}
	if ((wcscmp(entity->layer(), L"PANEL_TEXT") == 0))
	{
		auto name = entity->isA()->name();
		int i = 10;
	}
}

void CollectionHelper::CollectOpeningDimensions(AcDbEntity* entity)
{
	if (wcscmp(entity->layer(), L"OPENING_DIMENSIONS") == 0)
	{
		if ((wcscmp(entity->isA()->name(), L"AcDbLine") == 0))
			CollectLinesWithAnAngle(entity, vecHorDimOpeningPlaceHolders, vecVerDimOpeningPlaceHolders);
		if (wcscmp(entity->isA()->name(), L"AcDbRotatedDimension"))
			entity->erase(); // remove the dimensions after collecting them
	}
}

void CollectionHelper::CollectBraceDimensions(AcDbEntity* entity)
{
	if (wcscmp(entity->layer(), L"BRACE_INSERT_DIMENSIONS") == 0)
	{
		if ((wcscmp(entity->isA()->name(), L"AcDbLine") == 0))
			CollectLinesWithAnAngle(entity, vecHorDimBracePlaceHolders, vecVerDimBracePlaceHolders);
		if (wcscmp(entity->isA()->name(), L"AcDbRotatedDimension"))
		{
			if (wcscmp(entity->isA()->name(), L"AcDbText") == 0)
			{
				AcDbText* text = (AcDbText*)entity;
				if (text)
				{
					if (wcscmp(text->textString(), L"(BRACE INSERTS)"))
						entity->erase();
				}
			}
			else
				entity->erase(); // remove the dimensions after collecting them
		}
			
	}
}

void CollectionHelper::CollectLiftDimensions(AcDbEntity* entity)
{
	if (wcscmp(entity->layer(), L"LIFT_INSERT_DIMENSIONS") == 0)
	{
		if ((wcscmp(entity->isA()->name(), L"AcDbLine") == 0))
			CollectLinesWithAnAngle(entity, vecHorDimLiftPlaceHolders, vecVerDimLiftPlaceHolders);
		if (wcscmp(entity->isA()->name(), L"AcDbRotatedDimension"))
		{
			if (wcscmp(entity->isA()->name(), L"AcDbText") == 0)
			{
				AcDbText* text = (AcDbText*)entity;
				if (text)
				{
					if (wcscmp(text->textString(), L"(LIFT INSERTS)"))
						entity->erase();
				}
			}
			else
				entity->erase(); // remove the dimensions after collecting them
		}
	}
}

void CollectionHelper::CollectLiftDimensionsAfterReconnect(AcDbEntity* entity)
{
	if (wcscmp(entity->layer(), L"LIFT_INSERT_DIMENSIONS") == 0)
	{
		if (wcscmp(entity->isA()->name(), L"AcDbRotatedDimension") == 0)
		{
			AcDbRotatedDimension* dim = (AcDbRotatedDimension*)entity;
			AcGePoint3d start = dim->xLine1Point();
			AcGePoint3d end = dim->xLine2Point();
			AcGePoint3d dimLine = dim->dimLinePoint();
			double rotation = dim->rotation();

			if (rotation == 0)
				vecHorDimLiftPlaceHoldersAfterReconnect.push_back(dim);
			else
				vecVerDimLiftPlaceHoldersAfterReconnect.push_back(dim);
		}
	}
}

void CollectionHelper::CollectPanelDimensions(AcDbEntity* entity)
{
	if (wcscmp(entity->layer(), L"PANEL_DIMENSIONS") == 0)
	{
		if ((wcscmp(entity->isA()->name(), L"AcDbLine") == 0))
			CollectLinesWithAnAngle(entity, vecHorDimPanelPlaceHolders, vecVerDimPanelPlaceHolders);
		if (wcscmp(entity->isA()->name(), L"AcDbRotatedDimension"))
		{
			if (wcscmp(entity->isA()->name(), L"AcDbText") == 0)
			{
				AcDbText* text = (AcDbText*)entity;
				if (text)
				{
					if (wcscmp(text->textString(), L" FIN FLR "))
						entity->erase();
				}
			}
			else
				entity->erase(); // remove the dimensions after collecting them
		}
	}
}

void CollectionHelper::PopulatePanelData(std::list<Panel>& panels)
{
	// collect relevant elements into their respective panels
	for (auto& panel : panels)
	{
		// TODO: Optimize for better performance
		// collect the added coordiante indexes from the list and delete them on the next loop
		auto panelBuilder = PanelBuilder(panel);
		panelBuilder.buildLiftInserts(vecLiftInserts);
		panelBuilder.buildBraceInserts(vecBraceInserts);
		panelBuilder.buildReveals (vecReveals);
		panelBuilder.buildOpenings(vecOpenings, vecFutureOpenings);
		panelBuilder.buildInternalPanel(vecInternalPanels, vecInternalPanelLines);

		//dimensions
		panelBuilder.buildOpeningDimensions(vecHorDimOpeningPlaceHolders, vecVerDimOpeningPlaceHolders);
		panelBuilder.buildBraceDimensions(vecHorDimBracePlaceHolders, vecVerDimBracePlaceHolders);
		panelBuilder.buildLiftDimensions(vecHorDimLiftPlaceHolders, vecVerDimLiftPlaceHolders);
		panelBuilder.buildPanelDimensions(vecHorDimPanelPlaceHolders, vecVerDimPanelPlaceHolders);

		// dimensions after reconnect
		panelBuilder.buildLiftDimensionsAfterReconnect(vecHorDimLiftPlaceHoldersAfterReconnect, vecVerDimLiftPlaceHoldersAfterReconnect);

		panelBuilder.updatePanel();
		panelBuilder.buildLabels(vecTextLabels);
		panelBuilder.buildDeadManLabels(vecDeadManLabels);
	}	
}

void CollectionHelper::ClearCollectionData()
{
	vecLiftInserts.clear();
	vecBraceInserts.clear();
	vecOpenings.clear();
	vecFutureOpenings.clear();
	vecTextLabels.clear();
	vecDeadManLabels.clear();
	vecInternalPanels.clear();
	vecInternalPanelLines.clear();
	vecReveals.clear();

	//dimensions
	vecHorDimOpeningPlaceHolders.clear();
	vecVerDimOpeningPlaceHolders.clear();
	vecHorDimBracePlaceHolders.clear();
	vecVerDimBracePlaceHolders.clear();
	vecHorDimLiftPlaceHolders.clear();
	vecVerDimLiftPlaceHolders.clear();
	vecHorDimPanelPlaceHolders.clear();
	vecVerDimPanelPlaceHolders.clear();
}
