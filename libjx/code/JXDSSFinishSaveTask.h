/******************************************************************************
 JXDSSFinishSaveTask.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDSSFinishSaveTask
#define _H_JXDSSFinishSaveTask

#include "JXDirectSaveAction.h"
#include "JXUrgentTask.h"

class JXSaveFileDialog;

class JXDSSFinishSaveTask : public JXDirectSaveAction, public JXUrgentTask
{
public:

	JXDSSFinishSaveTask(JXSaveFileDialog* dialog);

	void	Save(const JString& fullName) override;

protected:

	~JXDSSFinishSaveTask() override;

	void	Perform() override;

private:

	JXSaveFileDialog*	itsDialog;		// not owned
	JString				itsDirName;
};

#endif
