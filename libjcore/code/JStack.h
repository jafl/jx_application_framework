/******************************************************************************
 JStack.h

	Interface for JStack class

	Copyright (C) 1993-97 John Lindal.

 ******************************************************************************/

#ifndef _H_JStack
#define _H_JStack

#include "JContainer.h"
#include "JListIterator.h"	// template

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
	T			Peek() const;
	JBoolean	Peek(T* element) const;
	T			Peek(const JIndex index) const;
	JBoolean	Peek(const JIndex index, T* element) const;

	void	Clear();
	void	Unwind(const JSize numToUnwind);

	JListIterator<T>*	NewIterator();
	JListIterator<T>*	NewIterator() const;

protected:

	S*	GetElements();

private:

	S*	itsElements;
};

#include "JStack.tmpl"

#endif
