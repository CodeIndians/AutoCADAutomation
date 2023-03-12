/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 * 22-Sep-2022 Satish D	- ABA-4 - Panel Strength
 * 8-Oct-2022 Raghu - 2.1 Panel Details
 * 16-Oct-2022 Raghu - 2.2 Similar Rigging
 * 25-Oct-2022 SatishD - 2.9 Reveal positions
 */

#include "Panel.h"
#include <algorithm>
#include <unordered_set>
#include <limits>
#include <float.h>

Panel::Panel(BOUNDS boundinfo)
{
	bounds = boundinfo;
	rebarBounds = boundinfo;
	internalPanelYOffset = (bounds.first.second + bounds.first.second) / 2;
	panelFFYPosition = 0.0f;
	bounds.second.second -= 250.0f; // panel texts below panel fix
	// set the rebar label collection bounds correctly
	rebarBounds.second.second -= 50.0f; 
	rebarBounds.first.first += 150.0f;
	rebarBounds.second.first += 150.0f;
	mOrigin = bounds.first;
	calculatePanelNameBounds();
	calculateDetailLabelsBounds();
	calculateNumRequiredLabelBounds();
	panelName = "Default Panel";
	totalThickness = "O inches";
	bHasInterferenceInInserts = false;
	rebarCoverExterior = "";
	rebarCoverInterior = "";
	mInternalPanelBounds.first.first = 0.0;
	mInternalPanelBounds.second.first = 0.0;
	mInternalPanelBounds.first.second = 0.0;
	mInternalPanelBounds.second.second = 0.0;
	uniqueRiggingTypeString = "NA";
}

void Panel::addLiftInsert(CIRCLE &liftInsert)
{
	vecLiftInserts.push_back(liftInsert);
}
void Panel::addEdgeLift(CIRCLE& edgeLift)
{
	vecEdgeLifts.push_back(edgeLift);
}

void Panel::addBraceInsert(COORDINATES &braceInsert)
{
	bool bFound = false;
	for (auto& insert : vecBraceInserts)
	{
		if (insert == braceInsert)
		{
			bFound = true;
			break;
		}
	}
	
	if(!bFound)
		vecBraceInserts.push_back(braceInsert);
}

void Panel::addReveal(BOUNDS& reveal)
{
	vecReveals.push_back(reveal);
}

void Panel::addOpening(BOUNDS& opening)
{
	vecOpenings.push_back(opening);
}

BOUNDS Panel::getPanelBounds()
{
	return bounds;
}

BOUNDS Panel::getInternalPanelBounds()
{
	return mInternalPanelBounds;
}

std::string& Panel::getPanelThickNess()
{
	return totalThickness;
}

double Panel::getPanelWidth()
{
	return Utilities::getUtils()->getBoundsWidth(mInternalPanelBounds);
}

double Panel::getPanelHeight()
{
	return Utilities::getUtils()->getBoundsHeight(mInternalPanelBounds);
}

double Panel::getPanelHeightBelowFF()
{
	return (panelFFYPosition - mInternalPanelBounds.first.second) >= 0.0f ? (panelFFYPosition - mInternalPanelBounds.first.second) : 0.0f;
}

double Panel::getPanelHeightAboveFF()
{
	return (mInternalPanelBounds.second.second - panelFFYPosition);
}

std::string Panel::getRiggingType()
{
	return riggingType;
}

std::string Panel::getPanelParameter(std::string key)
{
	if (panelDetailsMap.find(key) != panelDetailsMap.end())
		return panelDetailsMap[key];
	return "NA";
}

bool Panel::isElementWithinPanel(COORDINATES &element)
{
	return Utilities::getUtils()->boundCheck(bounds, element);
}

bool Panel::isElementWithinPanel(BOUNDS& element)
{
	if (isElementWithinPanel(element.first) && isElementWithinPanel(element.second))
		return true;
	return false;
}

bool Panel::isElementWithinPanel(CIRCLE& circle)
{
	return isElementWithinPanel(circle.first);
}

