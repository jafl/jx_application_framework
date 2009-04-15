/******************************************************************************
 JXTLWAdjustToTreeTask.h

	Interface for the JXTLWAdjustToTreeTask class

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTLWAdjustToTreeTask
#define _H_JXTLWAdjustToTreeTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>

class JXTreeListWidget;

class JXTLWAdjustToTreeTask : public JXUrgentTask
{
public:

	JXTLWAdjustToTreeTask(JXTreeListWidget* widget);

	virtual ~JXTLWAdjustToTreeTask();

	virtual void	Perform();

private:

	JXTreeListWidget*	itsWidget;		// we don't own this

private:

	// not allowed

	JXTLWAdjustToTreeTask(const JXTLWAdjustToTreeTask& source);
	const JXTLWAdjustToTreeTask& operator=(const JXTLWAdjustToTreeTask& source);
};

#endif
