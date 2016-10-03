/******************************************************************************
 JQueue.h

	Interface for JQueue class

	Copyright (C) 1993-97 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JQueue
#define _H_JQueue

#include <JContainer.h>
#include <JOrderedSetIterator.h>	// template

template <class T, class S>
class JQueue : public JContainer
{
public:

	JQueue();
	JQueue(const JQueue<T,S>& source);

	virtual	~JQueue();

	const JQueue<T,S>& operator=(const JQueue<T,S>& source);

	void		Append(const T& newElement);
	T			GetNext();
	JBoolean	GetNext(T* element);
	const T		PeekNext();
	JBoolean	PeekNext(T* element);
	const T		PeekNext(const JIndex index);
	JBoolean	PeekNext(const JIndex index, T* element);

	void	Flush();
	void	Discard(const JSize numToDiscard);

	JOrderedSetIterator<T>*	NewIterator();
	JOrderedSetIterator<T>*	NewIterator() const;

protected:

	S*	GetElements();

private:

	S*	itsElements;
};

#include <JQueue.tmpl>

#endif