bool Panel::isElementWithinPanel(AcDbRotatedDimension* dim)
{
	COORDINATES element1 = std::make_pair(dim->xLine1Point().x, dim->xLine1Point().y);
	COORDINATES element2 = std::make_pair(dim->xLine2Point().x, dim->xLine2Point().y);
	BOUNDS boundElem = std::make_pair(element1, element2);
	
	if (isElementWithinPanel(boundElem))
		return true;
	else
		return false;
}

void Panel::addFutureTextCoordinates(COORDINATES& futuretextposition)
{
	vecFutureCoordinates.push_back(futuretextposition);
}

// all the panel update methods will go here
void Panel::updatePanel()
{
	seperateFutureOpenings();
	filterCGLiftOpening();
	updateRiggingType();
	detectInterferenceCheck();
	updateFFYPosition();
	generateUniqueRiggingString();

	// dimensions
	createDimensions(vecOpeningDimHorPoints,vecOpeningDimVerPoints, L"OPENING_DIMENSIONS");
	createDimensions(vecBraceDimHorPoints, vecBraceDimVerPoints, L"BRACE_INSERT_DIMENSIONS");
	createDimensions(vecLiftDimHorPoints, vecLiftDimVerPoints, L"LIFT_INSERT_DIMENSIONS");
	createDimensions(vecPanelDimHorPoints, vecPanelDimVerPoints, L"PANEL_DIMENSIONS");
}

void Panel::seperateFutureOpenings()
{
	for (auto& future : vecFutureCoordinates)
	{
		for (auto& opening : vecOpenings)
		{
			if (Utilities::getUtils()->boundCheck(opening, future))
				vecFutureOpenings.push_back(opening);
		}
	}
}

void Panel::updatePanelThickness()
{
	std::string validateString = "TOTAL THICKNESS: ";
	for (auto& detailLabel : vecDetailLabels)
	{
		auto pos = detailLabel.first.find(validateString) + validateString.length();
		if (detailLabel.first.find(validateString) != std::string::npos)
		{
			totalThickness = detailLabel.first.substr(pos);
		}
	}
}

void Panel::generatePanelDetailsMap()
{
	std::string key;
	std::string value;
	std::string validateString = ":";
	
	// push detail labels into map 
	// For example
	//	TOTAL THICKNESS is the key
	//	8" is the value 
	for (auto& detailLabel : vecDetailLabels)
	{
		auto pos = detailLabel.first.find(validateString);
		if (detailLabel.first.find(validateString) != std::string::npos)
		{
			key = detailLabel.first.substr(0, pos);
			value = detailLabel.first.substr(pos + 1);
			panelDetailsMap[key] = value;
		}
	}
	for (auto& detailLabel : vecPanelLabels)
	{
		auto pos = detailLabel.first.find(validateString);
		if (detailLabel.first.find(validateString) != std::string::npos)
		{
			key = detailLabel.first.substr(0, pos);
			value = detailLabel.first.substr(pos);
			panelDetailsMap[key] = value;
		}
	}
}

void Panel::generateRebarCovers()
{
	for (auto& rebar : vecRebarLabelsInsideInternalPanel)
	{
		if (rebarCoverExterior != "")
			break;
		auto pos1 = rebar.first.find("SPACED, ") + 8;
		auto pos2 = rebar.first.find(" CLR");
		if (rebar.first.find("@ BOT") != std::string::npos)
		{
			rebarCoverExterior = rebar.first.substr(pos1, pos2 - pos1);
		}
	}
	for (auto& rebar : vecRebarLabelsInsideInternalPanel)
	{
		if (rebarCoverInterior != "")
			break;
		auto pos1 = rebar.first.find("SPACED, ") + 8;
		auto pos2 = rebar.first.find(" CLR");
		if (rebar.first.find("@ TOP") != std::string::npos)
		{
			rebarCoverInterior = rebar.first.substr(pos1, pos2 - pos1);
		}
	}
	for (auto& rebar : vecRebarLabelsOutsideInternalPanel)
	{
		if (rebarCoverExterior != "")
			break;
		auto pos1 = rebar.first.find("SPACED, ") + 8;
		auto pos2 = rebar.first.find(" CLR");
		if (rebar.first.find("@ BOT") != std::string::npos)
		{
			rebarCoverExterior = rebar.first.substr(pos1, pos2 - pos1);
		}
	}
	for (auto& rebar : vecRebarLabelsOutsideInternalPanel)
	{
		if (rebarCoverInterior != "")
			break;
		auto pos1 = rebar.first.find("SPACED, ") + 8;
		auto pos2 = rebar.first.find(" CLR");
		if (rebar.first.find("@ TOP") != std::string::npos)
		{
			rebarCoverInterior = rebar.first.substr(pos1, pos2 - pos1);
		}
	}
}

