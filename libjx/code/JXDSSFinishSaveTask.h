/******************************************************************************
 JXDSSFinishSaveTask.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDSSFinishSaveTask
#define _H_JXDSSFinishSaveTask

#include "jx-af/jx/JXDirectSaveAction.h"
#include "jx-af/jx/JXUrgentTask.h"

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
	JString				itsDirName;
};

#endif
