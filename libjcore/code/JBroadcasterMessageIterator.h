/******************************************************************************
 JBroadcasterMessageIterator.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JBroadcasterMessageIterator
#define _H_JBroadcasterMessageIterator

#include "JArray.h"

template <class T>
class JBroadcasterMessageIterator : public JArrayIterator<T>
{
public:

	JBroadcasterMessageIterator(const JArray<T>& theArray,
								const JIteratorPosition start = kJIteratorStartAtBeginning,
								const JIndex index = 0);
	JBroadcasterMessageIterator(JArray<T>* theArray,
								const JIteratorPosition start = kJIteratorStartAtBeginning,
								const JIndex index = 0);

protected:

	virtual void	ListChanged(const JBroadcaster::Message& message);
};

#include "JBroadcasterMessageIterator.tmpl"

#endif
