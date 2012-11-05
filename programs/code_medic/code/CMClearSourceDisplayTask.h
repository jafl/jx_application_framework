/******************************************************************************
 CMClearSourceDisplayTask.h

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMClearSourceDisplayTask
#define _H_CMClearSourceDisplayTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>

class CMSourceDirector;

class CMClearSourceDisplayTask : public JXUrgentTask
{
public:

	CMClearSourceDisplayTask(CMSourceDirector* dir);

	virtual ~CMClearSourceDisplayTask();

	virtual void	Perform();

private:

	CMSourceDirector*	itsDirector;	// not owned

private:

	// not allowed

	CMClearSourceDisplayTask(const CMClearSourceDisplayTask& source);
	const CMClearSourceDisplayTask& operator=(const CMClearSourceDisplayTask& source);
};

#endif
