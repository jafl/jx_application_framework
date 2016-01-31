/******************************************************************************
 CMLineNumberTable.cpp

	BASE CLASS = JXTable

	Copyright © 2001 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "CMLineNumberTable.h"
#include "CMSourceDirector.h"
#include "CMSourceText.h"
#include "CMCommandDirector.h"
#include "CMBreakpointsDir.h"
#include "CMBreakpointTable.h"
#include "CMBreakpointManager.h"
#include "CMDeselectLineNumberTask.h"
#include "cmGlobals.h"
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXFontManager.h>
#include <JXColormap.h>
#include <JTableSelection.h>
#include <JPainter.h>
#include <JColormap.h>
#include <jDirUtil.h>
#include <JOrderedSetUtil.h>
#include <jAssert.h>

enum
{
	kBreakpointColumn = 1,
	kExecPointColumn,
	kLineNumberColumn
};

const JCoordinate kMarginWidth = 2;

// Line menu

static const JCharacter* kLineMenuStr =
	"    Run until execution reaches this line %k Meta-click"
	"  | Set execution point to this line      %k Ctrl-click"
//	"  | Disassemble function containing this line"
	"%l";

enum
{
	kRunUntilCmd = 1,
	kSetExecPtCmd,		// = kLineMenuItemCount
//	kDisasmCmd
};

const JSize kLineMenuItemCount = kSetExecPtCmd;

static const JCharacter* kAllBreakpointsMenuStr =
	"    Set new breakpoint on this line       %k Click (if none)"
	"  | Set temporary breakpoint on this line %k Meta-Shift-click (if none)"
	"  | Remove all breakpoints from this line %k Click (if only one)"
	"%l";

enum
{
	kSetBreakpointCmd = 1,
	kSetTempBreakpointCmd,
	kRemoveAllBreakpointsCmd	// = kAllBreakpointsMenuItemCount
};

const JSize kAllBreakpointsMenuItemCount = kRemoveAllBreakpointsCmd;

static const JCharacter* kBreakpointMenuStr =
	"    Show information on breakpoint #$index"
	"  | Remove breakpoint"
	"  | Set condition"
	"  | Remove condition"
	"  | Enabled %b %k Shift-click (if only one)"
	"  | Ignore the next N times (currently, N=$ignore_count)"
	"%l";

enum
{
	kShowBreakpointInfoCmd = 1,
	kRemoveBreakpointCmd,
	kSetConditionCmd,
	kRemoveConditionCmd,
	kToggleEnableCmd,
	kIgnoreNextNCmd		// = kBreakpointMenuItemCount
};

const JSize kBreakpointMenuItemCount = kIgnoreNextNCmd;

/******************************************************************************
 Constructor

 ******************************************************************************/