// 2.2 :: ABA-3 :: Rigging Date ( Generate unique rigging string ) 
void Panel::generateUniqueRiggingString()
{
	std::vector<BOUNDS> floorOpenings;
	std::vector<BOUNDS> middleOpenings;
	for (auto& opening : vecOpenings)
	{
		// do not consider the future openings
		if (std::find(vecFutureOpenings.begin(), vecFutureOpenings.end(), opening) == vecFutureOpenings.end())
		{
			// floor opening condition
			// compare the y positions of floor opening and internal panel to identify the floor openings 
			if (Utilities::getUtils()->approximatelyEqual(opening.first.second, mInternalPanelBounds.first.second))
				floorOpenings.push_back(opening);
			else
				middleOpenings.push_back(opening);
		}
	}

	if (floorOpenings.size() > 0 && middleOpenings.size() > 0)
	{
		double highestFloorOpeningHeight = DBL_MAX;
		double highestMiddleOpeningHeight = DBL_MAX;

		// get the highest floor opening height
		for (auto& floorOpening : floorOpenings)
		{
			double height = mInternalPanelBounds.second.second - floorOpening.second.second;
			if (height < highestFloorOpeningHeight)
				highestFloorOpeningHeight = height;
		}

		// get the highest middle opening height
		for (auto& middleOpening : middleOpenings)
		{
			double height = mInternalPanelBounds.second.second - middleOpening.second.second;
			if (height < highestMiddleOpeningHeight)
				highestMiddleOpeningHeight = height;
		}

		uniqueRiggingTypeString = "F" + Utilities::getUtils()->inchesToFeet(highestFloorOpeningHeight) + "M" + Utilities::getUtils()->inchesToFeet(highestMiddleOpeningHeight);
	}
}
	

void Panel::createDimensions(std::vector<BOUNDS>& horizontalBounds, std::vector<BOUNDS>& verticalBounds, AcString layer)
{
	if (horizontalBounds.size() == 0 && verticalBounds.size() == 0)
		return;
	std::vector<COORDINATES> horizontalLines;
	std::vector<COORDINATES> verticalLines;
	for (auto& horizontalBound : horizontalBounds)
	{
		COORDINATES midPoint = Utilities::getUtils()->getMidPoint(horizontalBound);
		horizontalLines.push_back(midPoint);
	}
	for (auto& verticalBound : verticalBounds)
	{
		COORDINATES midPoint = Utilities::getUtils()->getMidPoint(verticalBound);
		verticalLines.push_back(midPoint);
	}
	std::unordered_set<double> horLinesY;
	std::unordered_set<double> verLinesX;
	for (auto& horline : horizontalLines)
	{
		horLinesY.insert(horline.second);
	}
	for (auto& verLine : verticalLines)
	{
		verLinesX.insert(verLine.first);
	}
	for (auto& horY : horLinesY)
	{
		std::vector<COORDINATES> singleLine;
		for (auto& point : horizontalLines)
		{
			if (horY == point.second)
				singleLine.push_back(point);
		}
		createDimension(singleLine,layer,true);
	}
	for (auto& verX : verLinesX)
	{
		std::vector<COORDINATES> singleLine;
		for (auto& point : verticalLines)
		{
			if (verX == point.first)
				singleLine.push_back(point);
		}
		createDimension(singleLine,layer,false);
	}
}

COORDINATES Panel::getAnchorPositionForRebarPlacement()
{
	COORDINATES anchorPosition = COORDINATES();

	// The anchor point for each panel is the position of the brace id detail label
	std::string validateString = "BRACE ID: ";
	for (auto& detailLabel : vecDetailLabels)
	{
		if (detailLabel.first.find(validateString) != std::string::npos)
		{
			anchorPosition = detailLabel.second;
			break;
		}
	}
	return anchorPosition;
}

