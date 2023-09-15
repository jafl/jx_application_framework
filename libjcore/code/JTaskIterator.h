/******************************************************************************
 JTaskIterator.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTaskIterator
#define _H_JTaskIterator

#include "JPtrArray.h"

template <class T>
class JTaskIterator : public JPtrArrayIterator<T>
{
public:

	JTaskIterator(const JPtrArray<T>& theArray,
				  const JListT::Position start = JListT::kStartAtBeginning,
				  const JIndex index = 0);
	JTaskIterator(JPtrArray<T>* theArray,
				  const JListT::Position start = JListT::kStartAtBeginning,
				  const JIndex index = 0);

protected:

	virtual void	ListChanged(const JBroadcaster::Message& message);
};

#include "JTaskIterator.tmpl"

#endif
