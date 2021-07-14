/******************************************************************************
 JLinkedListIterator.h

	Copyright (C) 1996-2018 by John Lindal.

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

	virtual JBoolean	Prev(T* item, const JBoolean move = kJTrue) override;
	virtual JBoolean	Next(T* item, const JBoolean move = kJTrue) override;

	virtual void		SkipPrev(const JSize count = 1) override;
	virtual void		SkipNext(const JSize count = 1) override;

	virtual void		MoveTo(const JIteratorPosition newPosition, const JIndex index) override;

	// only allowed if constructed from non-const JList<T>*

	virtual JBoolean	SetPrev(const T& data, const JBoolean move = kJTrue) override;
	virtual JBoolean	SetNext(const T& data, const JBoolean move = kJTrue) override;

	virtual JBoolean	RemovePrev(const JSize count = 1) override;
	virtual JBoolean	RemoveNext(const JSize count = 1) override;

	virtual JBoolean	Insert(const T& data) override;

protected:

	virtual void	ListChanged(const JBroadcaster::Message& message) override;

private:

	JLinkedList<T>*			itsLinkedList;
	JLinkedListElement<T>*	itsCurrElement;		// if cursor > 0, it is after this element
	JBoolean				itsIgnoreListChangedFlag;

private:

	void	PrivateMoveTo();

	// not allowed

	const JLinkedListIterator<T>& operator=(const JLinkedListIterator<T>& source);
};

#endif