void Panel::updateRiggingType()
{
	if (vecLiftInserts.size() == 0 && vecEdgeLifts.size() == 0)
	{
		riggingType = "N/A";
		return;
	}
	std::unordered_set<double> x_positions;
	std::unordered_set<double> y_positions;
	for (auto& lifts : vecLiftInserts)
	{
		x_positions.insert(lifts.first.first);
		y_positions.insert(lifts.first.second);
	}
	for (auto& lifts : vecEdgeLifts)
	{
		x_positions.insert(lifts.first.first);
		y_positions.insert(lifts.first.second);
	}
	riggingType = "R" + std::to_string(y_positions.size()) + std::to_string(x_positions.size());
}

void Panel::filterCGLiftOpening()
{
	if (vecLiftInserts.size() == 0 && vecEdgeLifts.size() == 0)
		return;
	
	// TODO: This logic might not be needed since circle size for the inserts are fixed
	double radius = 0.0f;
	if (vecLiftInserts.size() >= 2)
	{
		radius = vecLiftInserts[0].second;
		if (vecLiftInserts[1].second > radius)
			radius = vecLiftInserts[1].second;
	}
	if (vecEdgeLifts.size() >= 2)
	{
		radius = vecEdgeLifts[0].second;
		if (vecEdgeLifts[1].second > radius)
			radius = vecEdgeLifts[1].second;
	}

	radius = (radius == 0.0f) ? 4.0f : radius;

	bool foundCgCircle = false;
	for (auto& liftInsert : vecLiftInserts)
	{
		if (liftInsert.second < radius)
		{
			foundCgCircle = true;
			m_CG = liftInsert;
			break;
		}
	}
	if(foundCgCircle)
		vecLiftInserts.erase(std::remove(vecLiftInserts.begin(), vecLiftInserts.end(), m_CG));
}

void Panel::createDimension(std::vector<COORDINATES>& singleLine,AcString layer, bool isHorizontal)
{
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTableRecordPointer pBTR
	(
		acdbSymUtil()->blockModelSpaceId(pDb),
		AcDb::kForWrite
	);

	AcDbObjectId dimStyleId = pDb->dimstyle();
	double dimScale = pDb->dimscale();
	for (int i = 0; i < singleLine.size() - 1; i++)
	{
		double startX = singleLine[i].first;
		double startY = singleLine[i].second;
		double endX = singleLine[i + 1].first;
		double endY = singleLine[i + 1].second;

		projectToPanel(startX, startY, isHorizontal);
		projectToPanel(endX, endY, isHorizontal);
		projectToInserts(startX, startY, layer, isHorizontal);
		projectToInserts(endX, endY, layer, isHorizontal);

		if (layer == L"BRACE_INSERT_DIMENSIONS")
		{
			// filter out invalid dimensions
			if (singleLine.size() == 2)
			{
				if (isHorizontal)
				{
					if (std::abs(startX - endX) >= std::abs(mInternalPanelBounds.first.first - mInternalPanelBounds.second.first))
						continue;
				}
				else
				{
					if (std::abs(startY - endY) >= std::abs(mInternalPanelBounds.first.second - mInternalPanelBounds.second.second))
						continue;
				}
			}
		}

		AcGePoint3d start(startX, startY, 0.0f);
		AcGePoint3d end(endX, endY, 0.0f);
		AcGePoint3d dimLine((singleLine[i].first + singleLine[i+1].first) / 2, (singleLine[i].second + singleLine[i+1].second) / 2, 0.0f);
		double rotation = isHorizontal ? 0.0f : -1.5708;
		AcDbRotatedDimension* pDim = new AcDbRotatedDimension(rotation, start, end, dimLine, NULL, dimStyleId);
		pDim->setLayer(layer);
		pBTR->appendAcDbEntity(pDim);

		pDim->close();
	}
	pBTR->close();
}

void Panel::projectToPanel(double& xPos, double& yPos, bool isHorizontal)
{
	if (isHorizontal)
	{
		// below panel
		if (yPos < mInternalPanelBounds.first.second)
			yPos = mInternalPanelBounds.first.second;

		//above panel
		if (yPos > mInternalPanelBounds.second.second)
			yPos = mInternalPanelBounds.second.second;
	}
	else
	{
		// left side panel
		if (xPos < mInternalPanelBounds.first.first)
			xPos = mInternalPanelBounds.first.first;

		//right side panel
		if (xPos > mInternalPanelBounds.second.first)
			xPos = mInternalPanelBounds.second.first;
	}
}

