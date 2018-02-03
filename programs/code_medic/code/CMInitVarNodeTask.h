/******************************************************************************
 CMInitVarNodeTask.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMInitVarNodeTask
#define _H_CMInitVarNodeTask

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
