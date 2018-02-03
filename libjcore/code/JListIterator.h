/******************************************************************************
 JListIterator.h

	Interface for JListIterator class template

	Copyright (C) 1994-97 by John Lindal.

 ******************************************************************************/

#ifndef _H_JOrderedSetIterator
#define _H_JOrderedSetIterator

#include <JCollection.h>

// must forward declare to avoid #include loop
// (We do it here so JList includes JListIterator for convenience.)
template <class T> class JList;

template <class T>
class JListIterator
{
	friend class JList<T>;

public:

	JListIterator(const JList<T>& theOrderedSet,
						const JIteratorPosition start = kJIteratorStartAtBeginning,
						const JIndex index = 0);
	JListIterator(JList<T>* theOrderedSet,
						const JIteratorPosition start = kJIteratorStartAtBeginning,
						const JIndex index = 0);
	JListIterator(const JListIterator<T>& source);

	virtual ~JListIterator();

	virtual JBoolean	Prev(T* data);
	virtual JBoolean	Next(T* data);

	virtual void		SkipPrev(const JSize count = 1);
	virtual void		SkipNext(const JSize count = 1);

	virtual void		MoveTo(const JIteratorPosition newPosition, const JIndex index);
	JBoolean			AtBeginning();
	JBoolean			AtEnd();

	// only allowed if constructed from non-const JList<T>*

	virtual JBoolean	SetPrev(const T& data);
	virtual JBoolean	SetNext(const T& data);

	virtual JBoolean	RemovePrev();
	virtual JBoolean	RemoveNext();

protected:

	const JList<T>*	GetConstOrderedSet() const;
	JBoolean				GetOrderedSet(JList<T>** obj) const;

	JCursorPosition	GetCursor() const;
	void			SetCursor(const JCursorPosition pos);

	virtual void	OrderedSetChanged(const JBroadcaster::Message& message);

private:

	const JList<T>*	itsConstOrderedSet;		// JList that is being iterated over
	JList<T>*			itsOrderedSet;			// NULL if we were passed a const object
	JCursorPosition			itsCursorPosition;		// Current iterator position

	JListIterator<T>*	itsNextIterator;		// Next iterator in linked list

private:

	void	JOrderedSetIteratorX(const JIteratorPosition start, const JIndex index);

	void	AddToIteratorList();
	void	RemoveFromIteratorList();

	// not allowed

	const JListIterator<T>& operator=(const JListIterator<T>& source);
};

#endif
