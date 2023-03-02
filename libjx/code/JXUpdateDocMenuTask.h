/******************************************************************************
 JXUpdateDocMenuTask.h

	Interface for the JXUpdateDocMenuTask class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXUpdateDocMenuTask
#define _H_JXUpdateDocMenuTask

#include "JXUrgentTask.h"

class JXDocumentManager;

class JXUpdateDocMenuTask : public JXUrgentTask
{
public:

	JXUpdateDocMenuTask(JXDocumentManager* docMgr);

protected:

	~JXUpdateDocMenuTask() override;

	void	Perform() override;

private:

	JXDocumentManager*	itsDocMgr;
};

#endif
