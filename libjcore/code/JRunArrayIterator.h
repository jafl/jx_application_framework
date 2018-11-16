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
					  const JIteratorPosition start = kJIteratorStartAtBeginning,
					  const JIndex index = 0);
	JRunArrayIterator(JRunArray<T>* theRunArray,
					  const JIteratorPosition start = kJIteratorStartAtBeginning,
					  const JIndex index = 0);
	JRunArrayIterator(const JRunArrayIterator<T>& source);

	virtual ~JRunArrayIterator();

	virtual JBoolean	Prev(T* item) override;
	virtual JBoolean	Next(T* item) override;

	virtual void		SkipPrev(const JSize count = 1) override;
	virtual void		SkipNext(const JSize count = 1) override;

	virtual void		MoveTo(const JIteratorPosition newPosition, const JIndex index) override;

	// only allowed if constructed from non-const JList<T>*

	virtual JBoolean	SetPrev(const T& data) override;
	virtual JBoolean	SetNext(const T& data) override;

	JBoolean			SetPrev(const T& data, const JSize count);
	JBoolean			SetNext(const T& data, const JSize count);

	virtual JBoolean	RemovePrev(const JSize count = 1) override;
	virtual JBoolean	RemoveNext(const JSize count = 1) override;

	virtual JBoolean	Insert(const T& data) override;
	JBoolean			Insert(const T& data, const JSize count);
	JBoolean			InsertSlice(const JRunArray<T>& source, const JIndexRange& range);

	JBoolean	AtFirstRun() const;
	JBoolean	AtLastRun() const;

	JBoolean	PrevRun();
	JBoolean	NextRun();

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

private:

	void	CalcPrevRunInfo(JIndex* runIndex, JIndex* firstInRun) const;
	void	CalcNextRunInfo(JIndex* runIndex, JIndex* firstInRun) const;

	// not allowed

	const JRunArrayIterator<T>& operator=(const JRunArrayIterator<T>& source);
};

#endif
