/******************************************************************************
 GMFilterNameTable.cc

	BASE CLASS = public JXEditTable

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMFilterNameTable.h>
#include <GMFilterConditionTable.h>
#include <GMFilterActionTable.h>
#include <GFilterDragData.h>
#include <GMFilter.h>

#include <JXDisplay.h>
#include <JXDNDManager.h>
#include <JXInputField.h>
#include <JXTextButton.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>

#include <JFontManager.h>
#include <JPainter.h>
#include <JTableSelection.h>

#include <jXGlobals.h>

#include <jAssert.h>

const JCoordinate kDefRowHeight		= 20;
const JCoordinate kDefColWidth		= 100;

const JCoordinate kHMarginWidth	= 3;
const JCoordinate kRowHeightAdjust	= 2;

const JSize kDragLineWidth			= 2;
const JSize kDragLineAdjust			= 2;
const JCoordinate kDebounceWidth	= 3;

static const JCharacter* kDragFilterXAtomName	= "GMFilterName";
static const JCharacter* kDNDClassID			= "GMFilterNameTable";

/******************************************************************************
 Create (static)

 *****************************************************************************/

GMFilterNameTable*
GMFilterNameTable::Create
	(
	GMFilterDialog*		dialog,
	JPtrArray<GMFilter>*	filters,
	GMFilterConditionTable* conditionTable,
	GMFilterActionTable*	mboxTable,
	JXTextButton*			newButton,
	JXTextButton*			removeButton,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
{
	GMFilterNameTable* widget	=
		new GMFilterNameTable(dialog,
			filters, conditionTable, mboxTable, newButton, removeButton,
			scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h);
	assert(widget != NULL);
	widget->GMFilterNameTableX();
	return widget;
}

/******************************************************************************
 Constructor

 *****************************************************************************/

GMFilterNameTable::GMFilterNameTable
	(
	GMFilterDialog*		dialog,
	JPtrArray<GMFilter>*	filters,
	GMFilterConditionTable* conditionTable,
	GMFilterActionTable*	mboxTable,
	JXTextButton*			newButton,
	JXTextButton*			removeButton,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXEditTable(kDefRowHeight, kDefColWidth, scrollbarSet,
			enclosure, hSizing, vSizing, x,y, w,h),
	itsDialog(dialog),
	itsNewButton(newButton),
	itsRemoveButton(removeButton),
	itsInput(NULL),
	itsFilters(filters),
	itsConditionTable(conditionTable),
	itsActionTable(mboxTable),
	itsDNDIsValid(kJFalse),
	itsCurrentDNDIndex(0)
{
}

/******************************************************************************
 GMFilterNameTableX (private)

 ******************************************************************************/

void
GMFilterNameTable::GMFilterNameTableX()
{
	AppendCols(1, kDefColWidth);
	AdjustColWidths();

	const JFontManager* fm	= GetFontManager();
	itsRowHeight	=
		fm->GetLineHeight(JGetDefaultFontName(), kJDefaultFontSize,
						JFontStyle()) + kRowHeightAdjust;

	const JSize count = itsFilters->GetElementCount();
	AppendRows(count, itsRowHeight);

	AdjustButtons();
	WantInput(kJFalse);

	ListenTo(itsNewButton);
	ListenTo(itsRemoveButton);

	const Atom dndName	= (GetDNDManager())->GetDNDSelectionName();
	GetDisplay()->RegisterXAtom(kDragFilterXAtomName);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMFilterNameTable::~GMFilterNameTable()
{
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
GMFilterNameTable::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXEditTable::Draw(p, rect);
	if (itsDNDIsValid)
		{
		p.ResetClipRect();
		p.SetLineWidth(kDragLineWidth);
		if (itsCurrentDNDIndex == 0)
			{
			JRect crect =
				GetCellRect(JPoint(1, GetRowCount()));
			p.Line(0, crect.bottom, GetBoundsWidth(), crect.bottom);
			}
		else
			{
			JRect crect =
				GetCellRect(JPoint(1, itsCurrentDNDIndex));
			p.Line(0, crect.top, GetBoundsWidth(), crect.top);
			}
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GMFilterNameTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	assert(cell.y <= (JCoordinate)itsFilters->GetElementCount());

	HilightIfSelected(p, cell, rect);

	JRect r = rect;
	r.left += kHMarginWidth;
	p.JPainter::String(r, itsFilters->NthElement(cell.y)->GetNickname(),
			 JPainter::kHAlignLeft, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
GMFilterNameTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton	button,
	const JSize			clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsIsWaitingForDrag	= kJFalse;
	itsStartPt			= pt;
	itsDNDIsValid		= kJFalse;

	if (button > kJXRightButton)
		{
		ScrollForWheel(button, modifiers);
		return;
		}

	JPoint cell;
	if (!GetCell(pt, &cell))
		{
		return;
		}

	itsCurrentCell		= cell;

	JTableSelection& s	= GetTableSelection();

	JPoint oldCell;
	JBoolean hasSingleSel	= s.GetFirstSelectedCell(&oldCell);

	if (itsConditionTable->OKToSwitch() &&
		itsActionTable->OKToSwitch() &&
		(!IsEditing() || EndEditing()))
		{
		if ((hasSingleSel && cell.y != oldCell.y) ||
			(!hasSingleSel))
			{
			SelectFilter(cell.y);
			}
		itsIsWaitingForDrag	= kJTrue;
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
GMFilterNameTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsIsWaitingForDrag &&
		(JLAbs(pt.x - itsStartPt.x) > kDebounceWidth ||
		 JLAbs(pt.y - itsStartPt.y) > kDebounceWidth))
		{
		GFilterDragData* data =
			new GFilterDragData(this, kDNDClassID);
		assert(data != NULL);
		if (!BeginDND(pt, buttonStates, modifiers, data))
			{
			itsIsWaitingForDrag	= kJFalse;
			}
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
GMFilterNameTable::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton	button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (!GetCell(pt, &cell))
		{
		return;
		}
	if (cell == itsCurrentCell && itsIsWaitingForDrag)
		{
		BeginEditing(cell);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GMFilterNameTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsNewButton && message.Is(JXButton::kPushed))
		{
		if (GetWindow()->OKToUnfocusCurrentWidget())
			{
			NewFilter();
			}
		}
	else if (sender == itsRemoveButton && message.Is(JXButton::kPushed))
		{
		RemoveSelectedFilters();
		}
	else
		{
		JXEditTable::Receive(sender, message);
		}
}


/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
GMFilterNameTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	itsInput	=
		new JXInputField(this, kFixedLeft, kFixedTop, x, y, w, h);
	assert(itsInput != NULL);
	itsInput->SetIsRequired(kJTrue);
	GMFilter* filter	= itsFilters->NthElement(cell.y);
	itsInput->SetText(filter->GetNickname());
	return itsInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
GMFilterNameTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	JBoolean ok = itsInput->InputValid();
	if (ok)
		{
		GMFilter* filter	= itsFilters->NthElement(cell.y);
		filter->SetNickname(itsInput->GetText());
		}
	return ok;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
GMFilterNameTable::PrepareDeleteXInputField()
{
	itsInput = NULL;
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
GMFilterNameTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXEditTable::ApertureResized(dw,dh);
	AdjustColWidths();
}

/******************************************************************************
 AdjustColWidths (private)

 ******************************************************************************/

void
GMFilterNameTable::AdjustColWidths()
{
	SetColWidth(1, GetApertureWidth());
}

/******************************************************************************
 AdjustButtons (private)

 ******************************************************************************/

void
GMFilterNameTable::AdjustButtons()
{
	if (GetRowCount() == 0 || !GetTableSelection().HasSelection())
		{
		itsRemoveButton->Deactivate();
		}
	else
		{
		itsRemoveButton->Activate();
		}
}

/******************************************************************************
 NewFilter (private)

 ******************************************************************************/

void
GMFilterNameTable::NewFilter()
{
	if (!itsConditionTable->OKToSwitch() || !itsActionTable->OKToSwitch())
		{
		return;
		}

	GMFilter* filter	= new GMFilter();
	assert(filter != NULL);

	itsFilters->Append(filter);

	JString nickname	= "filter" + JString(itsFilters->GetElementCount());
	filter->SetNickname(nickname);

	AppendRows(1, itsRowHeight);
	itsConditionTable->SetFilter(filter);
	itsActionTable->SetFilter(filter);

	GetTableSelection().ClearSelection();
	GetTableSelection().SelectCell(GetRowCount(), 1);

	AdjustButtons();
}

/******************************************************************************
 RemoveSelectedFilters (private)

 ******************************************************************************/

void
GMFilterNameTable::RemoveSelectedFilters()
{
	JTableSelection& s	= GetTableSelection();
	assert(s.HasSelection());

	JTableSelectionIterator iter(&s);
	JPoint cell;
	while (iter.Next(&cell))
		{
		itsFilters->DeleteElement(cell.y);
		RemoveRow(cell.y);
		}
	itsConditionTable->SetFilter(NULL);
	itsActionTable->SetFilter(NULL);
	AdjustButtons();
}

/******************************************************************************
 SelectFilter (private)

 ******************************************************************************/

void
GMFilterNameTable::SelectFilter
	(
	const JIndex index
	)
{
	assert(itsFilters->IndexValid(index));
	GMFilter* filter	= itsFilters->NthElement(index);

	itsConditionTable->SetFilter(filter);
	itsActionTable->SetFilter(filter);

	JTableSelection& s	= GetTableSelection();

	s.ClearSelection();
	s.SelectCell(index, 1);
}

/******************************************************************************
 OKToDeactivate (public)

 ******************************************************************************/

JBoolean
GMFilterNameTable::OKToDeactivate()
{
	return kJTrue;
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
GMFilterNameTable::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return (GetDNDManager())->GetDNDActionMoveXAtom();
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

 ******************************************************************************/

JBoolean
GMFilterNameTable::WillAcceptDrop
	(
	const JArray<Atom>& typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	if (source == this)
		{
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 HandleDNDHere (virtual protected)

 ******************************************************************************/

void
GMFilterNameTable::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget*	source
	)
{
	itsDNDIsValid	= kJTrue;
	JIndex oldIndex	= itsCurrentDNDIndex;

	JPoint cell;
	if (GetCell(pt, &cell))
		{
		itsCurrentDNDIndex	= cell.y;
		}
	else
		{
		itsCurrentDNDIndex	= 0;
		}

	if (oldIndex != 0)
		{
		TableRefreshRow(oldIndex);
		}
	else
		{
		const JSize count	= GetRowCount();
		if (count > 0)
			{
			TableRefreshRow(count);
			}
		}

	if (itsCurrentDNDIndex != 0)
		{
		TableRefreshRow(itsCurrentDNDIndex);
		}
	else
		{
		const JSize count	= GetRowCount();
		if (count > 0)
			{
			TableRefreshRow(count);
			}
		}
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

 ******************************************************************************/

void
GMFilterNameTable::HandleDNDLeave()
{
	itsDNDIsValid		= kJFalse;
	itsCurrentDNDIndex	= 0;
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

 ******************************************************************************/

void
GMFilterNameTable::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>& typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*	source
	)
{
	JIndex dropIndex	= itsCurrentDNDIndex;
	if (dropIndex == 0)
		{
		dropIndex		= GetRowCount() + 1;
		}
	JPoint cell;
	if (GetTableSelection().GetFirstSelectedCell(&cell))
		{
		if (cell.y == (JCoordinate)itsCurrentDNDIndex ||
			cell.y == (JCoordinate)itsCurrentDNDIndex + 1)
			{
			return;
			}
		GMFilter* filter	= itsFilters->NthElement(cell.y);
		itsFilters->Remove(filter);
		JIndex newIndex		= dropIndex;
		if (cell.y < (JCoordinate)newIndex)
			{
			newIndex--;
			}
		itsFilters->InsertAtIndex(newIndex, filter);
		SelectFilter(newIndex);
		}
	HandleDNDLeave();
}
