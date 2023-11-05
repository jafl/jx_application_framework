/******************************************************************************
 JX2DCurveNameList.cpp

	BASE CLASS = public JXEditTable

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#include "JX2DCurveNameList.h"
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;
const JCoordinate kDefColWidth  = 100;

const JUtf8Byte* JX2DCurveNameList::kNameSelected = "JX2DCurveNameList::kNameSelected";

/******************************************************************************
 Constructor

 *****************************************************************************/

JX2DCurveNameList::JX2DCurveNameList
	(
	const JArray<J2DCurveInfo>&	curveInfo,
	const JIndex				startIndex,
	JXScrollbarSet*				scrollbarSet,
	JXContainer*				enclosure,
	const HSizingOption			hSizing,
	const VSizingOption			vSizing,
	const JCoordinate			x,
	const JCoordinate			y,
	const JCoordinate			w,
	const JCoordinate			h
	)
	:
	JXEditTable(1, kDefColWidth, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsInput(nullptr)
{
	itsMinColWidth = 1;

	const JSize rowHeight = 2*kVMarginWidth +
		JFontManager::GetDefaultFont().GetLineHeight(GetFontManager());
	SetDefaultRowHeight(rowHeight);

	const JSize count = curveInfo.GetElementCount();

	itsNameList = jnew JPtrArray<JString>(JPtrArrayT::kForgetAll, count);
	assert(itsNameList != nullptr);

	AppendRows(count);
	for (JIndex i=1; i<=count; i++)
	{
		const J2DCurveInfo info = curveInfo.GetElement(i);
		itsNameList->Append(info.name);

		const JCoordinate width = 2*kHMarginWidth +
			JFontManager::GetDefaultFont().GetStringWidth(GetFontManager(), *(info.name));
		if (width > itsMinColWidth)
		{
			itsMinColWidth = width;
		}
	}

	AppendCols(1);
	AdjustColWidth();

	SetRowBorderInfo(0, JColorManager::GetBlackColor());
	SetColBorderInfo(0, JColorManager::GetBlackColor());

	BeginEditing(JPoint(1, startIndex));
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JX2DCurveNameList::~JX2DCurveNameList()
{
	jdelete itsNameList;		// we don't own the strings
}

/******************************************************************************
 HandleMouseDown (protected)

 ******************************************************************************/

void
JX2DCurveNameList::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton	button,
	const JSize			clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button == kJXLeftButton && clickCount == 1 &&
		GetCell(pt, &cell))
	{
		BeginEditing(cell);
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
JX2DCurveNameList::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	HilightIfSelected(p, cell, rect);

	const JString* curveName = itsNameList->GetElement(cell.y);

	JRect r = rect;
	r.left += kHMarginWidth;
	p.String(r, *curveName, JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JX2DCurveNameList::ApertureResized
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
JX2DCurveNameList::AdjustColWidth()
{
	SetColWidth(1, JMax(itsMinColWidth, GetApertureWidth()));
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
JX2DCurveNameList::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectRow(cell.y);
	Broadcast(NameSelected(cell.y));

	assert(itsInput == nullptr);
	itsInput = jnew JXInputField(this, kHElastic, kVElastic, x, y, w, h);

	itsInput->GetText()->SetText(*(itsNameList->GetElement(cell.y)));
	itsInput->SetIsRequired();
	return itsInput;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
JX2DCurveNameList::PrepareDeleteXInputField()
{
	itsInput = nullptr;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

bool
JX2DCurveNameList::ExtractInputData
	(
	const JPoint& cell
	)
{
	const JString& name = itsInput->GetText()->GetText();
	if (!name.IsEmpty())
	{
		*(itsNameList->GetElement(cell.y)) = name;
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 HandleKeyPress (virtual public)

 ******************************************************************************/

void
JX2DCurveNameList::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	JTableSelection& s = GetTableSelection();
	const bool ok  = s.GetFirstSelectedCell(&cell);
	assert( ok );

	if (c == kJUpArrow)
	{
		cell.y--;
		if (CellValid(cell))
		{
			BeginEditing(cell);
		}
	}
	else if (c == kJDownArrow)
	{
		cell.y++;
		if (CellValid(cell))
		{
			BeginEditing(cell);
		}
	}

	else
	{
		JXEditTable::HandleKeyPress(c, keySym, modifiers);
	}
}
