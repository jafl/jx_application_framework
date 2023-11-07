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
template <class T> class JLinkedListItem;

template <class T>
class JLinkedListIterator : public JListIterator<T>
{
public:

	JLinkedListIterator(const JLinkedList<T>& theLinkedList,
						const JListT::Position start = JListT::kStartAtBeginning,
						const JIndex index = 0);
	JLinkedListIterator(JLinkedList<T>* theLinkedList,
						const JListT::Position start = JListT::kStartAtBeginning,
						const JIndex index = 0);
	JLinkedListIterator(const JLinkedListIterator<T>& source);

	~JLinkedListIterator() override;

	bool	Prev(T* item, const JListT::Action move = JListT::kMove) override;
	bool	Next(T* item, const JListT::Action move = JListT::kMove) override;

	void	SkipPrev(const JSize count = 1) override;
	void	SkipNext(const JSize count = 1) override;

	void	MoveTo(const JListT::Position newPosition, const JIndex index) override;

	// only allowed if constructed from non-const JList<T>*

	bool	SetPrev(const T& data, const JListT::Action move = JListT::kMove) override;
	bool	SetNext(const T& data, const JListT::Action move = JListT::kMove) override;

	bool	RemovePrev(const JSize count = 1) override;
	bool	RemoveNext(const JSize count = 1) override;

	bool	Insert(const T& data) override;

protected:

	void	ListChanged(const JBroadcaster::Message& message) override;

private:

	JLinkedList<T>*		itsLinkedList;
	JLinkedListItem<T>*	itsCurrItem;		// if cursor > 0, it is after this item
	bool				itsIgnoreListChangedFlag;

private:

	void	PrivateMoveTo();
};

#endif