CMLineNumberTable::CMLineNumberTable
	(
	CMSourceDirector*	dir,
	CMSourceText*		text,
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
	JXTable(1, 1, NULL, enclosure, hSizing, vSizing, x, y, w, h),
	itsDirector(dir),
	itsText(text),
	itsVScrollbar(scrollbarSet->GetVScrollbar()),
	itsCurrentVisualIndex(0),
	itsLineNumberList(NULL),
	itsLineMenu(NULL),
	itsDeselectTask(NULL)
{
	itsLink = CMGetLink();
	ListenTo(itsLink);

	itsBPList = new JPtrArray<CMBreakpoint>(JPtrArrayT::kForgetAll);
	assert(itsBPList != NULL);
	itsBPList->SetCompareFunction(CompareBreakpointLines);
	itsBPList->SetSortOrder(JOrderedSetT::kSortAscending);

	itsVisualBPIndexList = new JArray<JIndex>;
	assert( itsVisualBPIndexList != NULL );

	WantInput(kJFalse);
	SetBackColor((CMGetPrefsManager())->GetColor(CMPrefsManager::kBackColorIndex));
	SetFocusColor(GetBackColor());
	SetRowBorderInfo(0, GetBackColor());
	SetColBorderInfo(0, GetBackColor());
	SetDrawOrder(kDrawByCol);

	AppendCols(3);
	AdjustToText();

	ListenTo(itsText);
	ListenTo(itsVScrollbar);
	ListenTo(itsLink->GetBreakpointManager());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMLineNumberTable::~CMLineNumberTable()
{
	delete itsBPList;
	delete itsVisualBPIndexList;
	delete itsLineNumberList;
	delete itsDeselectTask;
}

/******************************************************************************
 ClearLineNumbers

 ******************************************************************************/

void
CMLineNumberTable::ClearLineNumbers()
{
	delete itsLineNumberList;
	itsLineNumberList = NULL;
}

/******************************************************************************
 SetLineNumbers

	We take ownership of the items in the list.

 ******************************************************************************/

void
CMLineNumberTable::SetLineNumbers
	(
	JPtrArray<JString>*	list
	)
{
	if (itsLineNumberList == NULL)
		{
		itsLineNumberList = new JPtrArray<JString>(*list, JPtrArrayT::kDeleteAll);
		assert( itsLineNumberList != NULL );
		}
	else
		{
		itsLineNumberList->CopyPointers(*list, JPtrArrayT::kDeleteAll, kJFalse);
		}
	list->SetCleanUpAction(JPtrArrayT::kForgetAll);

	itsLineNumberStartIndex = 1;
	if (!itsLineNumberList->IsEmpty())
		{
		const JString* s = itsLineNumberList->LastElement();
		if (s->GetLength() == (itsLineNumberList->FirstElement())->GetLength() &&
			s->BeginsWith("0x") && s->GetLength() > 2)
			{
			itsLineNumberStartIndex = 3;
			while (itsLineNumberStartIndex < s->GetLength() &&
				   s->GetCharacter(itsLineNumberStartIndex) == '0')
				{
				itsLineNumberStartIndex++;
				}
			}
		}
}

/******************************************************************************
 FindLineNumber

 ******************************************************************************/

JBoolean
CMLineNumberTable::FindLineNumber
	(
	const JCharacter*	origLineNumber,
	JIndex*				index
	)
	const
{
	if (itsLineNumberList != NULL)
		{
		const JString lineNumber(origLineNumber);

		const JSize count = itsLineNumberList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JString* addr = itsLineNumberList->NthElement(i);
			JIndex j;
			if (JStringCompare(lineNumber, *addr, kJFalse) == 0 ||
				(addr->BeginsWith(lineNumber, kJFalse) &&
				 isspace(addr->GetCharacter(lineNumber.GetLength()+1))))
				{
				*index = i;
				return kJTrue;
				}
			}
		}

	*index = 0;
	return kJFalse;
}

/******************************************************************************
 SetCurrentLine

 ******************************************************************************/

void
CMLineNumberTable::SetCurrentLine
	(
	const JIndex line
	)
{
	itsCurrentVisualIndex = line;
	if (0 < line && line <= itsText->GetLineCount())
		{
		itsText->SetCaretLocation(itsText->GetLineStart(line));
		}
	Refresh();
}

/******************************************************************************
 TablePrepareToDrawCol (virtual protected)

 ******************************************************************************/

void
CMLineNumberTable::TablePrepareToDrawCol
	(
	const JIndex colIndex,
	const JIndex firstRow,
	const JIndex lastRow
	)
{
	itsBPDrawIndex = 1;
	FindNextBreakpoint(firstRow);
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
CMLineNumberTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	HilightIfSelected(p, cell, rect);

	if (JIndex(cell.x) == kBreakpointColumn)
		{
		DrawBreakpoints(p, cell, rect);
		}

	else if (JIndex(cell.x) == kExecPointColumn && cell.y == itsCurrentVisualIndex)
		{
		// We can't use a static polygon because line heights can vary,
		// e.g. due to underlines.

		const JCoordinate delta = rect.height()/2;

		JPolygon poly;
		poly.AppendElement(JPoint(rect.left+kMarginWidth,       rect.top));
		poly.AppendElement(JPoint(rect.left+kMarginWidth+delta, rect.top + delta));
		poly.AppendElement(JPoint(rect.left+kMarginWidth,       rect.top + 2*delta));

		if (itsLineNumberList != NULL)
			{
			p.SetPenColor((GetColormap())->GetMagentaColor());
			}
		else
			{
			p.SetPenColor((GetColormap())->GetCyanColor());
			}
		p.SetFilling(kJTrue);
		p.Polygon(poly);

		p.SetPenColor((GetColormap())->GetBlackColor());
		p.SetFilling(kJFalse);
		p.Polygon(poly);
		}

	else if (JIndex(cell.x) == kLineNumberColumn)
		{
		JFontID id;
		JSize size;
		JFontStyle style;
		itsText->GetDefaultFont(&id, &size, &style);
		p.SetFont(id, size, style);

		JString str;
		if (itsLineNumberList != NULL && itsLineNumberList->IndexValid(cell.y))
			{
			const JString* s = itsLineNumberList->NthElement(cell.y);
			str              = s->GetSubstring(itsLineNumberStartIndex, s->GetLength());
			}
		else
			{
			str = JString(cell.y, JString::kBase10);
			}
		JRect r  = rect;
		r.right -= kMarginWidth;
		p.String(r, str, JPainter::kHAlignRight);
		}
}

/******************************************************************************
 DrawBreakpoints (private)

 ******************************************************************************/

void
CMLineNumberTable::DrawBreakpoints
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	// check for breakpoint(s) on this line

	JBoolean hasMultiple;
	if (!FindNextBreakpoint(cell.y, &hasMultiple))
		{
		return;
		}

	// draw breakpoint(s)

	JRect r = rect;
	r.Shrink(kMarginWidth, kMarginWidth);

	if (hasMultiple)
		{
		if (r.height() < 9)		// to allow concentric circles to be distinguished
			{
			r.top    = rect.ycenter() - 4;
			r.bottom = r.top + 9;
			r.left   = rect.xcenter() - 4;
			r.right  = r.left + 9;
			}

		p.Ellipse(r);
		r.Shrink(3, 3);
		p.Ellipse(r);
		}
	else
		{
		DrawBreakpoint(itsBPList->NthElement(itsBPDrawIndex), p, GetColormap(), r);
		}
}

