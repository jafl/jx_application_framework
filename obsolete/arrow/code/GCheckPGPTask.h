/******************************************************************************
 GCheckPGPTask.h

	Interface for the GCheckPGPTask class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GCheckPGPTask
#define _H_GCheckPGPTask

#include <JXUrgentTask.h>

class SMTPMessage;

class GCheckPGPTask : public JXUrgentTask
{
public:

	GCheckPGPTask();

	virtual ~GCheckPGPTask();

	virtual void	Perform();


private:

	// not allowed

	GCheckPGPTask(const GCheckPGPTask& source);
	const GCheckPGPTask& operator=(const GCheckPGPTask& source);
};

#endif
