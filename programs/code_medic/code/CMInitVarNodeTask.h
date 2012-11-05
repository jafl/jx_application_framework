/******************************************************************************
 CMInitVarNodeTask.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMInitVarNodeTask
#define _H_CMInitVarNodeTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>
#include <JBroadcaster.h>

class CMVarNode;

class CMInitVarNodeTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	CMInitVarNodeTask(CMVarNode* node);

	virtual ~CMInitVarNodeTask();

	virtual void	Perform();

private:

	CMVarNode*	itsNode;		// we don't own this

private:

	// not allowed

	CMInitVarNodeTask(const CMInitVarNodeTask& source);
	const CMInitVarNodeTask& operator=(const CMInitVarNodeTask& source);
};

#endif
