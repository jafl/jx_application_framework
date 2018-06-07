/******************************************************************************
 JXSpellList.cc

	BASE CLASS = JXStringList

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include <JXSpellList.h>
#include <JXColorManager.h>
#include <jXGlobals.h>
#include <JTableSelection.h>
#include <jAssert.h>

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
 HandleMouseDown

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

		const JString& word = *((GetStringList()).GetElement(cell.y));

		JBroadcaster::Message* msg = nullptr;
		if (clickCount == 1)
			{
			msg = jnew WordSelected(word);
			}
		else if (clickCount == 2 && modifiers.meta())
			{
			msg = jnew ReplaceWordAll(word);
			}
		else if (clickCount == 2)
			{
			msg = jnew ReplaceWord(word);
			}

		if (msg != nullptr)
			{
			Broadcast(*msg);
			jdelete msg;
			}
		}
}
