/******************************************************************************
 JStringIterator.cpp

						The JString Iterator Class

	UTF-8 combines the worst of arrays and linked lists.  Insertion and
	search are both O(N), and iteration is complicated by characters not
	being a fixed number of bytes.

	This iterator provides the most efficient, error-resistant way to loop
	through the UTF-8 characters in a string:

		JString s;
		...
		JStringIterator iterator(s);
		JUtf8Character  char;

		while(iterator.Next(&char))
			{
			<do something with the character>
			}

	An iterator is robust in the face of insertions and deletions to
	the JString it is working on:

	Each iterator has a current position called a "cursor". The cursor is
	always positioned at the start of the JString, at the end of the
	JString, or *between* two characters. The cursor position is an ordinal
	from 0 to n, where n is the number of characters in the JString.  The
	relationship of the cursor position to the JString index is illustrated
	below:

				+-----+-----+-----+-----+-----+-----+
		index	|  1  |  2  |  3  |  4  |  5  |  6  |
				+-----+-----+-----+-----+-----+-----+
		cursor	0     1     2     3     4     5     6

	JIteratorPosition lets you specify an initial (or changed) cursor position.
	kJIteratorStartBefore puts the cursor just before the specified item
	(so Next() will fetch the item) while kJIteratorStartAfter puts it just after
	(so Prev() will fetch the item).

	The operations Next() and Prev() fetch the item just after or the item
	just before the cursor, respectively, then increment or decrement the
	cursor. So a sequence of Next() calls will advance forward through a
	JString, while a sequence of Prev() calls will go backwards through the
	JString. You can change direction at any time.

	If the cursor is positioned at the number of characters in the JString
	and you call Next(), it will return FALSE. Likewise if the cursor is
	zero and you call Prev().

	If the JString is deleted while the cursor is active, all further calls
	to Next() or Prev() will return FALSE and the cursor position will be
	fixed at 0.

	Implementation notes:

	Since each iterator is used by exactly one JString, we can construct a
	linked list of iterators headed in the JString. In order not to
	transfer the overhead of managing this list into JString, the iterators
	manage the list.  Thus the iterator must be a friend of the JString.
	This is acceptable because the iterator can't exist without the
	JString.

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JStringIterator.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

template <class T>
JStringIterator<T>::JStringIterator
	(
	const JOrderedSet<T>&	theOrderedSet,
	const JIteratorPosition	start,
	const JIndex			index
	)
{
	itsConstOrderedSet = &theOrderedSet;
	itsOrderedSet      = NULL;

	JStringIteratorX(start, index);
}

template <class T>
JStringIterator<T>::JStringIterator
	(
	JOrderedSet<T>*			theOrderedSet,
	const JIteratorPosition	start,
	const JIndex			index
	)
{
	itsConstOrderedSet = theOrderedSet;
	itsOrderedSet      = theOrderedSet;

	JStringIteratorX(start, index);
}

// private

template <class T>
void
JStringIterator<T>::JStringIteratorX
	(
	const JIteratorPosition	start,
	const JIndex			index
	)
{
	itsNextIterator = NULL;
	AddToIteratorList();

	MoveTo(start, index);
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

template <class T>
JStringIterator<T>::JStringIterator
	(
	const JStringIterator<T>& source
	)
{
	itsConstOrderedSet = source.itsConstOrderedSet;
	itsOrderedSet      = source.itsOrderedSet;
	itsCursorPosition  = source.itsCursorPosition;
	itsNextIterator    = NULL;

	AddToIteratorList();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

template <class T>
JStringIterator<T>::~JStringIterator()
{
	RemoveFromIteratorList();
}

/******************************************************************************
 Assignment operator (private)

	operator= is private because it is not allowed

 ******************************************************************************/

template <class T>
const JStringIterator<T>&
JStringIterator<T>::operator=
	(
	const JStringIterator<T>& source
	)
{
	assert( 0 );
	return *this;
}

/******************************************************************************
 Prev (virtual)

	Returns kJTrue if there is a previous item, fetching the previous
	item in the list and decrementing the iterator position.
	Otherwise returns kJFalse without fetching or decrementing.

 ******************************************************************************/

template <class T>
JBoolean
JStringIterator<T>::Prev
	(
	T* data
	)
{
	if (AtBeginning())
		{
		return kJFalse;
		}

	*data = itsConstOrderedSet->GetElement(itsCursorPosition);
	itsCursorPosition--;

	return kJTrue;
}

