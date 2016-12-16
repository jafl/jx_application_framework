/******************************************************************************
 CMBreakpointTable.cpp

	BASE CLASS = JXEditTable

	Copyright (C) 2010 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include "CMBreakpointTable.h"
#include "CMBreakpointsDir.h"
#include "CMBreakpointManager.h"
#include "CMCommandDirector.h"
#include "CMLineIndexTable.h"
#include "cmGlobals.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXColHeaderWidget.h>
#include <JXIntegerInput.h>
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <JTableSelection.h>
#include <JFontManager.h>
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <JOrderedSetUtil.h>
#include <jASCIIConstants.h>
#include <sstream>
#include <jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

enum
{
	kStatusColumn = 1,		// enabled, disabled, temporary, conditional
	kFileNameColumn,
	kLineNumberColumn,
	kFunctionColumn,
	kAddressColumn,
	kIgnoreCountColumn,
	kConditionColumn
};

const JCoordinate kInitColWidth[] =
{
	10, 150, 50, 150, 70, 50, 150
};

static const JCharacter* kColTitle[] =
{
	"",
	"File name",
	"Line",
	"Function",
	"Address",
	"Ignore",
	"Condition",
};

const JSize kColCount = sizeof(kInitColWidth) / sizeof(JCoordinate);

// geometry information

const JFileVersion kCurrentGeometryDataVersion = 0;
const JCharacter kGeometryDataEndDelimiter     = '\1';

/******************************************************************************
 Constructor

 ******************************************************************************/

