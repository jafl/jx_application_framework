/******************************************************************************
 GLCurveNameList.cpp

	BASE CLASS = public JXEditTable

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GLCurveNameList.h>
#include "PlotDir.h"
#include <J2DPlotWidget.h>

#include <JXColormap.h>
#include <JXInputField.h>

#include <JFontManager.h>
#include <JPainter.h>
#include <JTableSelection.h>

#include <jGlobals.h>
#include <jASCIIConstants.h>

#include <JMinMax.h>
#include <jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;
const JCoordinate kDefColWidth  = 100;

const JCharacter* GLCurveNameList::kCurveSelected = "kCurveSelected::GLCurveNameList";

/******************************************************************************
 Constructor

 *****************************************************************************/

GLCurveNameList::GLCurveNameList
	(
	PlotDir*			dir,
	J2DPlotWidget* 		plot,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXEditTable(1, kDefColWidth, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsInput(NULL),
	itsPlot(plot),
	itsDir(dir)
{
	itsMinColWidth = 1;

	const JFontManager* fontMgr = GetFontManager();
	const JSize rowHeight = 2*kVMarginWidth + fontMgr->GetDefaultFont().GetLineHeight();
	SetDefaultRowHeight(rowHeight);

	const JSize count = plot->GetCurveCount();

	itsNameList = new JPtrArray<JString>(JPtrArrayT::kDeleteAll, count);
	assert(itsNameList != NULL);

	AppendRows(count);
	for (JIndex i=1; i<=count; i++)
		{
		JString* str	= new JString(plot->GetCurveName(i));
		assert(str != NULL);
		itsNameList->Append(str);

		const JCoordinate width = 2*kHMarginWidth +
			fontMgr->GetDefaultFont().GetStringWidth(*str);
		if (width > itsMinColWidth)
			{
			itsMinColWidth = width;
			}
		}

	AppendCols(1);
	AdjustColWidth();

	JXColormap* colormap = GetColormap();
	SetRowBorderInfo(0, colormap->GetBlackColor());
	SetColBorderInfo(0, colormap->GetBlackColor());

	ListenTo(itsPlot);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLCurveNameList::~GLCurveNameList()
{
	itsNameList->DeleteAll();
	delete itsNameList;		// we don't own the strings
}

/******************************************************************************
 HandleMouseDown (protected)

 ******************************************************************************/

void
GLCurveNameList::HandleMouseDown
	(
	const JPoint& 			pt,
	const JXMouseButton 	button,
	const JSize 			clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button == kJXLeftButton && clickCount == 1 &&
		GetCell(pt, &cell) && !itsDir->CurveIsFit(cell.y))
		{
		if (!GetTableSelection().IsSelected(cell))
			{
			SelectSingleCell(cell);
			Broadcast(CurveSelected(cell.y));
			}
		TableRefresh();
		}
	else
		{
		ScrollForWheel(button, modifiers);
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GLCurveNameList::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	HilightIfSelected(p, cell, rect);

	const JString* curveName = itsNameList->NthElement(cell.y);

	JRect r = rect;
	r.left += kHMarginWidth;
	JColorIndex color	= GetColormap()->GetBlackColor();
	if (itsDir->CurveIsFit(cell.y))
		{
		color	= GetColormap()->GetGrayColor(60);
		}
	p.SetFontStyle(JFontStyle(kJFalse, kJFalse, 0, kJFalse, color));
	p.String(r, *curveName, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
GLCurveNameList::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXEditTable::ApertureResized(dw,dh);
	AdjustColWidth();
}

/******************************************************************************
 AdjustColWidth (private)

 ******************************************************************************/

void
GLCurveNameList::AdjustColWidth()
{
	SetColWidth(1, JMax(itsMinColWidth, GetApertureWidth()));
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
GLCurveNameList::CreateXInputField
	(
	const JPoint& 		cell,
	const JCoordinate 	x,
	const JCoordinate 	y,
	const JCoordinate 	w,
	const JCoordinate 	h
	)
{
	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectRow(cell.y);
	Broadcast(CurveSelected(cell.y));

	assert(itsInput == NULL);
	itsInput = new JXInputField(this, kFixedLeft, kFixedTop, x, y, w, h);
	assert(itsInput != NULL);

	itsInput->SetText(*(itsNameList->NthElement(cell.y)));
	itsInput->SetIsRequired();
	return itsInput;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
GLCurveNameList::PrepareDeleteXInputField()
{
	itsInput = NULL;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
GLCurveNameList::ExtractInputData
	(
	const JPoint& cell
	)
{
	const JString& name = itsInput->GetText();
	if (!name.IsEmpty())
		{
		*(itsNameList->NthElement(cell.y)) = name;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 HandleKeyPress (virtual public)

 ******************************************************************************/

void
GLCurveNameList::HandleKeyPress
	(
	const int 				key,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	JTableSelection& s = GetTableSelection();
	const JBoolean ok  = s.GetFirstSelectedCell(&cell);
	assert( ok );

	if (key == kJUpArrow)
		{
		cell.y--;
		if (CellValid(cell))
			{
			BeginEditing(cell);
			}
		}
	else if (key == kJDownArrow)
		{
		cell.y++;
		if (CellValid(cell))
			{
			BeginEditing(cell);
			}
		}

	else
		{
		JXEditTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GLCurveNameList::Receive
	(
	JBroadcaster* 	sender, 
	const Message&	message
	)
{
	if (sender == itsPlot && message.Is(J2DPlotWidget::kCurveAdded))
		{
		const J2DPlotWidget::CurveAdded* info = 
			dynamic_cast<const J2DPlotWidget::CurveAdded*>(&message);
		assert(info != NULL);
		AppendRows(1);
		JString* str	= new JString(itsPlot->GetCurveName(info->GetIndex()));
		assert(str != NULL);
		itsNameList->Append(str);

		const JCoordinate width = 2*kHMarginWidth +
			GetFontManager()->GetDefaultFont().GetStringWidth(*str);
		if (width > itsMinColWidth)
			{
			itsMinColWidth = width;
			}
		AdjustColWidth();
		TableRefresh();
		}
	else if (sender == itsPlot && message.Is(J2DPlotWidget::kCurveRemoved))
		{
		const J2DPlotWidget::CurveRemoved* info = 
			dynamic_cast<const J2DPlotWidget::CurveRemoved*>(&message);
		assert(info != NULL);
		RemoveRow(info->GetIndex());
		itsNameList->DeleteElement(info->GetIndex());
		TableRefresh();
		}
}

/******************************************************************************
 GetCurrentCurveIndex (public)

 ******************************************************************************/

JBoolean
GLCurveNameList::GetCurrentCurveIndex
	(
	JIndex* index
	)
{
	JPoint cell;
	if (GetTableSelection().GetSingleSelectedCell(&cell))
		{
		*index	= cell.y;
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 SetCurrentCurveIndex (public)

 ******************************************************************************/

void
GLCurveNameList::SetCurrentCurveIndex
	(
	const JIndex index
	)
{
	if (GetRowCount() > 0)
		{
		JPoint cell(1, 1);
		SelectSingleCell(cell);
		Broadcast(CurveSelected(cell.y));
		}
}
