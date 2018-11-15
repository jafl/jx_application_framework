/******************************************************************************
 JArrayIterator.h

	Copyright (C) 1994-2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArrayIterator
#define _H_JArrayIterator

#include "JListIterator.h"

// must forward declare to avoid #include loop
// (We do it here so JArray includes JArrayIterator for convenience.)
template <class T> class JArray;

template <class T>
class JArrayIterator : public JListIterator<T>
{
public:

	JArrayIterator(const JArray<T>& theArray,
				   const JIteratorPosition start = kJIteratorStartAtBeginning,
				   const JIndex index = 0);
	JArrayIterator(JArray<T>* theArray,
				   const JIteratorPosition start = kJIteratorStartAtBeginning,
				   const JIndex index = 0);
	JArrayIterator(const JArrayIterator<T>& source);

	virtual ~JArrayIterator();

	virtual JBoolean	Prev(T* data) override;
	virtual JBoolean	Next(T* data) override;

	virtual void		SkipPrev(const JSize count = 1) override;
	virtual void		SkipNext(const JSize count = 1) override;

	// only allowed if constructed from non-const JArray<T>*

	virtual JBoolean	SetPrev(const T& data) override;
	virtual JBoolean	SetNext(const T& data) override;

	virtual JBoolean	RemovePrev(const JSize count = 1) override;
	virtual JBoolean	RemoveNext(const JSize count = 1) override;

	virtual JBoolean	Insert(const T& data) override;

protected:

	virtual void	ListChanged(const JBroadcaster::Message& message) override;

private:

	JArray<T>*	itsArray;

private:

	// not allowed

	const JArrayIterator<T>& operator=(const JArrayIterator<T>& source);
};

#endif
