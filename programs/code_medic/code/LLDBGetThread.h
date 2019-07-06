/******************************************************************************
 LLDBGetThread.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetThread
#define _H_LLDBGetThread

#include "CMGetThread.h"

class LLDBGetThread : public CMGetThread
{
public:

	LLDBGetThread(CMThreadsWidget* widget);

	virtual	~LLDBGetThread();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	LLDBGetThread(const LLDBGetThread& source);
	const LLDBGetThread& operator=(const LLDBGetThread& source);
};

#endif
