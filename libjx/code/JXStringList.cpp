/******************************************************************************
 JXStringList.cpp

	Maintains a column of strings stored in a JPtrArray<JString>.
	This is very different from a JStringTable which requires a JTableData
	object.

	Note that we can only receive notification when the elements of the
	JPtrArray change.  i.e. When the JString pointer changes, -not- when the
	JString data changes.

	Since recalculating the required width after every change would take
	too much time, we update the width whenever we encounter a string
	that is wider than our current width.

	BASE CLASS = JXTable

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXStringList.h>
#include <JXWindow.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <JStyleTableData.h>
#include <JTableSelection.h>
#include <JPainter.h>
#include <JFontManager.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStringList::JXStringList
	(
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
	JXTable(1,w, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsList        = NULL;
	itsSortedList  = NULL;
	itsMinColWidth = 1;

	itsStyles = new JStyleTableData(this, GetFontManager(), GetColormap());
	assert( itsStyles != NULL );

	JXColormap* colormap         = GetColormap();
	const JColorIndex blackColor = colormap->GetBlackColor();
	SetRowBorderInfo(0, blackColor);
	SetColBorderInfo(0, blackColor);

	AppendCols(1);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStringList::~JXStringList()
{
	delete itsStyles;
	delete itsSortedList;
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
JXStringList::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	HilightIfSelected(p, cell, rect);

	JSize fontSize;
	const JString& fontName = GetFont(&fontSize);
	p.SetFont(fontName, fontSize, itsStyles->GetCellStyle(cell));

	const JString* str = itsList->NthElement(cell.y);

	// check that column is wide enough
	// (Not assuming rect == cell_rect allows icons in derived classes.)

	const JCoordinate w  = p.GetStringWidth(*str) + 2*kHMarginWidth;
	const JCoordinate dw = rect.left - GetColLeft(1);
	if (w > GetColWidth(1) - dw)
		{
		itsMinColWidth = w + dw;
		AdjustColWidth();
		}

	// draw string

	JRect r = rect;
	r.left += kHMarginWidth;
	p.String(r, *str, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
}

/******************************************************************************
 SetStringList

 ******************************************************************************/

void
JXStringList::SetStringList
	(
	const JPtrArray<JString>* list
	)
{
	if (itsList != NULL)
		{
		StopListening(itsList);

		delete itsSortedList;
		itsSortedList = NULL;
		}

	itsList = list;

	itsMinColWidth = 1;
	if (itsList != NULL)
		{
		ListenTo(itsList);

		const JSize strCount = itsList->GetElementCount();
		if (strCount > GetRowCount())
			{
			AppendRows(strCount - GetRowCount());
			}
		else if (strCount < GetRowCount())
			{
			RemoveNextRows(1, GetRowCount() - strCount);
			}

		itsSortedList =
			new JAliasArray<JString*>(const_cast<JPtrArray<JString>*>(itsList),
									  JCompareStringsCaseInsensitive,
									  JOrderedSetT::kSortAscending);
		assert( itsSortedList != NULL );
		}
	else
		{
		RemoveAllRows();
		}
}

/******************************************************************************
 GetFont

 ******************************************************************************/

const JString&
JXStringList::GetFont
	(
	JSize* size
	)
	const
{
	return itsStyles->GetFont(size);
}

/******************************************************************************
 SetFont

 ******************************************************************************/

void
JXStringList::SetFont
	(
	const JCharacter*	name,
	const JSize			size
	)
{
	itsMinColWidth = 1;
	itsStyles->SetFont(name, size);
}

/******************************************************************************
 GetStyle

 ******************************************************************************/

JFontStyle
JXStringList::GetStyle
	(
	const JIndex index
	)
	const
{
	return itsStyles->GetCellStyle(JPoint(1, index));
}

/******************************************************************************
 SetStyle

 ******************************************************************************/

void
JXStringList::SetStyle
	(
	const JIndex		index,
	const JFontStyle&	style
	)
{
	itsStyles->SetCellStyle(JPoint(1, index), style);
}

/******************************************************************************
 SetStyles

	The given list must have the same length as the table.

 ******************************************************************************/

void
JXStringList::SetStyles
	(
	const JRunArray<JFontStyle>& styleList
	)
{
	const JSize count = GetRowCount();
	assert( styleList.GetElementCount() == count );

	for (JIndex i=1; i<=count; i++)
		{
		itsStyles->SetCellStyle(JPoint(1,i), styleList.GetElement(i));
		}
}

/******************************************************************************
 SetAllStyles

 ******************************************************************************/

