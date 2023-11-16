/******************************************************************************
 JXSpellList.cpp

	BASE CLASS = JXStringList

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "JXSpellList.h"
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

// JBroadcaster messages

const JUtf8Byte* JXSpellList::kWordSelected   = "WordSelected::JXSpellList";
const JUtf8Byte* JXSpellList::kReplaceWord    = "ReplaceWord::JXSpellList";
const JUtf8Byte* JXSpellList::kReplaceWordAll = "ReplaceWordAll::JXSpellList";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSpellList::JXSpellList
	(
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXStringList(scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSpellList::~JXSpellList()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXSpellList::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JTableSelection& s = GetTableSelection();

	JPoint cell;
	if (button > kJXRightButton)
	{
		ScrollForWheel(button, modifiers);
	}
	else if (!GetCell(pt, &cell))
	{
		s.ClearSelection();
	}
	else if (button == kJXLeftButton)
	{
		s.ClearSelection();
		s.SelectCell(cell);

		const JString& word = *GetStringList().GetItem(cell.y);

		if (clickCount == 1)
		{
			Broadcast(WordSelected(word));
		}
		else if (clickCount == 2 && modifiers.meta())
		{
			Broadcast(ReplaceWordAll(word));
		}
		else if (clickCount == 2)
		{
			Broadcast(ReplaceWord(word));
		}
	}
}