void Panel::projectToInserts(double& xPos, double& yPos, AcString& layer, bool isHorizontal)
{
	if ( layer == L"BRACE_INSERT_DIMENSIONS" )
	{
		if (isHorizontal)
			projectToNearestVerticalInsert(xPos, yPos, vecBraceInserts);
		else
			projectToNearestHorizontalInsert(xPos, yPos, vecBraceInserts);
	}
	if (layer == L"LIFT_INSERT_DIMENSIONS")
	{
		if (isHorizontal)
			projectToNearestVerticalInsert(xPos, yPos, vecLiftInserts);
		else
			projectToNearestHorizontalInsert(xPos, yPos, vecLiftInserts);
	}
}

// insert position from a horizonal line 
void Panel::projectToNearestVerticalInsert(double &x, double &y,std::vector<COORDINATES>& inserts)
{
	std::vector<double> matchingInsertsYPos;
	for (auto& insert : inserts)
	{
		if(Utilities::getUtils()->approximatelyEqual(insert.first, x))
			matchingInsertsYPos.push_back(insert.second);
	}

	double minDistance = 40000000.0f; // random big value
	double finalYValue = y;
	if (matchingInsertsYPos.size())
	{
		for (auto& yPos : matchingInsertsYPos)
		{
			double distance = std::abs(y - yPos);
			if (distance < minDistance)
			{
				finalYValue = yPos;
				minDistance = distance;
			}
		}
	}

	double offset = (y - finalYValue) * 0.3; //offset is 30% of the difference

	y = finalYValue + offset;
}

// insert position from a vertical line 
void Panel::projectToNearestHorizontalInsert(double& x, double& y, std::vector<COORDINATES>& inserts)
{
	std::vector<double> matchingInsertsXPos;
	for (auto& insert : inserts)
	{
		if (Utilities::getUtils()->approximatelyEqual(insert.second, y))
			matchingInsertsXPos.push_back(insert.first);
	}

	double minDistance = 40000000.0f; // random big value
	double finalXValue = x;
	if (matchingInsertsXPos.size())
	{
		for (auto& xPos : matchingInsertsXPos)
		{
			double distance = std::abs(x - xPos);
			if (distance < minDistance)
			{
				finalXValue = xPos;
				minDistance = distance;
			}
		}
	}
	double offset = (x - finalXValue) * 0.3; //offset is 30% of the difference

	x = finalXValue + offset;
}

// insert position from a horizonal line 
void Panel::projectToNearestVerticalInsert(double& x, double& y, std::vector<CIRCLE>& inserts)
{
	std::vector<COORDINATES> insertCoords;
	for (auto& insert : inserts)
	{
		insertCoords.push_back(insert.first);
	}
	projectToNearestVerticalInsert(x, y, insertCoords);
}

// insert position from a vertical line 
void Panel::projectToNearestHorizontalInsert(double& x, double& y, std::vector<CIRCLE>& inserts)
{
	std::vector<COORDINATES> insertCoords;
	for (auto& insert : inserts)
	{
		insertCoords.push_back(insert.first);
	}
	projectToNearestHorizontalInsert(x, y, insertCoords);
}

// create a rect starting from the right bottom def rectangle which is used to collect panels names
void Panel::calculatePanelNameBounds()
{
	mPanelNameBounds.first.first = bounds.second.first;
	mPanelNameBounds.first.second = bounds.first.second - 45.0f;
	mPanelNameBounds.second.first = bounds.second.first + 85.0f;
	mPanelNameBounds.second.second = bounds.first.second;
}

void Panel::calculateDetailLabelsBounds()
{
	mDetailLabelsBounds.first.first = bounds.first.first;
	mDetailLabelsBounds.first.second = bounds.first.second - 155.0f;
	mDetailLabelsBounds.second.first = bounds.second.first;
	mDetailLabelsBounds.second.second = bounds.first.second;
}