void
JXStringList::SetAllStyles
	(
	const JFontStyle& style
	)
{
	itsStyles->SetAllCellStyles(style);
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JXStringList::HandleFocusEvent()
{
	JXTable::HandleFocusEvent();
	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXStringList::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == ' ' || (key == kJEscapeKey && !IsEditing()))
		{
		ClearIncrementalSearchBuffer();
		(GetTableSelection()).ClearSelection();
		}

	// incremental search

	else if (JXIsPrint(key) && !modifiers.control() && !modifiers.meta())
		{
		itsKeyBuffer.AppendCharacter(key);

		JIndex index;
		if (ClosestMatch(itsKeyBuffer, &index))
			{
			const JString saveBuffer = itsKeyBuffer;
			SelectSingleCell(JPoint(1, index));
			itsKeyBuffer = saveBuffer;
			}
		else
			{
			(GetTableSelection()).ClearSelection();
			}
		}

	else
		{
		if (0 < key && key <= 255)
			{
			ClearIncrementalSearchBuffer();
			}
		JXTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 ClearIncrementalSearchBuffer

 ******************************************************************************/

void
JXStringList::ClearIncrementalSearchBuffer()
{
	itsKeyBuffer.Clear();
}

/******************************************************************************
 ClosestMatch (private)

	Returns the index of the closest match for the given prefix.

 ******************************************************************************/

JBoolean
JXStringList::ClosestMatch
	(
	const JString&	prefixStr,
	JIndex*			index
	)
	const
{
	if (itsSortedList == NULL)
		{
		*index = 0;
		return kJFalse;
		}

	JBoolean found;
	*index = itsSortedList->SearchSorted1(const_cast<JString*>(&prefixStr),
										  JOrderedSetT::kFirstMatch, &found);
	if (*index > GetRowCount())		// insert beyond end of list
		{
		*index = GetRowCount();
		}
	return JConvertToBoolean( *index > 0 );
}

/******************************************************************************
 Receive (virtual protected)

	Listen for changes in our list.

 ******************************************************************************/

void
JXStringList::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	// clear search buffer if list changed in any way

	if (sender == const_cast<JPtrArray<JString>*>(itsList))
		{
		ClearIncrementalSearchBuffer();
		}

	// then check how the list changed

	if (sender == const_cast<JPtrArray<JString>*>(itsList) &&
		message.Is(JOrderedSetT::kElementsInserted))
		{
		const JOrderedSetT::ElementsInserted* info =
			dynamic_cast(const JOrderedSetT::ElementsInserted*, &message);
		assert( info != NULL );
		InsertRows(info->GetFirstIndex(), info->GetCount());
		}

	else if (sender == const_cast<JPtrArray<JString>*>(itsList) &&
			 message.Is(JOrderedSetT::kElementsRemoved))
		{
		const JOrderedSetT::ElementsRemoved* info =
			dynamic_cast(const JOrderedSetT::ElementsRemoved*, &message);
		assert( info != NULL );
		itsMinColWidth = 1;
		RemoveNextRows(info->GetFirstIndex(), info->GetCount());
		}

	else if (sender == const_cast<JPtrArray<JString>*>(itsList) &&
			 message.Is(JOrderedSetT::kElementMoved))
		{
		const JOrderedSetT::ElementMoved* info =
			dynamic_cast(const JOrderedSetT::ElementMoved*, &message);
		assert( info != NULL );
		MoveRow(info->GetOrigIndex(), info->GetNewIndex());
		}

	else if (sender == const_cast<JPtrArray<JString>*>(itsList) &&
			 message.Is(JOrderedSetT::kElementsSwapped))
		{
		const JOrderedSetT::ElementsSwapped* info =
			dynamic_cast(const JOrderedSetT::ElementsSwapped*, &message);
		assert( info != NULL );
		const JFontStyle s1 = itsStyles->GetElement(info->GetIndex1(), 1);
		itsStyles->SetElement(info->GetIndex1(), 1, itsStyles->GetElement(info->GetIndex2(), 1));
		itsStyles->SetElement(info->GetIndex2(), 1, s1);
		}

	else if (sender == const_cast<JPtrArray<JString>*>(itsList) &&
			 message.Is(JOrderedSetT::kSorted))
		{
		assert( 0 );	// we don't allow this
		}

	else if (sender == const_cast<JPtrArray<JString>*>(itsList) &&
			 message.Is(JOrderedSetT::kElementChanged))
		{
		itsMinColWidth = 1;
		Refresh();
		}

	else
		{
		JXTable::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

 ******************************************************************************/

void
JXStringList::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == const_cast<JPtrArray<JString>*>(itsList))
		{
		SetStringList(NULL);
		}
	else
		{
		JXTable::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXStringList::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXTable::ApertureResized(dw,dh);
	AdjustColWidth();
}

/******************************************************************************
 AdjustColWidth (private)

 ******************************************************************************/

void
JXStringList::AdjustColWidth()
{
	JCoordinate w = GetApertureWidth();
	if (w < itsMinColWidth)
		{
		w = itsMinColWidth;
		}
	SetColWidth(1,w);
}