/******************************************************************************
 GetBreakpointColor (static)

 ******************************************************************************/

JColorIndex
CMLineNumberTable::GetBreakpointColor
	(
	const CMBreakpoint*	bp,
	JColormap*			cmap
	)
{
	JColorIndex color;
	if (!bp->IsEnabled())
		{
		return cmap->GetGreenColor();
		}
	else if (bp->HasCondition())
		{
		return cmap->GetYellowColor();
		}
	else
		{
		return cmap->GetRedColor();
		}
}

/******************************************************************************
 DrawBreakpoint (static)

 ******************************************************************************/

void
CMLineNumberTable::DrawBreakpoint
	(
	const CMBreakpoint*	bp,
	JPainter&			p,
	JColormap*			cmap,
	const JRect&		rect
	)
{
	const JColorIndex color = GetBreakpointColor(bp, cmap);

	p.SetPenColor(color);
	p.SetFilling(kJTrue);
	p.Ellipse(rect);

	if (bp->GetAction() != CMBreakpoint::kRemoveBreakpoint)
		{
		p.SetPenColor(cmap->GetBlackColor());
		p.SetFilling(kJFalse);
		p.Ellipse(rect);
		}
}

/******************************************************************************
 FindNextBreakpoint (private)

	Increments itsBPDrawIndex to point to the first breakpoint at or beyond
	the given rowIndex.  Returns kJTrue if the breakpoint is on the
	specified line.

	If multiple != NULL, it it set to kJTrue if there is more than one
	breakpoint on the specified line.

 ******************************************************************************/

