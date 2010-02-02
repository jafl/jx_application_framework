/******************************************************************************
 SyGBeginEditingTask.h

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGBeginEditingTask
#define _H_SyGBeginEditingTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>
#include <JBroadcaster.h>

class JPoint;
class SyGFileTreeTable;
class SyGFileTreeNode;

class SyGBeginEditingTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	SyGBeginEditingTask(SyGFileTreeTable* table, const JPoint& cell);

	virtual ~SyGBeginEditingTask();

	void			Perform();
	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	SyGFileTreeTable*	itsTable;	// owns us
	SyGFileTreeNode*	itsNode;	// not owned; NULL if dead

private:

	// not allowed

	SyGBeginEditingTask(const SyGBeginEditingTask& source);
	const SyGBeginEditingTask& operator=(const SyGBeginEditingTask& source);

};

#endif
