/******************************************************************************
 SVNBeginEditingTask.h

	Copyright (C) 2000-08 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNBeginEditingTask
#define _H_SVNBeginEditingTask

#include <JXIdleTask.h>
#include <JBroadcaster.h>

class JPoint;
class SVNRepoView;
class SVNRepoTreeNode;

class SVNBeginEditingTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	SVNBeginEditingTask(SVNRepoView* table, const JPoint& cell);

	virtual ~SVNBeginEditingTask();

	void			Perform();
	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	SVNRepoView*		itsView;	// owns us
	SVNRepoTreeNode*	itsNode;	// not owned; NULL if dead

private:

	// not allowed

	SVNBeginEditingTask(const SVNBeginEditingTask& source);
	const SVNBeginEditingTask& operator=(const SVNBeginEditingTask& source);

};

#endif