JBoolean
CMLineNumberTable::FindNextBreakpoint
	(
	const JIndex	rowIndex,
	JBoolean*		multiple
	)
{
	if (multiple != NULL)
		{
		*multiple = kJFalse;
		}

	while (itsBPList->IndexValid(itsBPDrawIndex))
		{
		CMBreakpoint* bp = itsBPList->NthElement(itsBPDrawIndex);
		const JIndex i   = itsVisualBPIndexList->GetElement(itsBPDrawIndex);
		if (i == rowIndex)
			{
			if (multiple != NULL)
				{
				*multiple = HasMultipleBreakpointsOnLine(itsBPDrawIndex);
				}
			return kJTrue;
			}
		if (i > rowIndex)
			{
			return kJFalse;
			}
		itsBPDrawIndex++;
		}

	return kJFalse;
}

/******************************************************************************
 HasMultipleBreakpointsOnLine (private)

 ******************************************************************************/

JBoolean
CMLineNumberTable::HasMultipleBreakpointsOnLine
	(
	const JIndex firstBPIndex
	)
	const
{
	return JI2B(itsBPList->IndexValid(firstBPIndex+1) &&
				(itsBPList->NthElement(firstBPIndex))->GetLineNumber() ==
					(itsBPList->NthElement(firstBPIndex+1))->GetLineNumber());
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CMLineNumberTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (ScrollForWheel(button, modifiers, NULL, itsVScrollbar))
		{
		return;
		}

	JPoint cell;
	if (!GetCell(pt, &cell))
		{
		return;
		}

	itsText->SelectLine(cell.y);

	if (button == kJXRightButton)
		{
		OpenLineMenu(cell.y, pt, buttonStates, modifiers, kJFalse);
		}
	else if (button == kJXLeftButton && modifiers.meta() && !modifiers.shift())
		{
		const JString* fullName;
		const JBoolean hasFile = itsDirector->GetFileName(&fullName);
		assert( hasFile );
		itsLink->RunUntil(*fullName, itsText->VisualLineIndexToCRLineIndex(cell.y));
		}
	else if (button == kJXLeftButton && modifiers.control())
		{
		const JString* fullName;
		const JBoolean hasFile = itsDirector->GetFileName(&fullName);
		assert( hasFile );
		itsLink->SetExecutionPoint(*fullName, itsText->VisualLineIndexToCRLineIndex(cell.y));
		}
	else if (button == kJXLeftButton)
		{
		AdjustBreakpoints(cell.y, pt, buttonStates, modifiers);
		}
}

/******************************************************************************
 AdjustBreakpoints (private)

 ******************************************************************************/

void
CMLineNumberTable::AdjustBreakpoints
	(
	const JIndex			visualIndex,
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const JIndex lineIndex = itsText->VisualLineIndexToCRLineIndex(visualIndex);

	CMBreakpoint bp("", lineIndex);
	JIndex bpIndex;
	if (itsBPList->SearchSorted(&bp, JOrderedSetT::kFirstMatch, &bpIndex))
		{
		if (HasMultipleBreakpointsOnLine(bpIndex))
			{
			OpenLineMenu(lineIndex, pt, buttonStates, modifiers, kJTrue, bpIndex);
			}
		else if (modifiers.shift())
			{
			(itsBPList->NthElement(bpIndex))->ToggleEnabled();
			}
		else
			{
			itsLink->RemoveBreakpoint(*(itsBPList->NthElement(bpIndex)));
			}
		}
	else if (modifiers.meta() && modifiers.shift())
		{
		const JString* fullName;
		const JBoolean hasFile = itsDirector->GetFileName(&fullName);
		assert( hasFile );

		itsLink->SetBreakpoint(*fullName, lineIndex, kJTrue);
		}
	else
		{
		const JString* fullName;
		const JBoolean hasFile = itsDirector->GetFileName(&fullName);
		assert( hasFile );

		itsLink->SetBreakpoint(*fullName, lineIndex);
		}
}

/******************************************************************************
 OpenLineMenu (private)

 ******************************************************************************/

