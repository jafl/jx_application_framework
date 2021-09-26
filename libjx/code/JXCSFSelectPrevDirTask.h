/******************************************************************************
 JXCSFSelectPrevDirTask.h

	Interface for the JXCSFSelectPrevDirTask class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCSFSelectPrevDirTask
#define _H_JXCSFSelectPrevDirTask

#include "jx-af/jx/JXUrgentTask.h"
#include <jx-af/jcore/JBroadcaster.h>
#include <jx-af/jcore/JString.h>

class JDirInfo;
class JXDirTable;

class JXCSFSelectPrevDirTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXCSFSelectPrevDirTask(JDirInfo* dirInfo, JXDirTable* dirTable,
						   const JString& dirName);

	virtual ~JXCSFSelectPrevDirTask();

	virtual void	Perform();

private:

	JDirInfo*	itsDirInfo;		// not owned
	JXDirTable*	itsDirTable;	// not owned
	JString		itsDirName;
};

#endif
