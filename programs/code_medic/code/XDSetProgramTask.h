/******************************************************************************
 XDSetProgramTask.h

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDSetProgramTask
#define _H_XDSetProgramTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>

class XDSetProgramTask : public JXUrgentTask
{
public:

	XDSetProgramTask();

	virtual ~XDSetProgramTask();

	virtual void	Perform();

private:

	// not allowed

	XDSetProgramTask(const XDSetProgramTask& source);
	const XDSetProgramTask& operator=(const XDSetProgramTask& source);
};

#endif
