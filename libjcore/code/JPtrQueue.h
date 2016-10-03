/******************************************************************************
 JPtrQueue.h

	Interface for JPtrQueue class

	Copyright (C) 1996-97 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPtrQueue
#define _H_JPtrQueue

#include <JQueue.h>
#include <JPtrArray.h>

template <class T, class S>
class JPtrQueue : public JQueue<T*,S>
{
public:

	JPtrQueue(const JPtrArrayT::CleanUpAction action);

	virtual	~JPtrQueue();

	// these insert a *copy* of the object into the array
	// (only available if template instantiated with #define JPtrQueueCopy)

	void	AppendCopy(const T& newElement);

	void	FlushDelete();
	void	FlushDeleteAsArrays();

	void	DiscardDelete(const JSize numToDiscard);
	void	DiscardDeleteAsArrays(const JSize numToDiscard);

	JPtrArrayT::CleanUpAction	GetCleanUpAction() const;
	void						SetCleanUpAction(const JPtrArrayT::CleanUpAction action);

private:

	JPtrArrayT::CleanUpAction	itsCleanUpAction;

private:

	// These are not safe because the CleanUpAction must not be kDeleteAllAs*
	// for both the original and the copy.

	JPtrQueue(const JPtrQueue<T,S>& source);
	const JPtrQueue<T,S>& operator=(const JPtrQueue<T,S>& source);
};

#include <JPtrQueue.tmpl>

#endif
