/******************************************************************************
 JXCSFSelectPrevDirTask.h

	Interface for the JXCSFSelectPrevDirTask class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCSFSelectPrevDirTask
#define _H_JXCSFSelectPrevDirTask

#include "JXUrgentTask.h"
#include <jx-af/jcore/JString.h>

class JDirInfo;
class JXDirTable;

class JXCSFSelectPrevDirTask : public JXUrgentTask
{
public:

	JXCSFSelectPrevDirTask(JDirInfo* dirInfo, JXDirTable* dirTable,
						   const JString& dirName);

protected:

	~JXCSFSelectPrevDirTask() override;

	void	Perform() override;

private:

	JDirInfo*	itsDirInfo;		// not owned
	JXDirTable*	itsDirTable;	// not owned
	JString		itsDirName;
};

#endif
