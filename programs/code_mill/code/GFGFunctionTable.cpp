/******************************************************************************
 GFGFunctionTable.cpp

	<Description>

	BASE CLASS = public JXTable

	Copyright (C) 2001 by Glenn W. Bach.

 *****************************************************************************/

#include <GFGFunctionTable.h>
#include "GFGMemberFunction.h"
#include "GFGClass.h"

#include <gfgGlobals.h>

#include <JXColHeaderWidget.h>
#include <JXColorManager.h>

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
	auto* table =
		jnew GFGFunctionTable(list, scrollbarSet, enclosure,
			hSizing, vSizing, x,y, w,h);
	assert(table != nullptr);

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
	itsNeedsAdjustment	= true;
	
	SetRowBorderInfo(0, JColorManager::GetBlackColor());
	SetColBorderInfo(0, JColorManager::GetBlackColor());
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
	if (sender == itsList && message.Is(JListT::kElementsInserted))
		{
		const JSize delta = itsList->GetElementCount() - GetRowCount();
		if (delta != 0)
			{
			AppendCols(delta);
			itsNeedsAdjustment	= true;
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
		GFGMemberFunction* fn	= itsList->GetElement(cell.y);
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
		p.SetPenColor(JColorManager::GetGrayColor(95));
		p.SetFilling(true);
		p.Rect(rect);
		p.SetFilling(false);
		}

	const GFGMemberFunction* fn	= itsList->GetElement(cell.y);

	if (cell.x == kFUsed)
		{
		if (fn->IsUsed())
			{
			JRect r(rect.ycenter(), rect.xcenter(), rect.ycenter(), rect.xcenter());
			r.Expand(kBulletRadius, kBulletRadius);
			p.SetPenColor(JColorManager::GetBlackColor());
			p.SetFilling(true);
			p.Ellipse(r);
			}
		return;
		}

	JFontStyle style;
	JPainter::HAlignment halign	= JPainter::kHAlignLeft;

	if (fn->IsRequired())
		{
		style.italic = true;
		}

	if (fn->IsProtected())
		{
		style.color = JColorManager::GetBrownColor();
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

	JFont font = JFontManager::GetDefaultMonospaceFont();
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
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == ' ')
		{
		GetTableSelection().ClearSelection();
		}
	else
		{
		JXTable::HandleKeyPress(c, keySym, modifiers);
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
	itsNeedsAdjustment = false;

	JFontManager* fontMgr = GetFontManager();
	const JFont& font     = JFontManager::GetDefaultMonospaceFont();

	const JSize count	= itsList->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		const GFGMemberFunction* fn	= itsList->GetElement(i);
		JSize width	= font.GetStringWidth(fontMgr, fn->GetReturnType());
		JCoordinate adjWidth = width + 2 * kHMarginWidth;
		if (adjWidth > GetColWidth(kFReturnType))
			{
			SetColWidth(kFReturnType, adjWidth);
			}
			
		width    = font.GetStringWidth(fontMgr, fn->GetFnName());
		adjWidth = width + 2 * kHMarginWidth;
		if (adjWidth > GetColWidth(kFFunctionName))
			{
			SetColWidth(kFFunctionName, adjWidth);
			}

		width    = font.GetStringWidth(fontMgr, fn->GetArgString());
		adjWidth = width + 2 * kHMarginWidth;
		if (adjWidth > GetColWidth(kFArgs))
			{
			SetColWidth(kFArgs, adjWidth);
			}
		}
}
