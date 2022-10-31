/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
*/

#include "DeadmanNotesHelper.h"

DeadmanNotesHelper::DeadmanNotesHelper(std::list<Panel>& vecPanels)
{
	this->m_PanelsList = vecPanels;
}

DeadmanNotesHelper::~DeadmanNotesHelper()
{
	this->m_PanelsList.clear();
}

void DeadmanNotesHelper::PlaceLabels()
{
	
	for (auto& panel : m_PanelsList)
	{
		AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
		AcDbBlockTableRecordPointer pBTR
		(
			acdbSymUtil()->blockModelSpaceId(pDb),
			AcDb::kForWrite
		);

		BOUNDS bPanelBounds = panel.getInternalPanelBounds();
		double dPanelXLow = bPanelBounds.first.first;
		double dPanelYHigh = bPanelBounds.second.second;

		// Condition- 1 For panels having both edge lift and face lift 
		if (panel.vecEdgeLifts.size() > 0 && panel.vecLiftInserts.size() > 1)
		{
 			AcDbMText* mtext = new AcDbMText;
			mtext->setLocation(AcGePoint3d((dPanelXLow - 122), (dPanelYHigh + 105), 0));
			mtext->setTextHeight(6.0f);
			mtext->setWidth(96);
			mtext->setContents(L"{\\LFOR PLUMBSET ONLY\\P\\lP92P 8T x 13.25' EDGE LIFT INSERT WITH #6 TENSION BAR. SEE GENERAL NOTES FOR CORECT TENSION BAR INSTALLATION }");

			pBTR->appendAcDbEntity(mtext);

			AcDbLeader* leader = new AcDbLeader;
			AcDbObjectId dimStyleId = pDb->dimstyle();

			leader->appendVertex(AcGePoint3d((dPanelXLow + 42), (dPanelYHigh), 0));
			leader->appendVertex(AcGePoint3d((dPanelXLow - 12), (dPanelYHigh + 100), 0));
			leader->appendVertex(AcGePoint3d((dPanelXLow - 25), (dPanelYHigh + 100), 0));

			leader->setAnnotationObjId(mtext->objectId());
			leader->setDimensionStyle(dimStyleId);
			pBTR->appendAcDbEntity(leader);

			leader->close();

			mtext->close();
		}
		
		
		

		// Condition 2 - Wall connection - T5A-HVY | Condition 3 - Wall connection - T6A
		if (panel.panelDetailsMap["BRACE WALL CONN."] == " T5A-HVY" || panel.panelDetailsMap["BRACE WALL CONN."] == " T6A" || panel.panelDetailsMap["BRACE WALL CONN."] == " T5A")
		{
			BOUNDS bPanelBounds = panel.getPanelNameBounds();
			double dDefPointsX = bPanelBounds.first.first - 745;
			double dDefPointsY = bPanelBounds.second.second + 820;

			AcDbLine* pLine1 = new AcDbLine();
			pLine1->setStartPoint(AcGePoint3d((dDefPointsX + 15) , (dDefPointsY - 15), 0));
			pLine1->setEndPoint(AcGePoint3d((dDefPointsX + 225), (dDefPointsY - 15), 0));
			pBTR->appendAcDbEntity(pLine1);
			pLine1->close();

			AcDbLine* pLine2 = new AcDbLine();
			pLine2->setStartPoint(AcGePoint3d((dDefPointsX + 225), (dDefPointsY - 15), 0));
			pLine2->setEndPoint(AcGePoint3d((dDefPointsX + 225), (dDefPointsY - 51), 0));
			pBTR->appendAcDbEntity(pLine2);
			pLine2->close();

			AcDbLine* pLine3 = new AcDbLine();
			pLine3->setStartPoint(AcGePoint3d((dDefPointsX + 225), (dDefPointsY - 51), 0));
			pLine3->setEndPoint(AcGePoint3d((dDefPointsX + 15), (dDefPointsY - 51), 0));
			pBTR->appendAcDbEntity(pLine3);
			pLine3->close();

			AcDbLine* pLine4 = new AcDbLine();
			pLine4->setStartPoint(AcGePoint3d((dDefPointsX + 15), (dDefPointsY - 15), 0));
			pLine4->setEndPoint(AcGePoint3d((dDefPointsX + 15), (dDefPointsY - 51), 0));
			pBTR->appendAcDbEntity(pLine4);
			pLine4->close();

			AcDbLine* pLine5 = new AcDbLine();
			pLine5->setStartPoint(AcGePoint3d((dDefPointsX + 15), (dDefPointsY - 33), 0));
			pLine5->setEndPoint(AcGePoint3d((dDefPointsX + 225), (dDefPointsY - 33), 0));
			pBTR->appendAcDbEntity(pLine5);
			pLine5->close();

			AcDbMText* mtext = new AcDbMText;
			mtext->setLocation(AcGePoint3d((dDefPointsX + 18), (dDefPointsY - 21), 0));
			mtext->setTextHeight(6.0f);
			mtext->setContents(L"{\\LPANEL SPECIFIC NOTES: }");

			pBTR->appendAcDbEntity(mtext);
			mtext->close();

			AcDbMText* mtext2 = new AcDbMText;
			mtext2->setLocation(AcGePoint3d((dDefPointsX + 18), (dDefPointsY - 38), 0));
			mtext2->setTextHeight(6.0f);
			if (panel.panelDetailsMap["BRACE WALL CONN."] == " T6A")
			{
				mtext2->setContents(L"{-ALL BRACE TO DEADMAN}");
			}
			else 
			{
				mtext2->setContents(L"{-ALL BRACE TO EXTERIOR DEADMAN}");
			}
			pBTR->appendAcDbEntity(mtext2);
			mtext2->close();
		}
		


		// Condition 4 - Distance between the panel top row lifts and top of the panel
		if (panel.vecLiftInserts.size() > 1)
		{
			bool bWedgeFreePanel = true;
			for (auto& liftInsert : panel.vecLiftInserts)
			{
				if (abs(dPanelYHigh - liftInsert.first.second) < 84)
				{
					bWedgeFreePanel = false;
					break;
				}
			}

			if (bWedgeFreePanel)
			{
				AcDbMText* mtext = new AcDbMText;
				mtext->setLocation(AcGePoint3d((dPanelXLow - 133), (dPanelYHigh + 42), 0));
				mtext->setTextHeight(6.0f);
				mtext->setContents(L"WEDGE PANEL FREE");

				pBTR->appendAcDbEntity(mtext);

				AcDbLeader* leader = new AcDbLeader;
				AcDbObjectId dimStyleId = pDb->dimstyle();

				leader->appendVertex(AcGePoint3d((dPanelXLow + 20), (dPanelYHigh), 0));
				leader->appendVertex(AcGePoint3d((dPanelXLow - 40), (dPanelYHigh + 38), 0));
				leader->appendVertex(AcGePoint3d((dPanelXLow - 45), (dPanelYHigh + 38), 0));

				leader->setAnnotationObjId(mtext->objectId());
				leader->setDimensionStyle(dimStyleId);
				pBTR->appendAcDbEntity(leader);

				leader->close();

				mtext->close();
			}
		}
		
		// Condition 5 - Depth less than 5" from opening bottom and Bottom of the panel
		if (panel.vecOpenings.size() > 0)
		{
			double dPanelLowY = bPanelBounds.first.second;
			for (auto& opening : panel.vecOpenings)
			{
				if ((abs(opening.first.second - dPanelLowY) <= 5) && (abs(opening.first.second - dPanelLowY) > 0))
				{
					AcDbMText* mtext = new AcDbMText;
					mtext->setLocation(AcGePoint3d((dPanelXLow + 230), (dPanelLowY - 80), 0));
					mtext->setTextHeight(6.0f);
					mtext->setWidth(135);
					mtext->setContents(L"\\LNOTE:\\P\\lDAYTON SUPERIOR CORP. IS NOT RESPONSIBLE FOR ANY CRACKING ADJACENT OR TO THE OPENING AS SHOWN(TYP.)");

					pBTR->appendAcDbEntity(mtext);

					AcDbLeader* leader = new AcDbLeader;
					AcDbObjectId dimStyleId = pDb->dimstyle();

					leader->appendVertex(AcGePoint3d((dPanelXLow + 150), (dPanelLowY), 0));
					leader->appendVertex(AcGePoint3d((dPanelXLow + 220), (dPanelLowY - 100), 0));
					leader->appendVertex(AcGePoint3d((dPanelXLow + 225), (dPanelLowY - 100), 0));

					leader->setAnnotationObjId(mtext->objectId());
					leader->setDimensionStyle(dimStyleId);
	

					pBTR->appendAcDbEntity(leader);

					leader->close();

					mtext->close();
				}
			}
		}
		pBTR->close();
	}


	return;
}
