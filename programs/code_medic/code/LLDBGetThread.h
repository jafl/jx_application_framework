/******************************************************************************
 LLDBGetThread.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetThread
#define _H_LLDBGetThread

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetThread.h"

class LLDBGetThread : public CMGetThread
{
public:

	LLDBGetThread(CMThreadsWidget* widget);

	virtual	~LLDBGetThread();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	LLDBGetThread(const LLDBGetThread& source);
	const LLDBGetThread& operator=(const LLDBGetThread& source);
};

#endif
