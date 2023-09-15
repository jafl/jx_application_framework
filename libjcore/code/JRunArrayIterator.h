/******************************************************************************
 JRunArrayIterator.h

	Interface for JRunArrayIterator class template

	Copyright (C) 1994-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JRunArrayIterator
#define _H_JRunArrayIterator

#include "JListIterator.h"

// must forward declare to avoid #include loop
// (We do it here so JRunArray includes JRunArrayIterator for convenience.)
template <class T> class JRunArray;

template <class T>
class JRunArrayIterator : public JListIterator<T>
{
public:

	JRunArrayIterator(const JRunArray<T>& theRunArray,
					  const JListT::Position start = JListT::kStartAtBeginning,
					  const JIndex index = 0);
	JRunArrayIterator(JRunArray<T>* theRunArray,
					  const JListT::Position start = JListT::kStartAtBeginning,
					  const JIndex index = 0);
	JRunArrayIterator(const JRunArrayIterator<T>& source);

	~JRunArrayIterator() override;

	bool	Prev(T* item, const JListT::Action move = JListT::kMove) override;
	bool	Next(T* item, const JListT::Action move = JListT::kMove) override;

	void	SkipPrev(const JSize count = 1) override;
	void	SkipNext(const JSize count = 1) override;

	void	MoveTo(const JListT::Position newPosition, const JIndex index) override;

	// only allowed if constructed from non-const JList<T>*

	bool	SetPrev(const T& data, const JListT::Action move = JListT::kMove) override;
	bool	SetNext(const T& data, const JListT::Action move = JListT::kMove) override;

	bool	SetPrev(const T& data, const JSize count, const JListT::Action move = JListT::kMove);
	bool	SetNext(const T& data, const JSize count, const JListT::Action move = JListT::kMove);

	bool	RemovePrev(const JSize count = 1) override;
	bool	RemoveNext(const JSize count = 1) override;

	bool	Insert(const T& data) override;
	bool	Insert(const T& data, const JSize count);
	bool	InsertSlice(const JRunArray<T>& source, const JIndexRange& range);

	bool	AtFirstRun() const;
	bool	AtLastRun() const;

	bool	PrevRun();
	bool	NextRun();

	JIndex	GetRunStart() const;
	JIndex	GetRunEnd() const;
	JSize	GetRunLength() const;
	JSize	GetRemainingInRun() const;
	T		GetRunData() const;

protected:

	void	ListChanged(const JBroadcaster::Message& message) override;

private:

	JRunArray<T>*	itsRunArray;
	JIndex			itsRunIndex;
	JIndex			itsOffsetInRun;
	bool			itsIgnoreListChangedFlag;

private:

	void	PrivateMoveTo(const JCursorPosition origPosition,
						  const JListT::Position newPosition, const JIndex index,
						  const bool wasAtLimit);
};

#endif
