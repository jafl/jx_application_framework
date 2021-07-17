/******************************************************************************
 JQueue.h

	Interface for JQueue class

	Copyright (C) 1993-97 John Lindal.

 ******************************************************************************/

#ifndef _H_JQueue
#define _H_JQueue

#include "JContainer.h"
#include "JListIterator.h"	// template

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
	bool	GetNext(T* element);
	const T		PeekNext() const;
	bool	PeekNext(T* element) const;
	const T		PeekNext(const JIndex index) const;
	bool	PeekNext(const JIndex index, T* element) const;

	void	Flush();
	void	Discard(const JSize numToDiscard);

	JListIterator<T>*	NewIterator();
	JListIterator<T>*	NewIterator() const;

protected:

	S*	GetElements();

private:

	S*	itsElements;
};

#include "JQueue.tmpl"

#endif
