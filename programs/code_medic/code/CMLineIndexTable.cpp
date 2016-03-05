/******************************************************************************
 CMLineIndexTable.cpp

	Base class for displaying values corresponding to lines in CMSourceText.

	BASE CLASS = JXTable

	Copyright © 2001-16 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "CMLineIndexTable.h"
#include "CMSourceDirector.h"
#include "CMSourceText.h"
#include "CMCommandDirector.h"
#include "CMBreakpointsDir.h"
#include "CMBreakpointTable.h"
#include "CMBreakpointManager.h"
#include "CMAdjustLineTableToTextTask.h"
#include "CMDeselectLineTask.h"
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

CMLineIndexTable::CMLineIndexTable
	(
	JOrderedSetT::CompareResult (*bpCcompareFn)(CMBreakpoint *const &, CMBreakpoint *const &),

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
	itsCurrentLineIndex(0),
	itsLineMenu(NULL),
	itsDeselectTask(NULL)
{
	itsLink = CMGetLink();
	ListenTo(itsLink);

	itsBPList = new JPtrArray<CMBreakpoint>(JPtrArrayT::kForgetAll);
	assert(itsBPList != NULL);
	itsBPList->SetCompareFunction(bpCcompareFn);
	itsBPList->SetSortOrder(JOrderedSetT::kSortAscending);

	WantInput(kJFalse);
	SetBackColor((CMGetPrefsManager())->GetColor(CMPrefsManager::kBackColorIndex));
	SetFocusColor(GetBackColor());
	SetRowBorderInfo(0, GetBackColor());
	SetColBorderInfo(0, GetBackColor());
	SetDrawOrder(kDrawByCol);

	AppendCols(3);

	CMAdjustLineTableToTextTask* task = new CMAdjustLineTableToTextTask(this);
	assert( task != NULL );
	task->Go();

	ListenTo(itsText);
	ListenTo(itsVScrollbar);
	ListenTo(itsLink->GetBreakpointManager());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMLineIndexTable::~CMLineIndexTable()
{
	delete itsBPList;
	delete itsDeselectTask;
}

/******************************************************************************
 SetCurrentLine

 ******************************************************************************/

void
CMLineIndexTable::SetCurrentLine
	(
	const JIndex line
	)
{
	itsCurrentLineIndex = line;
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
CMLineIndexTable::TablePrepareToDrawCol
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
CMLineIndexTable::TableDrawCell
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

	else if (JIndex(cell.x) == kExecPointColumn && cell.y == itsCurrentLineIndex)
		{
		// We can't use a static polygon because line heights can vary,
		// e.g. due to underlines.

		const JCoordinate delta = rect.height()/2;

		JPolygon poly;
		poly.AppendElement(JPoint(rect.left+kMarginWidth,       rect.top));
		poly.AppendElement(JPoint(rect.left+kMarginWidth+delta, rect.top + delta));
		poly.AppendElement(JPoint(rect.left+kMarginWidth,       rect.top + 2*delta));

		p.SetPenColor(GetCurrentLineMarkerColor());
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

		JRect r  = rect;
		r.right -= kMarginWidth;

		const JString str = GetLineText(cell.y);
		p.String(r, str, JPainter::kHAlignRight);
		}
}

/******************************************************************************
 DrawBreakpoints (private)

 ******************************************************************************/

void
CMLineIndexTable::DrawBreakpoints
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
 DrawBreakpoint (static)

 ******************************************************************************/