void
CMLineNumberTable::OpenLineMenu
	(
	const JIndex			visualIndex,
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	const JBoolean			onlyBreakpoints,
	const JIndex			firstBPIndex
	)
{
	const JIndex lineIndex = itsText->VisualLineIndexToCRLineIndex(visualIndex);

	if (itsLineMenu == NULL)
		{
		itsLineMenu = new JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsLineMenu != NULL );
		itsLineMenu->SetToHiddenPopupMenu(kJTrue);
		itsLineMenu->SetUpdateAction(JXMenu::kDisableNone);
		ListenTo(itsLineMenu);
		}

	itsLineMenuLineIndex  = lineIndex;
	itsIsFullLineMenuFlag = !onlyBreakpoints;

	itsLineMenu->RemoveAllItems();
	if (itsIsFullLineMenuFlag)
		{
		itsLineMenu->AppendMenuItems(kLineMenuStr);
		}
	itsLineMenu->AppendMenuItems(kAllBreakpointsMenuStr);

	JIndex bpIndex = firstBPIndex;
	if (bpIndex == 0)
		{
		CMBreakpoint bp("", lineIndex);
		if (!itsBPList->SearchSorted(&bp, JOrderedSetT::kFirstMatch, &bpIndex))
			{
			bpIndex == 0;
			}
		}

	itsLineMenuRange.SetToNothing();
	if (itsBPList->IndexValid(bpIndex))
		{
		itsLineMenuRange.first = bpIndex;
		}

	while (itsBPList->IndexValid(bpIndex))
		{
		itsLineMenuRange.last  = bpIndex;
		const CMBreakpoint* bp = itsBPList->NthElement(bpIndex);

		const JString bpIndexStr(bp->GetDebuggerIndex(), 0);
		const JString ignoreCountStr(bp->GetIgnoreCount(), 0);
		const JCharacter* map[] =
			{
			"index",        bpIndexStr.GetCString(),
			"ignore_count", ignoreCountStr.GetCString()
			};

		JString s = kBreakpointMenuStr;
		(JGetStringManager())->Replace(&s, map, sizeof(map));

		itsLineMenu->AppendMenuItems(s);

		if (!HasMultipleBreakpointsOnLine(bpIndex))
			{
			break;
			}
		bpIndex++;
		}

	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectRow(lineIndex);
	if (itsDeselectTask == NULL)
		{
		itsDeselectTask = new CMDeselectLineNumberTask(this);
		assert( itsDeselectTask != NULL );
		itsDeselectTask->Start();
		}

	itsLineMenu->PopUp(this, pt, buttonStates, modifiers);
}

/******************************************************************************
 UpdateLineMenu (private)

 ******************************************************************************/

void
CMLineNumberTable::UpdateLineMenu()
{
	JSize offset = 0;
	if (itsIsFullLineMenuFlag)
		{
		const JBoolean stopped = itsLink->ProgramIsStopped();
		itsLineMenu->SetItemEnable(offset + kRunUntilCmd, stopped);
		itsLineMenu->SetItemEnable(offset + kSetExecPtCmd,
			JI2B(stopped && itsLink->GetFeature(CMLink::kSetExecutionPoint)));

		offset += kLineMenuItemCount;
		}

	if (itsLineMenuRange.IsEmpty())
		{
		itsLineMenu->DisableItem(offset + kRemoveAllBreakpointsCmd);
		}
	offset += kAllBreakpointsMenuItemCount;

	if (!itsLineMenuRange.IsNothing())
		{
		itsLineMenu->SetItemEnable(offset + kShowBreakpointInfoCmd,
			itsLink->GetFeature(CMLink::kShowBreakpointInfo));
		itsLineMenu->SetItemEnable(offset + kSetConditionCmd,
			itsLink->GetFeature(CMLink::kSetBreakpointCondition));
		itsLineMenu->SetItemEnable(offset + kIgnoreNextNCmd,
			itsLink->GetFeature(CMLink::kSetBreakpointIgnoreCount));

		for (JIndex i=itsLineMenuRange.first; i<=itsLineMenuRange.last; i++)
			{
			const CMBreakpoint* bp = itsBPList->NthElement(i);
			if (!bp->HasCondition())
				{
				itsLineMenu->DisableItem(offset + kRemoveConditionCmd);
				}
			if (bp->IsEnabled())
				{
				itsLineMenu->CheckItem(offset + kToggleEnableCmd);
				}

			offset += kBreakpointMenuItemCount;
			}
		}
}

