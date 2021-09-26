/******************************************************************************
 JLinkedListIterator.h

	Copyright (C) 1996-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JLinkedListIterator
#define _H_JLinkedListIterator

#include "jx-af/jcore/JListIterator.h"

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

	virtual bool	Prev(T* item, const JIteratorAction move = kJIteratorMove) override;
	virtual bool	Next(T* item, const JIteratorAction move = kJIteratorMove) override;

	virtual void		SkipPrev(const JSize count = 1) override;
	virtual void		SkipNext(const JSize count = 1) override;

	virtual void		MoveTo(const JIteratorPosition newPosition, const JIndex index) override;

	// only allowed if constructed from non-const JList<T>*

	virtual bool	SetPrev(const T& data, const JIteratorAction move = kJIteratorMove) override;
	virtual bool	SetNext(const T& data, const JIteratorAction move = kJIteratorMove) override;

	virtual bool	RemovePrev(const JSize count = 1) override;
	virtual bool	RemoveNext(const JSize count = 1) override;

	virtual bool	Insert(const T& data) override;

protected:

	virtual void	ListChanged(const JBroadcaster::Message& message) override;

private:

	JLinkedList<T>*			itsLinkedList;
	JLinkedListElement<T>*	itsCurrElement;		// if cursor > 0, it is after this element
	bool				itsIgnoreListChangedFlag;

private:

	void	PrivateMoveTo();

	// not allowed

	const JLinkedListIterator<T>& operator=(const JLinkedListIterator<T>& source);
};

#endif
