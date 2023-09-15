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
				   const JListT::Position start = JListT::kStartAtBeginning,
				   const JIndex index = 0);
	JArrayIterator(JArray<T>* theArray,
				   const JListT::Position start = JListT::kStartAtBeginning,
				   const JIndex index = 0);
	JArrayIterator(const JArrayIterator<T>& source);

	~JArrayIterator() override;

	bool	Prev(T* data, const JListT::Action move = JListT::kMove) override;
	bool	Next(T* data, const JListT::Action move = JListT::kMove) override;

	void	SkipPrev(const JSize count = 1) override;
	void	SkipNext(const JSize count = 1) override;

	// only allowed if constructed from non-const JArray<T>*

	bool	SetPrev(const T& data, const JListT::Action move = JListT::kMove) override;
	bool	SetNext(const T& data, const JListT::Action move = JListT::kMove) override;

	bool	RemovePrev(const JSize count = 1) override;
	bool	RemoveNext(const JSize count = 1) override;

	bool	Insert(const T& data) override;

protected:

	void	ListChanged(const JBroadcaster::Message& message) override;

private:

	JArray<T>*	itsArray;
};

#endif
