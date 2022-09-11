#include "PanelBuilder.h"

PanelBuilder::PanelBuilder(Panel& panel)
{
	ptrPanel = &panel;
}

void PanelBuilder::buildLiftInserts(std::list<CIRCLE>& liftInserts)
{
	for (auto& liftInsert : liftInserts)
	{
		if (ptrPanel->isElementWithinPanel(liftInsert))
			ptrPanel->addLiftInsert(liftInsert);
	}
}

void PanelBuilder::buildBraceInserts(std::list<COORDINATES>& braceInserts)
{
	for (auto& braceInsert : braceInserts)
	{
		if (ptrPanel->isElementWithinPanel(braceInsert))
			ptrPanel->addBraceInsert(braceInsert);
	}
}

void PanelBuilder::buildOpenings(std::list<BOUNDS>& openings, std::list<COORDINATES> futureTextCordinates)
{
	// collect all openings within a panel
	for (auto& opening : openings)
	{
		if (ptrPanel->isElementWithinPanel(opening))
			ptrPanel->addOpening(opening);
	}

	// collect future openings
	for (auto& futureOpening : futureTextCordinates)
	{
		if (ptrPanel->isElementWithinPanel(futureOpening))
			ptrPanel->addFutureTextCoordinates(futureOpening);
	}
}

void PanelBuilder::buildLabels(std::list<LABELTEXT> textLabels)
{
	// collect panel Name
	for (auto& textLabel : textLabels)
	{
		if (Utilities::getUtils()->boundCheck(ptrPanel->getPanelNameBounds(), textLabel.second))
			ptrPanel->setPanelName(textLabel.first);

		// collect rebar labels
		if (ptrPanel->isElementWithinPanel(textLabel.second))
		{
			if (textLabel.first.length() > 0 && textLabel.first[0] == '(')
			{
				if (textLabel.second.second <= ptrPanel->getInternalPanelYOffset())
					ptrPanel->addRebarLabels(textLabel, true); // collect rebar labels inside panel
				else
					ptrPanel->addRebarLabels(textLabel, false); // collect rebar labels outside panel
			}
			else
			{
				ptrPanel->addPanelLabels(textLabel);
			}
		}
		// collect detail labels
		else if (Utilities::getUtils()->boundCheck(ptrPanel->getDetailLableBounds(), textLabel.second))
		{
			ptrPanel->addDetailLabels(textLabel);
		}
	}
	ptrPanel->updatePanelThickness();
}

void PanelBuilder::buildInternalPanel(std::list<BOUNDS> internalPanels, std::list<COORDINATES> internalPanelLines)
{
	// collect internal panel Bounds
	for (auto& internalPanelBound : internalPanels)
	{
		if (ptrPanel->isElementWithinPanel(internalPanelBound))
			ptrPanel->addInternalPanelBounds(internalPanelBound);
	}
	//collect internal panel Lines
	for (auto& internalPanelLine : internalPanelLines)
	{
		if (ptrPanel->isElementWithinPanel(internalPanelLine))
			ptrPanel->addInternalPanelLines(internalPanelLine);
	}
}

void PanelBuilder::updatePanel()
{
	ptrPanel->updatePanel();
}

void PanelBuilder::buildOpeningDimensions(std::list<BOUNDS> horizantalBounds, std::list<BOUNDS> verticalBounds)
{
	// collect horizontal bounds
	for (auto& horizontalBound : horizantalBounds)
	{
		if (ptrPanel->isElementWithinPanel(horizontalBound))
			ptrPanel->addOpeningDimensions(horizontalBound, true);
	}
	// collect vertical bounds
	for (auto& verticalBound : verticalBounds)
	{
		if (ptrPanel->isElementWithinPanel(verticalBound))
			ptrPanel->addOpeningDimensions(verticalBound, false);
	}
}

void PanelBuilder::buildBraceDimensions(std::list<BOUNDS> horizantalBounds, std::list<BOUNDS> verticalBounds)
{
	// collect horizontal bounds
	for (auto& horizontalBound : horizantalBounds)
	{
		if (ptrPanel->isElementWithinPanel(horizontalBound))
			ptrPanel->addBraceDimensions(horizontalBound, true);
	}
	// collect vertical bounds
	for (auto& verticalBound : verticalBounds)
	{
		if (ptrPanel->isElementWithinPanel(verticalBound))
			ptrPanel->addBraceDimensions(verticalBound, false);
	}
}

void PanelBuilder::buildLiftDimensions(std::list<BOUNDS> horizantalBounds, std::list<BOUNDS> verticalBounds)
{
	// collect horizontal bounds
	for (auto& horizontalBound : horizantalBounds)
	{
		if (ptrPanel->isElementWithinPanel(horizontalBound))
			ptrPanel->addLiftDimensions(horizontalBound, true);
	}
	// collect vertical bounds
	for (auto& verticalBound : verticalBounds)
	{
		if (ptrPanel->isElementWithinPanel(verticalBound))
			ptrPanel->addLiftDimensions(verticalBound, false);
	}
}

void PanelBuilder::buildPanelDimensions(std::list<BOUNDS> horizantalBounds, std::list<BOUNDS> verticalBounds)
{// collect horizontal bounds
	for (auto& horizontalBound : horizantalBounds)
	{
		if (ptrPanel->isElementWithinPanel(horizontalBound))
			ptrPanel->addPanelDimensions(horizontalBound, true);
	}
	// collect vertical bounds
	for (auto& verticalBound : verticalBounds)
	{
		if (ptrPanel->isElementWithinPanel(verticalBound))
			ptrPanel->addPanelDimensions(verticalBound, false);
	}
}
