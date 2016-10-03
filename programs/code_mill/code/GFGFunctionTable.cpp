/******************************************************************************
 GFGFunctionTable.cpp

	<Description>

	BASE CLASS = public JXTable

	Copyright (C) 2001 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GFGFunctionTable.h>
#include "GFGMemberFunction.h"
#include "GFGClass.h"

#include <gfgGlobals.h>

#include <JXColHeaderWidget.h>
#include <JXColormap.h>

#include <JFontManager.h>
#include <JPainter.h>
#include <JSimpleProcess.h>
#include <JTableSelection.h>

#include <signal.h>
#include <jAssert.h>

const JCoordinate kDefColWidth   = 100;
const JCoordinate kUsedColWidth  = 60;
const JCoordinate kConstColWidth = 50;
const JCoordinate kDefRowHeight  = 20;
const JCoordinate kHMarginWidth  = 5;
const JCoordinate kBulletRadius  = 4;

/******************************************************************************
 Create (static public)

 ******************************************************************************/

GFGFunctionTable*
GFGFunctionTable::Create
	(
	GFGClass* 			list,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	GFGFunctionTable* table =
		new GFGFunctionTable(list, scrollbarSet, enclosure,
			hSizing, vSizing, x,y, w,h);
	assert(table != NULL);

	table->GFGFunctionTableX();

	return table;
}

/******************************************************************************
 Constructor (protected)

 *****************************************************************************/

GFGFunctionTable::GFGFunctionTable
	(
	GFGClass* 			list,
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
	JXTable(kDefRowHeight, kDefColWidth, scrollbarSet,
			enclosure, hSizing, vSizing, x,y, w,h),
	itsList(list)
{
	ListenTo(itsList);
}

void
GFGFunctionTable::GFGFunctionTableX()
{
	itsNeedsAdjustment	= kJTrue;
	
	SetRowBorderInfo(0, GetColormap()->GetBlackColor());
	SetColBorderInfo(0, GetColormap()->GetBlackColor());
	AppendCols(kFArgs);
	AppendRows(itsList->GetElementCount());
	
	SetColWidth(kFUsed, kUsedColWidth);
	SetColWidth(kFConst, kConstColWidth);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GFGFunctionTable::~GFGFunctionTable()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GFGFunctionTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsList && message.Is(JOrderedSetT::kElementsInserted))
		{
		const JSize delta = itsList->GetElementCount() - GetRowCount();
		if (delta != 0)
			{
			AppendCols(delta);
			itsNeedsAdjustment	= kJTrue;
			}
		}
	else
		{
		JXTable::Receive(sender, message);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
GFGFunctionTable::HandleMouseDown
	(
	const JPoint& pt,
	const JXMouseButton button,
	const JSize clickCount,
	const JXButtonStates& buttonStates,
	const JXKeyModifiers& modifiers
	)
{
	if (ScrollForWheel(button, modifiers))
		{
		return;
		}

	JPoint cell;
	if (GetCell(pt, &cell))
		{
		GFGMemberFunction* fn	= itsList->NthElement(cell.y);
		fn->ShouldBeUsed(!fn->IsUsed());
		TableRefresh();
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GFGFunctionTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	AdjustColumnWidths();
	
	if (JRound(cell.y/2)*2 != cell.y)
		{
		p.SetPenColor(GetColormap()->GetGrayColor(95));
		p.SetFilling(kJTrue);
		p.Rect(rect);
		p.SetFilling(kJFalse);
		}

	const GFGMemberFunction* fn	= itsList->NthElement(cell.y);

	if (cell.x == kFUsed)
		{
		if (fn->IsUsed())
			{
			JRect r(rect.ycenter(), rect.xcenter(), rect.ycenter(), rect.xcenter());
			r.Expand(kBulletRadius, kBulletRadius);
			p.SetPenColor((p.GetColormap())->GetBlackColor());
			p.SetFilling(kJTrue);
			p.Ellipse(r);
			}
		return;
		}

	JFontStyle style;
	JPainter::HAlignment halign	= JPainter::kHAlignLeft;

	if (fn->IsRequired())
		{
		style.italic = kJTrue;
		}

	if (fn->IsProtected())
		{
		JColorIndex color	= GetColormap()->GetBrownColor();
		style.color			= color;
		}

	JString str;
	if (cell.x == kFReturnType)
		{
		str	= fn->GetReturnType();
		}
	else if (cell.x == kFFunctionName)
		{
		str	= fn->GetFnName();
		}
	else if (cell.x == kFConst)
		{
		if (fn->IsConst())
			{
			str	= "const";
			halign	= JPainter::kHAlignCenter;
			}
		}
	else if (cell.x == kFArgs)
		{
		str = fn->GetArgString();
		}

	JRect r = rect;
	r.left  += kHMarginWidth;
	r.right -= kHMarginWidth;

	JFont font = GetFontManager()->GetDefaultMonospaceFont();
	font.SetStyle(style);
	p.SetFont(font);
	p.JPainter::String(r, str, halign, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleKeyPress (virtual protected)

 ******************************************************************************/

void
GFGFunctionTable::HandleKeyPress
	(
	const int key,
	const JXKeyModifiers& modifiers
	)
{
	if (key == ' ')
		{
		GetTableSelection().ClearSelection();
		}
	else
		{
		JXTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 AdjustColumnWidths (private)

 ******************************************************************************/

void
GFGFunctionTable::AdjustColumnWidths()
{
	if (!itsNeedsAdjustment)
		{
		return;
		}
	itsNeedsAdjustment = kJFalse;

	const JFont& font = GetFontManager()->GetDefaultMonospaceFont();

	const JSize count	= itsList->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		const GFGMemberFunction* fn	= itsList->NthElement(i);
		JSize width	= font.GetStringWidth(fn->GetReturnType());
		JCoordinate adjWidth = width + 2 * kHMarginWidth;
		if (adjWidth > GetColWidth(kFReturnType))
			{
			SetColWidth(kFReturnType, adjWidth);
			}
			
		width    = font.GetStringWidth(fn->GetFnName());
		adjWidth = width + 2 * kHMarginWidth;
		if (adjWidth > GetColWidth(kFFunctionName))
			{
			SetColWidth(kFFunctionName, adjWidth);
			}

		width    = font.GetStringWidth(fn->GetArgString());
		adjWidth = width + 2 * kHMarginWidth;
		if (adjWidth > GetColWidth(kFArgs))
			{
			SetColWidth(kFArgs, adjWidth);
			}
		}
}
