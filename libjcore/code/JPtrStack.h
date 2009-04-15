/******************************************************************************
 JPtrStack.h

	Interface for JPtrStack class

	Copyright © 1996-97 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPtrStack
#define _H_JPtrStack

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JStack.h>
#include <JPtrArray.h>

template <class T, class S>
class JPtrStack : public JStack<T*,S>
{
public:

	JPtrStack(const JPtrArrayT::CleanUpAction action);

	virtual ~JPtrStack();

	// these insert a *copy* of the object into the array
	// (only available if template instantiated with #define JPtrStackCopy)

	void	PushCopy(const T& newElement);

	void	ClearDelete();
	void	ClearDeleteAsArrays();

	void	UnwindDelete(const JSize numToUnwind);
	void	UnwindDeleteAsArrays(const JSize numToUnwind);

	JPtrArrayT::CleanUpAction	GetCleanUpAction() const;
	void						SetCleanUpAction(const JPtrArrayT::CleanUpAction action);

private:

	JPtrArrayT::CleanUpAction	itsCleanUpAction;

private:

	// These are not safe because the CleanUpAction must not be kDeleteAllAs*
	// for both the original and the copy.

	JPtrStack(const JPtrStack<T,S>& source);
	const JPtrStack<T,S>& operator=(const JPtrStack<T,S>& source);
};

#endif