void Panel::calculateNumRequiredLabelBounds()
{
	mNumRequiredBounds.first.first = bounds.second.first - 85.0f;
	mNumRequiredBounds.first.second = bounds.first.second - 45.0f;
	mNumRequiredBounds.second.first = bounds.second.first;
	mNumRequiredBounds.second.second = bounds.first.second;
}

void Panel::addPanelLabels(LABELTEXT& panelLabels)
{
	vecPanelLabels.push_back(panelLabels);
}

void Panel::addDetailLabels(LABELTEXT& detailLabels)
{
	vecDetailLabels.push_back(detailLabels);
}

void Panel::addRebarLabels(LABELTEXT& rebarLabels, bool inside)
{
	if (inside)
		vecRebarLabelsInsideInternalPanel.push_back(rebarLabels);
	else
		vecRebarLabelsOutsideInternalPanel.push_back(rebarLabels);
}

void Panel::addDeadmanLabels(BOUNDS& deadmanLabels)
{
	vecDeadmanLabels.push_back(deadmanLabels);
}

void Panel::addInternalPanelBounds(BOUNDS& internalPanelBounds)
{
	if (Utilities::getUtils()->DistanceBetweenPoints(mInternalPanelBounds) > Utilities::getUtils()->DistanceBetweenPoints(internalPanelBounds))
		return;

	mInternalPanelBounds = internalPanelBounds;
	internalPanelYOffset = mInternalPanelBounds.second.second;
}

void Panel::addInternalPanelLines(COORDINATES& internalPanelLines)
{
	vecInteralPanelLines.push_back(internalPanelLines);
}

void Panel::addOpeningDimensions(BOUNDS& dimensions, bool isHorizontal)
{
	if (isHorizontal)
		vecOpeningDimHorPoints.push_back(dimensions);
	else
		vecOpeningDimVerPoints.push_back(dimensions);
}

void Panel::addBraceDimensions(BOUNDS& dimensions, bool isHorizontal)
{
	if (isHorizontal)
		vecBraceDimHorPoints.push_back(dimensions);
	else
		vecBraceDimVerPoints.push_back(dimensions);
}

void Panel::addLiftDimensions(BOUNDS& dimensions, bool isHorizontal)
{
	if (isHorizontal)
		vecLiftDimHorPoints.push_back(dimensions);
	else
		vecLiftDimVerPoints.push_back(dimensions);
}

void Panel::addLiftDimensionsAfterReconnect(AcDbRotatedDimension* dimensions, bool isHorizontal)
{
	if (isHorizontal)
		vecLiftDimHorPointsAfterReconnect.push_back(dimensions);
	else
		vecLiftDimVerPointsAfterReconnect.push_back(dimensions);
}

void Panel::addPanelDimensions(BOUNDS& dimensions, bool isHorizontal)
{
	if (isHorizontal)
		vecPanelDimHorPoints.push_back(dimensions);
	else
		vecPanelDimVerPoints.push_back(dimensions);
}

bool Panel::areRebarTextsMatching()
{
	std::list<LABELTEXT> outsideRebarsLabelCopy;
	std::copy(vecRebarLabelsOutsideInternalPanel.begin(), vecRebarLabelsOutsideInternalPanel.end(), std::back_inserter(outsideRebarsLabelCopy));
	bool matching = true;
	if (vecRebarLabelsOutsideInternalPanel.size() == 0 && vecRebarLabelsInsideInternalPanel.size() > 0)
	{
		std::wstring widestr = std::wstring(panelName.begin(), panelName.end()) + std::wstring( L" outer labels are not found");
		acutPrintf(widestr.c_str());
	}
	for (auto& insideLabel : vecRebarLabelsInsideInternalPanel)
	{
		if (!matching)
			break;
		matching = false;
		LABELTEXT matchingLabel;
		for (auto& outsideLabel : outsideRebarsLabelCopy)
		{
			if (insideLabel.first == outsideLabel.first)
			{
				matching = true;
				matchingLabel = outsideLabel;
			}
		}
		if (matching)
			outsideRebarsLabelCopy.remove(matchingLabel);
	}
	return matching;
}

std::string Panel::getOpeningType(BOUNDS& bound)
{
	bool isFuture = false;
	for (auto& opening : vecFutureOpenings)
	{
		if (opening == bound)
		{
			isFuture = true;
			break;
		}
	}
	return isFuture ? "K.O." : "OP";
}

