/******************************************************************************
 JLinkedListIterator.h

	Interface for JLinkedListIterator class template

	Copyright (C) 1996-97 by John Lindal.

 ******************************************************************************/

#ifndef _H_JLinkedListIterator
#define _H_JLinkedListIterator

#include "JListIterator.h"

// must forward declare to avoid #include loop
// (We do it here so JLinkedList includes JLinkedListIterator for convenience.)
template <class T> class JLinkedList;
template <class T> class JLinkedListElement;

template <class T>
class JLinkedListIterator : public JListIterator<T>
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

	// only allowed if constructed from non-const JList<T>*

	virtual JBoolean	SetPrev(const T& data);
	virtual JBoolean	SetNext(const T& data);

	virtual JBoolean	RemovePrev();
	virtual JBoolean	RemoveNext();

protected:

	virtual void	ListChanged(const JBroadcaster::Message& message);

private:

	JLinkedList<T>*			itsLinkedList;
	JLinkedListElement<T>*	itsCurrElement;		// element pointed to by ((JIndex) cursor)

private:

	// not allowed

	const JLinkedListIterator<T>& operator=(const JLinkedListIterator<T>& source);
};

#endif