/******************************************************************************
 HandleLineMenu (private)

 ******************************************************************************/

void
CMLineNumberTable::HandleLineMenu
	(
	const JIndex index
	)
{
	const JString* fullName;
	const JBoolean hasFile = itsDirector->GetFileName(&fullName);
	assert( hasFile );

	// line

	JSize offset = 0;
	if (itsIsFullLineMenuFlag)
		{
		if (index == offset + kRunUntilCmd)
			{
			itsLink->RunUntil(*fullName, itsLineMenuLineIndex);
			return;
			}
		else if (index == offset + kSetExecPtCmd)
			{
			itsLink->SetExecutionPoint(*fullName, itsLineMenuLineIndex);
			return;
			}
		offset += kLineMenuItemCount;
		}

	// all breakpoints

	if (index == offset + kSetBreakpointCmd)
		{
		itsLink->SetBreakpoint(*fullName, itsLineMenuLineIndex);
		return;
		}
	else if (index == offset + kSetTempBreakpointCmd)
		{
		itsLink->SetBreakpoint(*fullName, itsLineMenuLineIndex, kJTrue);
		return;
		}
	else if (index == offset + kRemoveAllBreakpointsCmd)
		{
		itsLink->RemoveAllBreakpointsOnLine(*fullName, itsLineMenuLineIndex);
		return;
		}
	offset += kAllBreakpointsMenuItemCount;

	// individual breakpoints

	if (!itsLineMenuRange.IsNothing())
		{
		for (JIndex i=itsLineMenuRange.first; i<=itsLineMenuRange.last; i++)
			{
			CMBreakpoint* bp = itsBPList->NthElement(i);
			if (index == offset + kShowBreakpointInfoCmd)
				{
				(itsDirector->GetCommandDirector()->GetBreakpointsDir()->GetBreakpointTable())->Show(bp);
				return;
				}
			else if (index == offset + kRemoveBreakpointCmd)
				{
				itsLink->RemoveBreakpoint(*bp);
				return;
				}
			else if (index == offset + kSetConditionCmd)
				{
				(itsDirector->GetCommandDirector()->GetBreakpointsDir()->GetBreakpointTable())->EditCondition(bp);
				return;
				}
			else if (index == offset + kRemoveConditionCmd)
				{
				bp->RemoveCondition();
				return;
				}
			else if (index == offset + kToggleEnableCmd)
				{
				bp->ToggleEnabled();
				return;
				}
			else if (index == offset + kIgnoreNextNCmd)
				{
				(itsDirector->GetCommandDirector()->GetBreakpointsDir()->GetBreakpointTable())->EditIgnoreCount(bp);
				return;
				}

			offset += kBreakpointMenuItemCount;
			}
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMLineNumberTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsText &&
		(message.Is(JTextEditor::kTextChanged) ||
		 message.Is(JTextEditor::kTextSet)))
		{
		AdjustToText();
		}

	else if (sender == itsVScrollbar && message.Is(JXScrollbar::kScrolled))
		{
		const JXScrollbar::Scrolled* info =
			dynamic_cast<const JXScrollbar::Scrolled*>(&message);
		assert( info != NULL );
		ScrollTo(0, info->GetValue());
		}

	else if (sender == itsLink &&
			 (message.Is(CMLink::kDebuggerStarted)     ||
			  message.Is(CMLink::kSymbolsLoaded)       ||
			  message.Is(CMLink::kAttachedToProcess)   ||
			  message.Is(CMLink::kDetachedFromProcess) ||
			  message.Is(CMLink::kProgramRunning)      ||
			  message.Is(CMLink::kProgramFinished)))
		{
		if (!itsLink->HasCore())
			{
			SetCurrentLine(0);
			}
		}

	else if (sender == itsLink->GetBreakpointManager() &&
			 message.Is(CMBreakpointManager::kBreakpointsChanged))
		{
		UpdateBreakpoints();
		}

	else if (sender == itsLineMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateLineMenu();
		}
	else if (sender == itsLineMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleLineMenu(selection->GetIndex());
		}

	else
		{
		JXTable::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
CMLineNumberTable::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !CMIsShuttingDown())
		{
		itsLink = CMGetLink();
		ListenTo(itsLink);
		ListenTo(itsLink->GetBreakpointManager());

		itsBPList->RemoveAll();
		itsVisualBPIndexList->RemoveAll();
		}
	else
		{
		JXTable::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 AdjustToText (private)

 ******************************************************************************/

void
CMLineNumberTable::AdjustToText()
{
	const JFontManager* fontMgr = GetFontManager();

	JFontID fontID;
	JSize fontSize;
	JFontStyle fontStyle;
	itsText->GetCurrentFont(&fontID, &fontSize, &fontStyle);

	const JSize lineCount = itsText->IsEmpty() ? 0 : itsText->GetLineCount();

	JString lineCountStr;
	if (itsLineNumberList != NULL && !itsLineNumberList->IsEmpty())
		{
		const JString* s = itsLineNumberList->LastElement();
		lineCountStr     = s->GetSubstring(itsLineNumberStartIndex, s->GetLength());
		}
	else
		{
		lineCountStr = JString(lineCount, JString::kBase10);
		}

	const JSize lineHeight      = fontMgr->GetLineHeight(fontID, fontSize, fontStyle);
	const JSize lineNumberWidth = fontMgr->GetStringWidth(fontID, fontSize, fontStyle, lineCountStr);

	SetColWidth(kBreakpointColumn, lineHeight);
	SetColWidth(kExecPointColumn,  lineHeight);
	SetColWidth(kLineNumberColumn, lineNumberWidth + 2*kMarginWidth);

	SetAllRowHeights(lineHeight);
	const JSize origRowCount = GetRowCount();
	if (origRowCount < lineCount)
		{
		AppendRows(lineCount - origRowCount, lineHeight);
		}
	else if (origRowCount > lineCount)
		{
		RemovePrevRows(origRowCount, origRowCount - lineCount);
		}

	const JCoordinate tableWidth = GetBoundsWidth();
	const JCoordinate apWidth    = GetApertureWidth();
	if (tableWidth != apWidth)
		{
		AdjustSize(tableWidth-apWidth, 0);
		itsText->Place(GetFrameWidth(), 0);
		itsText->AdjustSize(apWidth-tableWidth, 0);
		}

	ScrollTo(0, itsVScrollbar->GetValue());
	UpdateBreakpoints();
}

/******************************************************************************
 UpdateBreakpoints (private)

 ******************************************************************************/

void
CMLineNumberTable::UpdateBreakpoints()
{
	itsBPList->RemoveAll();
	itsVisualBPIndexList->RemoveAll();

	const JString* fileName;
	if (itsDirector->GetFileName(&fileName))
		{
		(itsLink->GetBreakpointManager())->GetBreakpoints(*fileName, itsBPList);

		const JSize count = itsBPList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const CMBreakpoint* bp = itsBPList->NthElement(i);
			itsVisualBPIndexList->AppendElement(
				itsText->CRLineIndexToVisualLineIndex(bp->GetLineNumber()));
			}
		}

	Refresh();
}

/******************************************************************************
 CompareBreakpointLines (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
CMLineNumberTable::CompareBreakpointLines
	(
	CMBreakpoint* const & bp1,
	CMBreakpoint* const & bp2
	)
{
	return JCompareIndices(bp1->GetLineNumber(), bp2->GetLineNumber());
}