/******************************************************************************
 Next (virtual)

	Returns kJTrue if there is a next item, fetching the next
	item in the list and incrementing the iterator position.
	Otherwise returns kJFalse without fetching or incrementing.

 ******************************************************************************/

template <class T>
JBoolean
JStringIterator<T>::Next
	(
	T* data
	)
{
	if (AtEnd())
		{
		return kJFalse;
		}

	itsCursorPosition++;
	*data = itsConstOrderedSet->GetElement(itsCursorPosition);

	return kJTrue;
}

/******************************************************************************
 SkipPrev (virtual)

 ******************************************************************************/

template <class T>
void
JStringIterator<T>::SkipPrev
	(
	const JSize count
	)
{
	if (count <= itsCursorPosition)
		{
		itsCursorPosition -= count;
		}
	else
		{
		itsCursorPosition = 0;
		}
}

/******************************************************************************
 SkipNext (virtual)

 ******************************************************************************/

template <class T>
void
JStringIterator<T>::SkipNext
	(
	const JSize count
	)
{
	if (!AtEnd())
		{
		itsCursorPosition += count;

		const JSize maxCount = itsConstOrderedSet->GetElementCount();
		if (itsCursorPosition > maxCount)
			{
			itsCursorPosition = maxCount;
			}
		}
}

/******************************************************************************
 SetPrev (virtual)

	Returns kJTrue if we have a non-const JOrderedSet<T>* and
	there is a previous item.

 ******************************************************************************/

template <class T>
JBoolean
JStringIterator<T>::SetPrev
	(
	const T& data
	)
{
	if (itsOrderedSet == NULL || AtBeginning())
		{
		return kJFalse;
		}

	itsOrderedSet->SetElement(itsCursorPosition, data);
	return kJTrue;
}

/******************************************************************************
 SetNext (virtual)

	Returns kJTrue if we have a non-const JOrderedSet<T>* and
	there is a next item.

 ******************************************************************************/

template <class T>
JBoolean
JStringIterator<T>::SetNext
	(
	const T& data
	)
{
	if (itsOrderedSet == NULL || AtEnd())
		{
		return kJFalse;
		}

	itsOrderedSet->SetElement(itsCursorPosition+1, data);
	return kJTrue;
}

/******************************************************************************
 RemovePrev (virtual)

	Returns kJTrue if we have a non-const JOrderedSet<T>* and
	there is a previous item.

 ******************************************************************************/

template <class T>
JBoolean
JStringIterator<T>::RemovePrev()
{
	if (itsOrderedSet == NULL || AtBeginning())
		{
		return kJFalse;
		}

	itsOrderedSet->RemoveElement(itsCursorPosition);
	return kJTrue;
}

/******************************************************************************
 RemoveNext (virtual)

	Returns kJTrue if we have a non-const JOrderedSet<T>* and
	there is a next item.

 ******************************************************************************/

template <class T>
JBoolean
JStringIterator<T>::RemoveNext()
{
	if (itsOrderedSet == NULL || AtEnd())
		{
		return kJFalse;
		}

	itsOrderedSet->RemoveElement(itsCursorPosition+1);
	return kJTrue;
}

/******************************************************************************
 MoveTo (virtual)

 ******************************************************************************/

template <class T>
void
JStringIterator<T>::MoveTo
	(
	const JIteratorPosition	newPosition,
	const JIndex			index
	)
{
	if (itsConstOrderedSet == NULL)
		{
		return;
		}

	if (newPosition == kJIteratorStartAtBeginning)
		{
		itsCursorPosition = 0;
		}
	else if (newPosition == kJIteratorStartAtEnd)
		{
		itsCursorPosition = itsConstOrderedSet->GetElementCount();
		}
	else if (newPosition == kJIteratorStartBefore)
		{
		assert( itsConstOrderedSet->IndexValid(index) );
		itsCursorPosition = index-1;
		}
	else
		{
		assert( newPosition == kJIteratorStartAfter );
		assert( itsConstOrderedSet->IndexValid(index) );
		itsCursorPosition = index;
		}
}

/******************************************************************************
 AtBeginning

	Return kJTrue if iterator is positioned at the beginning of the ordered set
	or if the ordered set has been deleted.

 ******************************************************************************/

template <class T>
JBoolean
JStringIterator<T>::AtBeginning()
{
	return JConvertToBoolean( itsConstOrderedSet == NULL || itsCursorPosition == 0 );
}

/******************************************************************************
 AtEnd

	Return kJTrue if iterator is positioned at the end of the ordered set
	or if the ordered set has been deleted.

 ******************************************************************************/

