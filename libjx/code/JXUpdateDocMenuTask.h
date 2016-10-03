/******************************************************************************
 JXUpdateDocMenuTask.h

	Interface for the JXUpdateDocMenuTask class

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXUpdateDocMenuTask
#define _H_JXUpdateDocMenuTask

#include <JXUrgentTask.h>

class JXDocumentManager;

class JXUpdateDocMenuTask : public JXUrgentTask
{
public:

	JXUpdateDocMenuTask(JXDocumentManager* docMgr);

	virtual ~JXUpdateDocMenuTask();

	virtual void	Perform();

private:

	JXDocumentManager*	itsDocMgr;

private:

	// not allowed

	JXUpdateDocMenuTask(const JXUpdateDocMenuTask& source);
	const JXUpdateDocMenuTask& operator=(const JXUpdateDocMenuTask& source);
};

#endif
