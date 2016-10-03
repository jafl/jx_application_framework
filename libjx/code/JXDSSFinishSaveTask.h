/******************************************************************************
 JXDSSFinishSaveTask.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDSSFinishSaveTask
#define _H_JXDSSFinishSaveTask

#include <JXDirectSaveAction.h>
#include <JXUrgentTask.h>

class JXSaveFileDialog;

class JXDSSFinishSaveTask : public JXDirectSaveAction, public JXUrgentTask
{
public:

	JXDSSFinishSaveTask(JXSaveFileDialog* dialog);

	virtual ~JXDSSFinishSaveTask();

	virtual void	Save(const JString& fullName);
	virtual void	Perform();

private:

	JXSaveFileDialog*	itsDialog;		// not owned
	JString*			itsDirName;

private:

	// not allowed

	JXDSSFinishSaveTask(const JXDSSFinishSaveTask& source);
	const JXDSSFinishSaveTask& operator=(const JXDSSFinishSaveTask& source);
};

#endif
