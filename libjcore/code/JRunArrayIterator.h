/******************************************************************************
 JRunArrayIterator.h

	Interface for JRunArrayIterator class template

	Copyright (C) 1994-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JRunArrayIterator
#define _H_JRunArrayIterator

#include "jx-af/jcore/JListIterator.h"

// must forward declare to avoid #include loop
// (We do it here so JRunArray includes JRunArrayIterator for convenience.)
template <class T> class JRunArray;

template <class T>
class JRunArrayIterator : public JListIterator<T>
{
public:

	JRunArrayIterator(const JRunArray<T>& theRunArray,
					  const JIteratorPosition start = kJIteratorStartAtBeginning,
					  const JIndex index = 0);
	JRunArrayIterator(JRunArray<T>* theRunArray,
					  const JIteratorPosition start = kJIteratorStartAtBeginning,
					  const JIndex index = 0);
	JRunArrayIterator(const JRunArrayIterator<T>& source);

	virtual ~JRunArrayIterator();

	virtual bool	Prev(T* item, const JIteratorAction move = kJIteratorMove) override;
	virtual bool	Next(T* item, const JIteratorAction move = kJIteratorMove) override;

	virtual void		SkipPrev(const JSize count = 1) override;
	virtual void		SkipNext(const JSize count = 1) override;

	virtual void		MoveTo(const JIteratorPosition newPosition, const JIndex index) override;

	// only allowed if constructed from non-const JList<T>*

	virtual bool	SetPrev(const T& data, const JIteratorAction move = kJIteratorMove) override;
	virtual bool	SetNext(const T& data, const JIteratorAction move = kJIteratorMove) override;

	bool			SetPrev(const T& data, const JSize count, const JIteratorAction move = kJIteratorMove);
	bool			SetNext(const T& data, const JSize count, const JIteratorAction move = kJIteratorMove);

	virtual bool	RemovePrev(const JSize count = 1) override;
	virtual bool	RemoveNext(const JSize count = 1) override;

	virtual bool	Insert(const T& data) override;
	bool			Insert(const T& data, const JSize count);
	bool			InsertSlice(const JRunArray<T>& source, const JIndexRange& range);

	bool	AtFirstRun() const;
	bool	AtLastRun() const;

	bool	PrevRun();
	bool	NextRun();

	JIndex		GetRunStart() const;
	JIndex		GetRunEnd() const;
	JSize		GetRunLength() const;
	JSize		GetRemainingInRun() const;
	T			GetRunData() const;

protected:

	virtual void	ListChanged(const JBroadcaster::Message& message) override;

private:

	JRunArray<T>*	itsRunArray;
	JIndex			itsRunIndex;
	JIndex			itsOffsetInRun;
	bool		itsIgnoreListChangedFlag;

private:

	void	PrivateMoveTo(const JCursorPosition origPosition,
						  const JIteratorPosition newPosition, const JIndex index,
						  const bool wasAtLimit);

	// not allowed

	const JRunArrayIterator<T>& operator=(const JRunArrayIterator<T>& source);
};

#endif
