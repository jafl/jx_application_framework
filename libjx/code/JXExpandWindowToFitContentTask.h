/******************************************************************************
 JXExpandWindowToFitContentTask.h

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXExpandWindowToFitContentTask
#define _H_JXExpandWindowToFitContentTask

#include "jx-af/jx/JXUrgentTask.h"
#include <jx-af/jcore/JBroadcaster.h>

class JXWindow;
class JXWidget;

class JXExpandWindowToFitContentTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXExpandWindowToFitContentTask(JXWindow* window);

	~JXExpandWindowToFitContentTask();

	void	ShowAfterFTC();
	void	PlaceAsDialogAfterFTC();
	void	FocusAfterFTC(JXWidget* widget);

	void	Perform() override;

private:

	JXWindow*	itsWindow;	// not owned
	bool		itShowWindowAfterFTCFlag;
	JXWidget*	itsFocusWidget;
};


/******************************************************************************
 ShowAfterFTC

 ******************************************************************************/

inline void
JXExpandWindowToFitContentTask::ShowAfterFTC()
{
	itShowWindowAfterFTCFlag = true;
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