template <class T>
JBoolean
JStringIterator<T>::AtEnd()
{
	return JConvertToBoolean( itsConstOrderedSet == NULL ||
							  itsCursorPosition >= itsConstOrderedSet->GetElementCount() );
}

/******************************************************************************
 GetOrderedSet (protected)

	Return the current ordered set for this iterator.

 ******************************************************************************/

template <class T>
const JOrderedSet<T>*
JStringIterator<T>::GetConstOrderedSet()
	const
{
	return itsConstOrderedSet;
}

template <class T>
JBoolean
JStringIterator<T>::GetOrderedSet
	(
	JOrderedSet<T>** obj
	)
	const
{
	*obj = itsOrderedSet;
	return JConvertToBoolean( itsOrderedSet != NULL );
}

/******************************************************************************
 GetCursor (protected)

	Return the current cursor position.

 ******************************************************************************/

template <class T>
JCursorPosition
JStringIterator<T>::GetCursor()
	const
{
	return itsCursorPosition;
}

/******************************************************************************
 SetCursor (protected)

	Set the current cursor position.

 ******************************************************************************/

template <class T>
void
JStringIterator<T>::SetCursor
	(
	const JCursorPosition pos
	)
{
	itsCursorPosition = pos;
}

/******************************************************************************
 AddToIteratorList (private)

	Add this iterator at the front of the active iterator list.

 ******************************************************************************/

template <class T>
void
JStringIterator<T>::AddToIteratorList()
{
	if (itsConstOrderedSet != NULL)
		{
		JOrderedSet<T>* set   = const_cast<JOrderedSet<T>*>(itsConstOrderedSet);
		itsNextIterator       = set->itsFirstIterator;
		set->itsFirstIterator = this;
		}
}

/******************************************************************************
 RemoveFromIteratorList (private)

	Remove this iterator from the active iterator list.

 ******************************************************************************/

template <class T>
void
JStringIterator<T>::RemoveFromIteratorList()
{
	if (itsConstOrderedSet == NULL)
		{
		return;
		}

	JStringIterator<T>* firstIterator = itsConstOrderedSet->itsFirstIterator;

	if (firstIterator == this)
		{
		const_cast<JOrderedSet<T>*>(itsConstOrderedSet)->itsFirstIterator = itsNextIterator;
		}
	else
		{
		JStringIterator<T>* prevIterator = firstIterator;
		JStringIterator<T>* currIterator = firstIterator->itsNextIterator;

		while (currIterator != NULL)
			{
			if (currIterator == this)
				{
				prevIterator->itsNextIterator = itsNextIterator;
				break;
				}

			prevIterator = currIterator;
			currIterator = currIterator->itsNextIterator;
			}
		}
}

/******************************************************************************
 OrderedSetChanged (virtual protected)

	Respond to changes in itsConstOrderedSet and recursively tell the next
	iterator in the list to do the same. Assuming that only a handful of
	iterators are ever in a list, recursion should be safe.

 ******************************************************************************/

template <class T>
void
JStringIterator<T>::OrderedSetChanged
	(
	const JBroadcaster::Message& message
	)
{
	if (message.Is(JOrderedSetT::kElementsInserted))
		{
		const JOrderedSetT::ElementsInserted* info =
			dynamic_cast<const JOrderedSetT::ElementsInserted*>(&message);
		assert( info != NULL );
		if (itsCursorPosition >= info->GetFirstIndex())
			{
			itsCursorPosition += info->GetCount();
			}
		}
	else if (message.Is(JOrderedSetT::kElementsRemoved))
		{
		const JOrderedSetT::ElementsRemoved* info =
			dynamic_cast<const JOrderedSetT::ElementsRemoved*>(&message);
		assert( info != NULL );
		if (itsCursorPosition > info->GetLastIndex())
			{
			itsCursorPosition -= info->GetCount();
			}
		else if (itsCursorPosition >= info->GetFirstIndex())
			{
			itsCursorPosition = info->GetFirstIndex()-1;
			}
		}
	else if (message.Is(JOrderedSetT::kCopied))
		{
		itsCursorPosition = 0;
		}
	else if (message.Is(JOrderedSetT::kGoingAway))
		{
		itsConstOrderedSet = NULL;
		itsOrderedSet      = NULL;
		itsCursorPosition  = 0;
		}

	if (itsNextIterator != NULL)
		{
		itsNextIterator->OrderedSetChanged(message);
		}
}

#endif
