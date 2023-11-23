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

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXStringList.h"
#include "JXWindow.h"
#include "JXColorManager.h"
#include "jXGlobals.h"
#include <jx-af/jcore/JStyleTableData.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kHMarginWidth = 3;

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
	itsList        = nullptr;
	itsSortedList  = nullptr;
	itsMinColWidth = 1;

	itsStyles = jnew JStyleTableData(this, GetFontManager());
	assert( itsStyles != nullptr );

	const JColorID blackColor = JColorManager::GetBlackColor();
	SetRowBorderInfo(0, blackColor);
	SetColBorderInfo(0, blackColor);

	AppendCols(1);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStringList::~JXStringList()
{
	jdelete itsStyles;
	jdelete itsSortedList;
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

	JFont font = GetFont();
	font.SetStyle(itsStyles->GetCellStyle(cell));
	p.SetFont(font);

	const JString* str = itsList->GetItem(cell.y);

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
	p.String(r, *str, JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);
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
	if (itsList != nullptr)
	{
		StopListening(itsList);

		jdelete itsSortedList;
		itsSortedList = nullptr;
	}

	itsList = list;

	itsMinColWidth = 1;
	if (itsList != nullptr)
	{
		ListenTo(itsList);

		const JSize strCount = itsList->GetItemCount();
		if (strCount > GetRowCount())
		{
			AppendRows(strCount - GetRowCount());
		}
		else if (strCount < GetRowCount())
		{
			RemoveNextRows(1, GetRowCount() - strCount);
		}

		itsSortedList =
			jnew JAliasArray<JString*>(const_cast<JPtrArray<JString>*>(itsList),
									  JCompareStringsCaseInsensitive,
									  JListT::kSortAscending);
		assert( itsSortedList != nullptr );
	}
	else
	{
		RemoveAllRows();
	}
}

/******************************************************************************
 GetFont

 ******************************************************************************/

const JFont&
JXStringList::GetFont()
	const
{
	return itsStyles->GetFont();
}

/******************************************************************************
 SetFont

 ******************************************************************************/

void
JXStringList::SetFont
	(
	const JString&	name,
	const JSize		size
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
	assert( styleList.GetItemCount() == count );

	JRunArrayIterator iter(styleList);

	JFontStyle style;
	JIndex i = 1;
	while (iter.Next(&style))
	{
		itsStyles->SetCellStyle(JPoint(1,i), style);
		i++;
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
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == ' ' || (c == kJEscapeKey && !IsEditing()))
	{
		ClearIncrementalSearchBuffer();
		GetTableSelection().ClearSelection();
	}

	// incremental search

	else if (c.IsPrint() && !modifiers.control() && !modifiers.meta())
	{
		itsKeyBuffer.Append(c);

		JIndex index;
		if (ClosestMatch(itsKeyBuffer, &index))
		{
			const JString saveBuffer = itsKeyBuffer;
			SelectSingleCell(JPoint(1, index));
			itsKeyBuffer = saveBuffer;
		}
		else
		{
			GetTableSelection().ClearSelection();
		}
	}

	else
	{
		if (!c.IsBlank())
		{
			ClearIncrementalSearchBuffer();
		}
		JXTable::HandleKeyPress(c, keySym, modifiers);
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

bool
JXStringList::ClosestMatch
	(
	const JString&	prefixStr,
	JIndex*			index
	)
	const
{
	if (itsSortedList == nullptr)
	{
		*index = 0;
		return false;
	}

	bool found;
	*index = itsSortedList->SearchSortedOTI(const_cast<JString*>(&prefixStr),
										  JListT::kFirstMatch, &found);
	if (*index > GetRowCount())		// insert beyond end of list
	{
		*index = GetRowCount();
	}
	return *index > 0;
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
		message.Is(JListT::kItemsInserted))
	{
		const auto* info =
			dynamic_cast<const JListT::ItemsInserted*>(&message);
		assert( info != nullptr );
		InsertRows(info->GetFirstIndex(), info->GetCount());
	}

	else if (sender == const_cast<JPtrArray<JString>*>(itsList) &&
			 message.Is(JListT::kItemsRemoved))
	{
		const auto* info =
			dynamic_cast<const JListT::ItemsRemoved*>(&message);
		assert( info != nullptr );
		itsMinColWidth = 1;
		RemoveNextRows(info->GetFirstIndex(), info->GetCount());
	}

	else if (sender == const_cast<JPtrArray<JString>*>(itsList) &&
			 message.Is(JListT::kItemMoved))
	{
		const auto* info =
			dynamic_cast<const JListT::ItemMoved*>(&message);
		assert( info != nullptr );
		MoveRow(info->GetOrigIndex(), info->GetNewIndex());
	}

	else if (sender == const_cast<JPtrArray<JString>*>(itsList) &&
			 message.Is(JListT::kItemsSwapped))
	{
		const auto* info =
			dynamic_cast<const JListT::ItemsSwapped*>(&message);
		assert( info != nullptr );
		const JFontStyle s1 = itsStyles->GetItem(info->GetIndex1(), 1);
		itsStyles->SetItem(info->GetIndex1(), 1, itsStyles->GetItem(info->GetIndex2(), 1));
		itsStyles->SetItem(info->GetIndex2(), 1, s1);
	}

	else if (sender == const_cast<JPtrArray<JString>*>(itsList) &&
			 message.Is(JListT::kSorted))
	{
		assert( 0 );	// we don't allow this
	}

	else if (sender == const_cast<JPtrArray<JString>*>(itsList) &&
			 message.Is(JListT::kItemsChanged))
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
		SetStringList(nullptr);
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