void
CMLineIndexTable::DrawBreakpoint
	(
	const CMBreakpoint*	bp,
	JPainter&			p,
	JColormap*			cmap,
	const JRect&		rect
	)
{
	const JColorIndex color =
		!bp->IsEnabled()   ? cmap->GetGreenColor() :
		bp->HasCondition() ? cmap->GetYellowColor() : cmap->GetRedColor();

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
CMLineIndexTable::FindNextBreakpoint
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
		const JIndex i   = itsText->CRLineIndexToVisualLineIndex(GetBreakpointLineIndex(itsBPDrawIndex, bp));
		if (i == rowIndex)
			{
			if (multiple != NULL && itsBPList->IndexValid(itsBPDrawIndex+1))
				{
				*multiple = BreakpointsOnSameLine(bp, itsBPList->NthElement(itsBPDrawIndex+1));
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
CMLineIndexTable::HasMultipleBreakpointsOnLine
	(
	const JIndex bpIndex
	)
	const
{
	if (!itsBPList->IndexValid(bpIndex) ||
		!itsBPList->IndexValid(bpIndex+1))
		{
		return kJFalse;
		}

	return BreakpointsOnSameLine(
		itsBPList->NthElement(bpIndex),
		itsBPList->NthElement(bpIndex+1));
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CMLineIndexTable::HandleMouseDown
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
	const JIndex lineIndex = itsText->VisualLineIndexToCRLineIndex(cell.y);

	if (button == kJXRightButton)
		{
		OpenLineMenu(lineIndex, pt, buttonStates, modifiers, kJFalse);
		}
	else if (button == kJXLeftButton &&
			 modifiers.GetState(JXMenu::AdjustNMShortcutModifier(kJXMetaKeyIndex)) &&
			 !modifiers.shift())
		{
		RunUntil(lineIndex);
		}
	else if (button == kJXLeftButton &&
			 modifiers.GetState(JXMenu::AdjustNMShortcutModifier(kJXControlKeyIndex)))
		{
		SetExecutionPoint(lineIndex);
		}
	else if (button == kJXLeftButton)
		{
		AdjustBreakpoints(lineIndex, pt, buttonStates, modifiers);
		}
}

/******************************************************************************
 AdjustBreakpoints (private)

 ******************************************************************************/

void
CMLineIndexTable::AdjustBreakpoints
	(
	const JIndex			lineIndex,
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JIndex bpIndex;
	if (!GetFirstBreakpointOnLine(lineIndex, &bpIndex))
		{
		SetBreakpoint(lineIndex, JI2B(
			modifiers.GetState(JXMenu::AdjustNMShortcutModifier(kJXMetaKeyIndex)) &&
			modifiers.shift()));
		}
	else if (HasMultipleBreakpointsOnLine(bpIndex))
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

/******************************************************************************
 OpenLineMenu (private)

 ******************************************************************************/

void
CMLineIndexTable::OpenLineMenu
	(
	const JIndex			lineIndex,
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	const JBoolean			onlyBreakpoints,
	const JIndex			firstBPIndex
	)
{
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
	if (bpIndex == 0 && !GetFirstBreakpointOnLine(lineIndex, &bpIndex))
		{
		bpIndex == 0;
		}

	itsLineMenuBPRange.SetToNothing();
	if (itsBPList->IndexValid(bpIndex))
		{
		itsLineMenuBPRange.first = bpIndex;
		}

	while (itsBPList->IndexValid(bpIndex))
		{
		itsLineMenuBPRange.last = bpIndex;
		const CMBreakpoint* bp  = itsBPList->NthElement(bpIndex);

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
		itsDeselectTask = new CMDeselectLineTask(this);
		assert( itsDeselectTask != NULL );
		itsDeselectTask->Start();
		}

	itsLineMenu->PopUp(this, pt, buttonStates, modifiers);
}

/******************************************************************************
 UpdateLineMenu (private)

 ******************************************************************************/

void
CMLineIndexTable::UpdateLineMenu()
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

	if (itsLineMenuBPRange.IsEmpty())
		{
		itsLineMenu->DisableItem(offset + kRemoveAllBreakpointsCmd);
		}
	offset += kAllBreakpointsMenuItemCount;

	if (!itsLineMenuBPRange.IsNothing())
		{
		for (JIndex i=itsLineMenuBPRange.first; i<=itsLineMenuBPRange.last; i++)
			{
			itsLineMenu->SetItemEnable(offset + kShowBreakpointInfoCmd,
				itsLink->GetFeature(CMLink::kShowBreakpointInfo));
			itsLineMenu->SetItemEnable(offset + kSetConditionCmd,
				itsLink->GetFeature(CMLink::kSetBreakpointCondition));
			itsLineMenu->SetItemEnable(offset + kIgnoreNextNCmd,
				itsLink->GetFeature(CMLink::kSetBreakpointIgnoreCount));

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
CMLineIndexTable::HandleLineMenu
	(
	const JIndex index
	)
{
	// line

	JSize offset = 0;
	if (itsIsFullLineMenuFlag)
		{
		if (index == offset + kRunUntilCmd)
			{
			RunUntil(itsLineMenuLineIndex);
			return;
			}
		else if (index == offset + kSetExecPtCmd)
			{
			SetExecutionPoint(itsLineMenuLineIndex);
			return;
			}
		offset += kLineMenuItemCount;
		}

	// all breakpoints

	if (index == offset + kSetBreakpointCmd)
		{
		SetBreakpoint(itsLineMenuLineIndex, kJFalse);
		return;
		}
	else if (index == offset + kSetTempBreakpointCmd)
		{
		SetBreakpoint(itsLineMenuLineIndex, kJTrue);
		return;
		}
	else if (index == offset + kRemoveAllBreakpointsCmd)
		{
		RemoveAllBreakpointsOnLine(itsLineMenuLineIndex);
		return;
		}
	offset += kAllBreakpointsMenuItemCount;

	// individual breakpoints

	if (!itsLineMenuBPRange.IsNothing())
		{
		for (JIndex i=itsLineMenuBPRange.first; i<=itsLineMenuBPRange.last; i++)
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
CMLineIndexTable::Receive
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
CMLineIndexTable::ReceiveGoingAway
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
CMLineIndexTable::AdjustToText()
{
	const JFontManager* fontMgr = GetFontManager();

	JFontID fontID;
	JSize fontSize;
	JFontStyle fontStyle;
	itsText->GetCurrentFont(&fontID, &fontSize, &fontStyle);

	const JSize lineCount       = itsText->IsEmpty() ? 0 : itsText->GetLineCount();
	const JString lineCountStr  = GetLongestLineText(lineCount);
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
CMLineIndexTable::UpdateBreakpoints()
{
	itsBPList->RemoveAll();
	GetBreakpoints(itsBPList);
	Refresh();
}
