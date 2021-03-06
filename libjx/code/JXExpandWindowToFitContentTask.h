/******************************************************************************
 JXExpandWindowToFitContentTask.h

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXExpandWindowToFitContentTask
#define _H_JXExpandWindowToFitContentTask

#include "JXUrgentTask.h"
#include <JBroadcaster.h>

class JXWindow;
class JXWidget;

class JXExpandWindowToFitContentTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXExpandWindowToFitContentTask(JXWindow* window);

	virtual ~JXExpandWindowToFitContentTask();

	void	ShowAfterFTC();
	void	PlaceAsDialogAfterFTC();
	void	FocusAfterFTC(JXWidget* widget);

	virtual void	Perform();

private:

	JXWindow*	itsWindow;	// not owned
	JBoolean	itShowWindowAfterFTCFlag;
	JXWidget*	itsFocusWidget;

private:

	// not allowed

	JXExpandWindowToFitContentTask(const JXExpandWindowToFitContentTask& source);
	const JXExpandWindowToFitContentTask& operator=(const JXExpandWindowToFitContentTask& source);
};


/******************************************************************************
 ShowAfterFTC

 ******************************************************************************/

inline void
JXExpandWindowToFitContentTask::ShowAfterFTC()
{
	itShowWindowAfterFTCFlag = kJTrue;
}

/******************************************************************************
 FocusAfterFTC

 ******************************************************************************/

inline void
JXExpandWindowToFitContentTask::FocusAfterFTC
	(
	JXWidget* widget
	)
{
	itsFocusWidget = widget;
}

#endif
