/******************************************************************************
 CBWaitForSymbolUpdateTask.h

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBWaitForSymbolUpdateTask
#define _H_CBWaitForSymbolUpdateTask

#include <JXUrgentTask.h>
#include <JBroadcaster.h>

class JProcess;

class CBWaitForSymbolUpdateTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	CBWaitForSymbolUpdateTask(JProcess* p);

	virtual ~CBWaitForSymbolUpdateTask();

	virtual void	Perform();

	void	StopWaiting();

private:

	JProcess*	itsProcess;
	JBoolean	itsKeepWaitingFlag;

private:

	// not allowed

	CBWaitForSymbolUpdateTask(const CBWaitForSymbolUpdateTask& source);
	const CBWaitForSymbolUpdateTask& operator=(const CBWaitForSymbolUpdateTask& source);
};


/******************************************************************************
 StopWaiting

 ******************************************************************************/

inline void
CBWaitForSymbolUpdateTask::StopWaiting()
{
	itsKeepWaitingFlag = kJFalse;
}

#endif
