/******************************************************************************
 JXCSFSelectPrevDirTask.h

	Interface for the JXCSFSelectPrevDirTask class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXCSFSelectPrevDirTask
#define _H_JXCSFSelectPrevDirTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>
#include <JBroadcaster.h>

class JString;
class JDirInfo;
class JXDirTable;

class JXCSFSelectPrevDirTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXCSFSelectPrevDirTask(JDirInfo* dirInfo, JXDirTable* dirTable,
						   const JCharacter* dirName);

	virtual ~JXCSFSelectPrevDirTask();

	virtual void	Perform();

protected:

	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JDirInfo*	itsDirInfo;		// not owned
	JXDirTable*	itsDirTable;	// not owned
	JString*	itsDirName;

private:

	// not allowed

	JXCSFSelectPrevDirTask(const JXCSFSelectPrevDirTask& source);
	const JXCSFSelectPrevDirTask& operator=(const JXCSFSelectPrevDirTask& source);
};

#endif
