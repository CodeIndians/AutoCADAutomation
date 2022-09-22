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

#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <windows.h>
// Load the common AutoCAD headers
#include "arxHeaders.h"
#include "dbents.h"
#include <list>
#include "Utilities.h"

typedef std::pair<double, double> COORDINATES;
typedef std::pair< COORDINATES, COORDINATES> BOUNDS;
typedef std::pair<std::string, COORDINATES> LABELTEXT;
typedef std::pair<COORDINATES, double> CIRCLE;

class Panel
{
private:
	BOUNDS bounds;
	BOUNDS mPanelNameBounds;
	std::string panelName;
	std::string totalThickness;
	std::string riggingType;
	BOUNDS mInternalPanelBounds;
	BOUNDS mDetailLabelsBounds;
	double internalPanelYOffset;
	double panelFFYPosition;
	bool bHasInterferenceInInserts;
	COORDINATES mOrigin;
	CIRCLE m_CG;

	// bounds calculation functions
	void calculatePanelNameBounds();
	void calculateDetailLabelsBounds();

	// update functions
	void seperateFutureOpenings();
	void detectInterferenceCheck();
	void updateFFYPosition();
	void updateRiggingType();
	void filterCGLiftOpening();
	void createDimension(std::vector<COORDINATES>& singleLine,AcString layer, bool isHorizontal);

	//utility helpers
	void projectToPanel(double& xPos, double& yPos, bool isHorizontal);
	void projectToInserts(double& xPos, double& yPos, AcString& layer, bool isHorizontal);
	void projectToNearestVerticalInsert(double &x, double &y, std::vector<COORDINATES>& inserts);
	void projectToNearestHorizontalInsert(double &x, double &y, std::vector<COORDINATES>& inserts);
	void projectToNearestVerticalInsert(double& x, double& y, std::vector<CIRCLE>& inserts);
	void projectToNearestHorizontalInsert(double& x, double& y, std::vector<CIRCLE>& inserts);

public:
	std::list<BOUNDS> vecOpenings;
	std::vector<BOUNDS> vecFutureOpenings;
	std::vector<CIRCLE> vecLiftInserts;
	std::vector<COORDINATES> vecBraceInserts;
	std::vector<COORDINATES> vecFutureCoordinates;
	std::vector<LABELTEXT> vecRebarLabelsInsideInternalPanel;
	std::vector<LABELTEXT> vecRebarLabelsOutsideInternalPanel;
	std::vector<LABELTEXT> vecPanelLabels;
	std::vector<LABELTEXT> vecDetailLabels;
	std::vector< COORDINATES> vecInteralPanelLines;

	//dimensions reconnect
	std::vector<BOUNDS> vecOpeningDimHorPoints;
	std::vector<BOUNDS> vecOpeningDimVerPoints;
	std::vector<BOUNDS> vecBraceDimHorPoints;
	std::vector<BOUNDS> vecBraceDimVerPoints;
	std::vector<BOUNDS> vecLiftDimHorPoints;
	std::vector<BOUNDS> vecLiftDimVerPoints;
	std::vector<BOUNDS> vecPanelDimHorPoints;
	std::vector<BOUNDS> vecPanelDimVerPoints;

	std::vector<AcDbRotatedDimension*> vecLiftDimHorPointsAfterReconnect;
	std::vector<AcDbRotatedDimension*> vecLiftDimVerPointsAfterReconnect;

	explicit Panel(BOUNDS boundInfo);

	// Panel add methods
	void addLiftInsert(CIRCLE &liftInsert);
	void addBraceInsert(COORDINATES &braceInsert);
	void addOpening(BOUNDS& opening);
	void addFutureTextCoordinates(COORDINATES& futuretextposition);
	void addPanelLabels(LABELTEXT& panelLabels);
	void addDetailLabels(LABELTEXT& detailLabels);
	void addRebarLabels(LABELTEXT& panelLabels, bool inside);
	void addInternalPanelBounds(BOUNDS& internalPanelBounds);
	void addInternalPanelLines(COORDINATES& internalPanelLines);

	//Panel add dimension methods
	void addOpeningDimensions(BOUNDS& dimensions, bool isHorizontal);
	void addBraceDimensions(BOUNDS& dimensions, bool isHorizontal);
	void addLiftDimensions(BOUNDS& dimensions, bool isHorizontal);
	void addPanelDimensions(BOUNDS& dimensions, bool isHorizontal);

	void addLiftDimensionsAfterReconnect(AcDbRotatedDimension* dimensions, bool isHorizontal);

	//Utility functions
	bool isElementWithinPanel(COORDINATES& element);
	bool isElementWithinPanel(BOUNDS& element);
	bool isElementWithinPanel(CIRCLE& circle);
	bool isElementWithinPanel(AcDbRotatedDimension* dim);
	bool isInterferenceDetected();
	bool areRebarTextsMatching();
	std::string getOpeningType(BOUNDS& bound); // should be used only for openings

	//Get functions
	BOUNDS& getPanelNameBounds();
	BOUNDS& getDetailLableBounds();
	double getInternalPanelYOffset();
	std::string& getPanelName();
	BOUNDS getPanelBounds();
	std::string& getPanelThickNess();
	double getPanelWidth();
	double getPanelHeight();
	double getPanelHeightBelowFF();
	double getPanelHeightAboveFF();
	std::string getRiggingType();
	BOUNDS getInternalPanelBounds();
	CIRCLE GetCG();


	//set functions
	void setPanelName(std::string name);

	//panel update functions
	void updatePanel();
	void updatePanelThickness();

	//TODO : move dimensions logic into a different class
	//void create dimensions
	void createDimensions(std::vector<BOUNDS>& horizontalBounds, std::vector<BOUNDS>& verticalBounds, AcString layer);
	
};

