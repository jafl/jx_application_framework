/******************************************************************************
 JOrderedSetIterator.h

	Interface for JOrderedSetIterator class template

	Copyright © 1994-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JOrderedSetIterator
#define _H_JOrderedSetIterator

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JCollection.h>

// must forward declare to avoid #include loop
// (We do it here so JOrderedSet includes JOrderedSetIterator for convenience.)
template <class T> class JOrderedSet;

typedef unsigned long	JCursorPosition;

enum JIteratorPosition
{
	kJIteratorStartAtBeginning,	// absolute
	kJIteratorStartAtEnd,		// absolute
	kJIteratorStartBefore,		// relative to given index
	kJIteratorStartAfter		// relative to given index
};

template <class T>
class JOrderedSetIterator
{
	friend class JOrderedSet<T>;

public:

	JOrderedSetIterator(const JOrderedSet<T>& theOrderedSet,
						const JIteratorPosition start = kJIteratorStartAtBeginning,
						const JIndex index = 0);
	JOrderedSetIterator(JOrderedSet<T>* theOrderedSet,
						const JIteratorPosition start = kJIteratorStartAtBeginning,
						const JIndex index = 0);
	JOrderedSetIterator(const JOrderedSetIterator<T>& source);

	virtual ~JOrderedSetIterator();

	virtual JBoolean	Prev(T* data);
	virtual JBoolean	Next(T* data);

	virtual void		SkipPrev(const JSize count = 1);
	virtual void		SkipNext(const JSize count = 1);

	virtual void		MoveTo(const JIteratorPosition newPosition, const JIndex index);
	JBoolean			AtBeginning();
	JBoolean			AtEnd();

	// only allowed if constructed from non-const JOrderedSet<T>*

	virtual JBoolean	SetPrev(const T& data);
	virtual JBoolean	SetNext(const T& data);

	virtual JBoolean	RemovePrev();
	virtual JBoolean	RemoveNext();

protected:

	const JOrderedSet<T>*	GetConstOrderedSet() const;
	JBoolean				GetOrderedSet(JOrderedSet<T>** obj) const;

	JCursorPosition	GetCursor() const;
	void			SetCursor(const JCursorPosition pos);

	virtual void	OrderedSetChanged(const JBroadcaster::Message& message);

private:

	const JOrderedSet<T>*	itsConstOrderedSet;		// JOrderedSet that is being iterated over
	JOrderedSet<T>*			itsOrderedSet;			// NULL if we were passed a const object
	JCursorPosition			itsCursorPosition;		// Current iterator position

	JOrderedSetIterator<T>*	itsNextIterator;		// Next iterator in linked list

private:

	void	JOrderedSetIteratorX(const JIteratorPosition start, const JIndex index);

	void	AddToIteratorList();
	void	RemoveFromIteratorList();

	// not allowed

	const JOrderedSetIterator<T>& operator=(const JOrderedSetIterator<T>& source);
};

#endif
