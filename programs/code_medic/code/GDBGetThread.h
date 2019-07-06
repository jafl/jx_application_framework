/******************************************************************************
 GDBGetThread.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetThread
#define _H_GDBGetThread

#include "CMGetThread.h"

class GDBGetThread : public CMGetThread
{
public:

	GDBGetThread(CMThreadsWidget* widget);

	virtual	~GDBGetThread();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	GDBGetThread(const GDBGetThread& source);
	const GDBGetThread& operator=(const GDBGetThread& source);
};

#endif