CMBreakpointTable::CMBreakpointTable
	(
	CMBreakpointsDir*	dir,
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
	JXEditTable(1,1, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	JPrefObject(CMGetPrefsManager(), kBreakpointTableID),
	itsDir(dir),
	itsTextInput(NULL),
	itsFont(GetFontManager()->GetDefaultFont())
{
	itsBPList = jnew JPtrArray<CMBreakpoint>(JPtrArrayT::kForgetAll);
	assert(itsBPList != NULL);
	itsBPList->SetCompareFunction(CompareBreakpointLocations);
	itsBPList->SetSortOrder(JOrderedSetT::kSortAscending);

	// font

	JString fontName;
	JSize fontSize;
	CMGetPrefsManager()->GetDefaultFont(&fontName, &fontSize);
	itsFont = GetFontManager()->GetFont(fontName, fontSize);

	const JSize rowHeight = 2*kVMarginWidth + JMax(
		GetFontManager()->GetDefaultFont().GetLineHeight(),
		itsFont.GetLineHeight());
	SetDefaultRowHeight(rowHeight);

	// data

	for (JIndex i=1; i<=kColCount; i++)
		{
		AppendCols(1, kInitColWidth[i-1]);
		}

	SetColWidth(kStatusColumn, rowHeight);

	JPrefObject::ReadPrefs();

	ListenTo(&(GetTableSelection()));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMBreakpointTable::~CMBreakpointTable()
{
	JPrefObject::WritePrefs();

	jdelete itsBPList;
}

/******************************************************************************
 Update

 ******************************************************************************/

void
CMBreakpointTable::Update()
{
	CMBreakpointManager* mgr = (CMGetLink())->GetBreakpointManager();

	// save selected cell

	const JPoint cell = itsSelectedCell;

	// merge lists

	itsBPList->CleanOut();

	const JPtrArray<CMBreakpoint>& list1 = mgr->GetBreakpoints();

	JSize count = list1.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		itsBPList->InsertSorted(const_cast<CMBreakpoint*>(list1.NthElement(i)));
		}

	const JPtrArray<CMBreakpoint>& list2 = mgr->GetOtherpoints();

	count = list2.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		itsBPList->InsertSorted(const_cast<CMBreakpoint*>(list2.NthElement(i)));
		}

	// adjust table

	count            = itsBPList->GetElementCount();
	const JSize orig = GetRowCount();
	if (orig < count)
		{
		AppendRows(count - orig);
		}
	else if (orig > count)
		{
		RemoveNextRows(count+1, orig - count);
		}

	// restore selection

	if (cell.y > 0)
		{
		for (JIndex i=1; i<=count; i++)
			{
			if ((itsBPList->NthElement(i))->GetDebuggerIndex() == (JSize) cell.y)
				{
				SelectSingleCell(JPoint(cell.x, i));
				break;
				}
			}
		}

	Refresh();
}

/******************************************************************************
 Show

 ******************************************************************************/

void
CMBreakpointTable::Show
	(
	const CMBreakpoint* bp
	)
{
	JIndex i;
	if (FindBreakpointByDebuggerIndex(bp, &i))
		{
		itsDir->Activate();
		GetWindow()->RequestFocus();
		TableScrollToCell(JPoint(kIgnoreCountColumn, i), kJTrue);

		JTableSelection& s = GetTableSelection();
		s.ClearSelection();
		s.SelectRow(i);
		}
}

/******************************************************************************
 EditIgnoreCount

 ******************************************************************************/

void
CMBreakpointTable::EditIgnoreCount
	(
	const CMBreakpoint* bp
	)
{
	JIndex i;
	if (FindBreakpointByDebuggerIndex(bp, &i))
		{
		itsDir->Activate();
		GetWindow()->RequestFocus();
		BeginEditing(JPoint(kIgnoreCountColumn, i));
		}
}

/******************************************************************************
 EditCondition

 ******************************************************************************/

void
CMBreakpointTable::EditCondition
	(
	const CMBreakpoint* bp
	)
{
	JIndex i;
	if (FindBreakpointByDebuggerIndex(bp, &i))
		{
		itsDir->Activate();
		GetWindow()->RequestFocus();
		BeginEditing(JPoint(kConditionColumn, i));
		}
}

/******************************************************************************
 FindBreakpointByDebuggerIndex (private)

 ******************************************************************************/

JBoolean
CMBreakpointTable::FindBreakpointByDebuggerIndex
	(
	const CMBreakpoint*	bp,
	JIndex*				index
	)
	const
{
	const JSize count = itsBPList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CMBreakpoint* b = itsBPList->NthElement(i);
		if (b->GetDebuggerIndex() == bp->GetDebuggerIndex())
			{
			*index = i;
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}

/******************************************************************************
 Receive (virtual protected)

	Listen for changes in our JTableData and JAuxTableData objects.

 ******************************************************************************/

void
CMBreakpointTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == &(GetTableSelection()) && message.Is(JTableData::kRectChanged))
		{
		if ((GetTableSelection()).GetSingleSelectedCell(&itsSelectedCell))
			{
			itsSelectedCell.y =
				(itsBPList->NthElement(itsSelectedCell.y))->GetDebuggerIndex();
			}
		}

	JXEditTable::Receive(sender, message);
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

inline JUnsignedOffset
cmFileNameOffset
	(
	const CMBreakpoint* bp
	)
{
	JIndex i;
	(bp->GetFileName()).LocateLastSubstring(ACE_DIRECTORY_SEPARATOR_STR, &i);
	return i;
}

void
CMBreakpointTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JPoint editCell;
	if (GetEditedCell(&editCell) && cell == editCell)
		{
		return;
		}

	HilightIfSelected(p, cell, rect);

	const CMBreakpoint* bp = itsBPList->NthElement(cell.y);
	if (cell.x == kStatusColumn)
		{
		JRect r = rect;
		r.Shrink(kHMarginWidth, kHMarginWidth);

		if (bp->GetLineNumber() > 0)
			{
			CMLineIndexTable::DrawBreakpoint(bp, p, GetColormap(), r);
			}
		else
			{
			JPolygon poly;
			poly.AppendElement(JPoint(r.topLeft()));
			poly.AppendElement(JPoint(r.topRight()));
			poly.AppendElement(JPoint(r.xcenter(), r.bottom));

			JXColormap* cmap        = GetColormap();
			const JColorIndex color = bp->IsEnabled() ? cmap->GetRedColor() : cmap->GetGreenColor();

			p.SetPenColor(color);
			p.SetFilling(kJTrue);
			p.Polygon(poly);

			if (bp->GetAction() != CMBreakpoint::kRemoveBreakpoint)
				{
				p.SetPenColor(GetColormap()->GetBlackColor());
				p.SetFilling(kJFalse);
				p.Polygon(poly);
				}
			}
		}
	else
		{
		JString s;
		JPainter::HAlignment hAlign = JPainter::kHAlignLeft;
		if (cell.x == kFileNameColumn)
			{
			s = bp->GetFileName().GetCString() + cmFileNameOffset(bp);
			}
		else if (cell.x == kLineNumberColumn)
			{
			const JSize line = bp->GetLineNumber();
			if (line > 0)
				{
				s      = JString(line, JString::kBase10);
				hAlign = JPainter::kHAlignRight;
				}
			}
		else if (cell.x == kFunctionColumn)
			{
			s = bp->GetFunctionName();
			}
		else if (cell.x == kAddressColumn)
			{
			s = bp->GetAddress();
			}
		else if (cell.x == kIgnoreCountColumn)
			{
			s      = JString(bp->GetIgnoreCount(), JString::kBase10);
			hAlign = JPainter::kHAlignRight;
			}
		else if (cell.x == kConditionColumn)
			{
			bp->GetCondition(&s);
			}

		p.SetFont(itsFont);

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, s, hAlign, JPainter::kVAlignCenter);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CMBreakpointTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (ScrollForWheel(button, modifiers) ||
		!GetCell(pt, &cell))
		{
		return;
		}
	else if (button == kJXLeftButton)
		{
		if (cell.x != kStatusColumn)
			{
			SelectSingleCell(cell);
			}

		CMBreakpoint* bp = itsBPList->NthElement(cell.y);
		if (cell.x == kStatusColumn)
			{
			(CMGetLink())->SetBreakpointEnabled(bp->GetDebuggerIndex(), !bp->IsEnabled());
			}
		else if (clickCount == 2 &&
				 (cell.x == kFileNameColumn || cell.x == kLineNumberColumn))
			{
			if (bp->GetLineNumber() > 0)
				{
				(itsDir->GetCommandDirector())->OpenSourceFile(bp->GetFileName(), bp->GetLineNumber());
				}
			}
		else if (clickCount == 2 &&
				 (cell.x == kFunctionColumn || cell.x == kAddressColumn))
			{
			if (!bp->GetFunctionName().IsEmpty())
				{
				(itsDir->GetCommandDirector())->DisassembleFunction(bp->GetFunctionName(), bp->GetAddress());
				}
			}
		else if (clickCount == 2)
			{
			BeginEditing(cell);
			}
		}
}

