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

	~JXDSSFinishSaveTask() override;

	void	Save(const JString& fullName) override;
	void	Perform() override;

private:

	JXSaveFileDialog*	itsDialog;		// not owned
	JString				itsDirName;
};

#endif
