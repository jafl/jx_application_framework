/******************************************************************************
 JStack.h

	Interface for JStack class

	Copyright © 1993-97 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JStack
#define _H_JStack

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JContainer.h>
#include <JOrderedSetIterator.h>	// template

template <class T, class S>
class JStack : public JContainer
{
public:

	JStack();
	JStack(const JStack<T,S>& source);

	virtual	~JStack();

	const JStack<T,S>& operator=(const JStack<T,S>& source);

	void		Push(const T& newElement);
	T			Pop();
	JBoolean	Pop(T* element);
	const T		Peek();
	JBoolean	Peek(T* element);
	const T		Peek(const JIndex index);
	JBoolean	Peek(const JIndex index, T* element);

	void	Clear();
	void	Unwind(const JSize numToUnwind);

	JOrderedSetIterator<T>*	NewIterator();
	JOrderedSetIterator<T>*	NewIterator() const;

protected:

	S*	GetElements();

private:

	S*	itsElements;
};

#endif
