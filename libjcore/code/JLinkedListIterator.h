/******************************************************************************
 JLinkedListIterator.h

	Interface for JLinkedListIterator class template

	Copyright © 1996-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JLinkedListIterator
#define _H_JLinkedListIterator

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JOrderedSetIterator.h>

// must forward declare to avoid #include loop
// (We do it here so JLinkedList includes JLinkedListIterator for convenience.)
template <class T> class JLinkedList;
template <class T> class JLinkedListElement;

template <class T>
class JLinkedListIterator : public JOrderedSetIterator<T>
{
public:

	JLinkedListIterator(const JLinkedList<T>& theLinkedList,
						const JIteratorPosition start = kJIteratorStartAtBeginning,
						const JIndex index = 0);
	JLinkedListIterator(JLinkedList<T>* theLinkedList,
						const JIteratorPosition start = kJIteratorStartAtBeginning,
						const JIndex index = 0);
	JLinkedListIterator(const JLinkedListIterator<T>& source);

	virtual ~JLinkedListIterator();

	virtual JBoolean	Prev(T* item);
	virtual JBoolean	Next(T* item);

	virtual void		SkipPrev(const JSize count = 1);
	virtual void		SkipNext(const JSize count = 1);

	virtual void		MoveTo(const JIteratorPosition newPosition, const JIndex index);

	// only allowed if constructed from non-const JOrderedSet<T>*

	virtual JBoolean	SetPrev(const T& data);
	virtual JBoolean	SetNext(const T& data);

	virtual JBoolean	RemovePrev();
	virtual JBoolean	RemoveNext();

protected:

	virtual void	OrderedSetChanged(const JBroadcaster::Message& message);

private:

	JLinkedList<T>*			itsLinkedList;
	JLinkedListElement<T>*	itsCurrElement;		// element pointed to by ((JIndex) cursor)

private:

	// not allowed

	const JLinkedListIterator<T>& operator=(const JLinkedListIterator<T>& source);
};

#endif
