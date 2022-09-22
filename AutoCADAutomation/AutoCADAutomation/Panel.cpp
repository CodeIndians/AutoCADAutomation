/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 11-Sep-2022	SatishD/Raghu	- Initial Creation
 *  * 
 * 22-Sep-2022 Satish D	- ABA-4 - Panel Strength
 */

#include "Panel.h"
#include <algorithm>
#include <unordered_set>
#include <limits>

Panel::Panel(BOUNDS boundinfo)
{
	bounds = boundinfo;
	internalPanelYOffset = (bounds.first.second + bounds.first.second) / 2;
	panelFFYPosition = 0.0f;
	bounds.second.second -= 250.0f; // panel texts below panel fix
	mOrigin = bounds.first;
	calculatePanelNameBounds();
	calculateDetailLabelsBounds();
	panelName = "Default Panel";
	totalThickness = "O inches";
	bHasInterferenceInInserts = false;

	mInternalPanelBounds.first.first = 0.0;
	mInternalPanelBounds.second.first = 0.0;
	mInternalPanelBounds.first.second = 0.0;
	mInternalPanelBounds.second.second = 0.0;
}

void Panel::addLiftInsert(CIRCLE &liftInsert)
{
	vecLiftInserts.push_back(liftInsert);
}

void Panel::addBraceInsert(COORDINATES &braceInsert)
{
	vecBraceInserts.push_back(braceInsert);
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
	return (panelFFYPosition - mInternalPanelBounds.first.second);
}

double Panel::getPanelHeightAboveFF()
{
	return (mInternalPanelBounds.second.second - panelFFYPosition);
}

std::string Panel::getRiggingType()
{
	return riggingType;
}

bool Panel::isElementWithinPanel(COORDINATES &element)
{
	return Utilities::getUtils()->boundCheck(bounds, element);
	//auto elementX = element.first;
	//auto elementY = element.second;
	//if (bounds.first.first <= elementX && bounds.second.first >= elementX)   // x coordinate check
	//	if (bounds.first.second <= elementY && bounds.second.second >= elementY) // y coordinate check
	//		return true;
	//return false;
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
	createDimensions(vecOpeningDimHorPoints,vecOpeningDimVerPoints, L"OPENING_DIMENSIONS");
	createDimensions(vecBraceDimHorPoints, vecBraceDimVerPoints, L"BRACE_INSERT_DIMENSIONS");
	createDimensions(vecLiftDimHorPoints, vecLiftDimVerPoints, L"LIFT_INSERT_DIMENSIONS");
	createDimensions(vecPanelDimHorPoints, vecPanelDimVerPoints, L"PANEL_DIMENSIONS");

}

void Panel::seperateFutureOpenings()
{
	std::vector<int> removeIndexes;
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

void Panel::updateRiggingType()
{
	if (vecLiftInserts.size() == 0)
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
	riggingType = "R" + std::to_string(y_positions.size()) + std::to_string(x_positions.size());
}

void Panel::filterCGLiftOpening()
{
	if (vecLiftInserts.size() == 0)
		return;
	double radius = 0;
	if (vecLiftInserts.size() >= 2)
	{
		radius = vecLiftInserts[0].second;
		if (vecLiftInserts[1].second > radius)
			radius = vecLiftInserts[1].second;
	}
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
		if(Utilities::getUtils()->approximatelyEqual(insert.first, x, std::numeric_limits<double>::epsilon()))
			matchingInsertsYPos.push_back(insert.second);
	}

	double minDistance = 40000000.0f; // random big value
	double finalYValue = y;
	if (matchingInsertsYPos.size())
	{
		for (auto& yPos : matchingInsertsYPos)
		{
			float distance = std::abs(y - yPos);
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
		if (Utilities::getUtils()->approximatelyEqual(insert.second, y, std::numeric_limits<double>::epsilon()))
			matchingInsertsXPos.push_back(insert.first);
	}

	double minDistance = 40000000.0f; // random big value
	double finalXValue = x;
	if (matchingInsertsXPos.size())
	{
		for (auto& xPos : matchingInsertsXPos)
		{
			float distance = std::abs(x - xPos);
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
			if (diff < 12.0f && !Utilities::getUtils()->approximatelyEqual(diff,12.0, std::numeric_limits<double>::epsilon()))
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
		std::sort(vecInteralPanelLines.begin(), vecInteralPanelLines.end());
		panelFFYPosition = vecInteralPanelLines[0].second;
		mInternalPanelBounds.first = vecInteralPanelLines[1];
		mInternalPanelBounds.second = vecInteralPanelLines[vecInteralPanelLines.size() - 2];
		internalPanelYOffset = mInternalPanelBounds.second.second;
	}
}
