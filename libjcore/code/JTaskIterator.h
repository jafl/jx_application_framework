/******************************************************************************
 JTaskIterator.h

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTaskIterator
#define _H_JTaskIterator

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray.h>

template <class T>
class JTaskIterator : public JPtrArrayIterator<T>
{
public:

	JTaskIterator(const JPtrArray<T>& theArray,
				  const JIteratorPosition start = kJIteratorStartAtBeginning,
				  const JIndex index = 0);
	JTaskIterator(JPtrArray<T>* theArray,
				  const JIteratorPosition start = kJIteratorStartAtBeginning,
				  const JIndex index = 0);

protected:

	virtual void	OrderedSetChanged(const JBroadcaster::Message& message);
};

#endif
