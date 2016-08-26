/******************************************************************************
 JXCSFSelectPrevDirTask.h

	Interface for the JXCSFSelectPrevDirTask class

	Copyright � 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXCSFSelectPrevDirTask
#define _H_JXCSFSelectPrevDirTask

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