bool Panel::isOutsideDeadmanLabelPresent()
{
	if (vecDeadmanLabels.size() > 3)
		acutPrintf(L"Deadman labels logic needs to be changed");
	return (vecDeadmanLabels.size() == 1) || (vecDeadmanLabels.size() == 3);
}

bool Panel::isInsideDeadmanLabelPresent()
{
	if (vecDeadmanLabels.size() > 3)
		acutPrintf(L"Deadman labels logic needs to be changed");
	return (vecDeadmanLabels.size() == 2) || (vecDeadmanLabels.size() == 3);
}

BOUNDS& Panel::getPanelNameBounds()
{
	return mPanelNameBounds;
}

CIRCLE Panel::GetCG()
{
	return m_CG;
}

BOUNDS& Panel::getDetailLableBounds()
{
	return mDetailLabelsBounds;
}

BOUNDS& Panel::getNumRequiredBounds()
{
	return mNumRequiredBounds;
}

BOUNDS& Panel::getRebarLabelBounds()
{
	return rebarBounds;
}

double Panel::getInternalPanelYOffset()
{
	return internalPanelYOffset;
}

std::string& Panel::getPanelName()
{
	return panelName;
}

bool Panel::isInterferenceDetected()
{
	return bHasInterferenceInInserts;
}

void Panel::setPanelName(std::string name)
{
	panelName = name;
}

void Panel::setNumRequired(std::string numrequired)
{
	numRequired = numrequired;
}

void Panel::detectInterferenceCheck()
{
	for (auto& braceInsert : vecBraceInserts)
	{
		if (bHasInterferenceInInserts)
			break;

		auto braceX = braceInsert.first;

		for (auto& liftInsert : vecLiftInserts)
		{
			auto diff = std::abs(liftInsert.first.first - braceX);
			if (diff < 12.0f && !Utilities::getUtils()->approximatelyEqual(diff,12.0))
			{
				bHasInterferenceInInserts = true;
				break;
			}
		}
	}
}

void Panel::updateFFYPosition()
{
	if (vecInteralPanelLines.size() == 0)
		return;
	//logic for closed panels
	if (vecInteralPanelLines.size() == 2)
	{
		panelFFYPosition = vecInteralPanelLines[0].second;
	}
	else if (vecInteralPanelLines.size() == 4)
	{
		std::sort(vecInteralPanelLines.begin(), vecInteralPanelLines.end());
		panelFFYPosition = vecInteralPanelLines[0].second;
	}
	// logic for broken panels
	else
	{
		// sort all the points based on X cordinates first
		std::sort(vecInteralPanelLines.begin(), vecInteralPanelLines.end(), [](const COORDINATES& point1, const COORDINATES& point2) {
			return point1.first < point2.first;
			});

		// remove the duplicate points ( this might not be needed at all, keeping it anyway)
		vecInteralPanelLines.erase(std::unique(vecInteralPanelLines.begin(), vecInteralPanelLines.end()), vecInteralPanelLines.end());

		panelFFYPosition = vecInteralPanelLines[0].second;

		BOUNDS tempBounds;
		mInternalPanelBounds.first = vecInteralPanelLines[1]; // second lowest x point
		mInternalPanelBounds.second = vecInteralPanelLines[vecInteralPanelLines.size() - 2]; // seccond highest x point

		std::sort(vecInteralPanelLines.begin(), vecInteralPanelLines.end(), [](const COORDINATES& point1, const COORDINATES& point2) {
					return point1.second < point2.second; // sort all the panel points based on Y co-ordinate
			});

		mInternalPanelBounds.first.second = vecInteralPanelLines[0].second;  // lowest panel Y position 
		mInternalPanelBounds.second.second = vecInteralPanelLines[vecInteralPanelLines.size() - 1].second;  //highest panel Y position

		if (Utilities::getUtils()->isNullBound(mInternalPanelBounds) && Utilities::getUtils()->isBoundWidthGreater(tempBounds,mInternalPanelBounds))
		{
			mInternalPanelBounds = tempBounds;
		}
		internalPanelYOffset = mInternalPanelBounds.second.second;
	}
}