/******************************************************************************
 HandleKeyPress (virtual)

	Backspace/Delete:  clear menu text, menu shortcut, name

 ******************************************************************************/

void
CMBreakpointTable::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (IsEditing())
		{
		JXEditTable::HandleKeyPress(key, modifiers);
		}
	else if (key == kJDeleteKey || key == kJForwardDeleteKey)
		{
		JTableSelection& s = GetTableSelection();
		JPoint cell;
		if (s.GetSingleSelectedCell(&cell))
			{
			CMBreakpoint* bp = itsBPList->NthElement(cell.y);
			(CMGetLink())->RemoveBreakpoint(*bp);
			}
		}
	else if (key == kJReturnKey)
		{
		JTableSelection& s = GetTableSelection();
		JPoint cell;
		if (!s.GetSingleSelectedCell(&cell))
			{
			// do nothing
			}
		else if (cell.x == kStatusColumn)
			{
			CMBreakpoint* bp = itsBPList->NthElement(cell.y);
			(CMGetLink())->SetBreakpointEnabled(bp->GetDebuggerIndex(), !bp->IsEnabled());
			}
		else if (cell.x == kFileNameColumn || cell.x == kLineNumberColumn)
			{
			CMBreakpoint* bp = itsBPList->NthElement(cell.y);
			if (bp->GetLineNumber() > 0)
				{
				(itsDir->GetCommandDirector())->OpenSourceFile(bp->GetFileName(), bp->GetLineNumber());
				}
			}
		else if (cell.x == kFunctionColumn || cell.x == kAddressColumn)
			{
			CMBreakpoint* bp = itsBPList->NthElement(cell.y);
			if (!bp->GetFunctionName().IsEmpty())
				{
				(itsDir->GetCommandDirector())->DisassembleFunction(bp->GetFunctionName(), bp->GetAddress());
				}
			}
		else if (cell.x == kIgnoreCountColumn || cell.x == kConditionColumn)
			{
			BeginEditing(cell);
			}
		}
	else
		{
		HandleSelectionKeyPress(key, modifiers);
		}
}

/******************************************************************************
 IsEditable (virtual)

 ******************************************************************************/

JBoolean
CMBreakpointTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	CMBreakpoint* bp = itsBPList->NthElement(cell.y);
	return JI2B( cell.x == kIgnoreCountColumn ||
				 (cell.x == kConditionColumn && bp->GetLineNumber() > 0) );
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
CMBreakpointTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsTextInput == NULL );

	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectCell(cell);

	CMBreakpoint* bp = itsBPList->NthElement(cell.y);
	JString text;
	if (cell.x == kIgnoreCountColumn)
		{
		JXIntegerInput* input = jnew JXIntegerInput(this, kFixedLeft, kFixedTop, x,y, w,h);
		assert( input != NULL );
		input->SetLowerLimit(0);
		input->SetValue(bp->GetIgnoreCount());

		itsTextInput = input;
		}
	else if (cell.x == kConditionColumn)
		{
		bp->GetCondition(&text);
		}

	if (itsTextInput == NULL)
		{
		itsTextInput = jnew JXInputField(this, kFixedLeft, kFixedTop, x,y, w,h);
		assert( itsTextInput != NULL );
		itsTextInput->SetText(text);
		}

	itsTextInput->SetFont(itsFont);
	return itsTextInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
CMBreakpointTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsTextInput != NULL );

	if (!itsTextInput->InputValid())
		{
		return kJFalse;
		}

	CMBreakpoint* bp = itsBPList->NthElement(cell.y);
	if (cell.x == kIgnoreCountColumn)
		{
		JInteger count;
		const JBoolean ok = dynamic_cast<JXIntegerInput*>(itsTextInput)->GetValue(&count);
		assert( ok );
		if (((JSize) count) != bp->GetIgnoreCount())
			{
			(CMGetLink())->SetBreakpointIgnoreCount(bp->GetDebuggerIndex(), count);
			}
		return kJTrue;
		}
	else if (cell.x == kConditionColumn)
		{
		const JString& s = itsTextInput->GetText();

		JString cond;
		const JBoolean hasCondition = bp->GetCondition(&cond);
		if (s != cond)
			{
			(CMGetLink())->SetBreakpointCondition(bp->GetDebuggerIndex(), s);
			}
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
CMBreakpointTable::PrepareDeleteXInputField()
{
	itsTextInput = NULL;
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
CMBreakpointTable::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentGeometryDataVersion)
		{
		JCoordinate w;
		input >> w;
		SetColWidth(kStatusColumn, w);
		input >> w;
		SetColWidth(kFileNameColumn, w);
		input >> w;
		SetColWidth(kLineNumberColumn, w);
		input >> w;
		SetColWidth(kFunctionColumn, w);
		input >> w;
		SetColWidth(kAddressColumn, w);
		input >> w;
		SetColWidth(kIgnoreCountColumn, w);
		input >> w;
		SetColWidth(kConditionColumn, w);
		}

	JIgnoreUntil(input, kGeometryDataEndDelimiter);
}

/******************************************************************************
 WritePrefs

 ******************************************************************************/

void
CMBreakpointTable::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentGeometryDataVersion;
	output << ' ' << GetColWidth(kStatusColumn);
	output << ' ' << GetColWidth(kFileNameColumn);
	output << ' ' << GetColWidth(kLineNumberColumn);
	output << ' ' << GetColWidth(kFunctionColumn);
	output << ' ' << GetColWidth(kAddressColumn);
	output << ' ' << GetColWidth(kIgnoreCountColumn);
	output << ' ' << GetColWidth(kConditionColumn);
	output << kGeometryDataEndDelimiter;
}

/******************************************************************************
 SetColTitles

 ******************************************************************************/

void
CMBreakpointTable::SetColTitles
	(
	JXColHeaderWidget* widget
	)
	const
{
	for (JIndex i=1; i<=kColCount; i++)
		{
		widget->SetColTitle(i, kColTitle[i-1]);
		}
}

/******************************************************************************
 CompareBreakpointLocations (static)

 ******************************************************************************/

JOrderedSetT::CompareResult
CMBreakpointTable::CompareBreakpointLocations
	(
	CMBreakpoint* const & bp1,
	CMBreakpoint* const & bp2
	)
{
	int r = JStringCompare(
		bp1->GetFileName().GetCString() + cmFileNameOffset(bp1),
		bp2->GetFileName().GetCString() + cmFileNameOffset(bp2), kJFalse);
	if (r > 0)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (r < 0)
		{
		return JOrderedSetT::kFirstLessSecond;
		}

	JOrderedSetT::CompareResult r1 =
		JCompareIndices(bp1->GetLineNumber(), bp2->GetLineNumber());

	if (r1 == JOrderedSetT::kFirstEqualSecond)
		{
		r1 = JCompareStringsCaseInsensitive(
				const_cast<JString*>(&(bp1->GetFunctionName())),
				const_cast<JString*>(&(bp2->GetFunctionName())));
		}

	if (r1 == JOrderedSetT::kFirstEqualSecond)
		{
		JString c1, c2;
		bp1->GetCondition(&c1);
		bp2->GetCondition(&c2);

		r1 = JCompareStringsCaseInsensitive(&c1, &c2);
		}

	return r1;
}
