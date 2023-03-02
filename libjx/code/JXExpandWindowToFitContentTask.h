/******************************************************************************
 JXExpandWindowToFitContentTask.h

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXExpandWindowToFitContentTask
#define _H_JXExpandWindowToFitContentTask

#include "JXUrgentTask.h"

class JXWindow;
class JXWidget;

class JXExpandWindowToFitContentTask : public JXUrgentTask
{
public:

	JXExpandWindowToFitContentTask(JXWindow* window);

	void	ShowAfterFTC();
	void	PlaceAsDialogAfterFTC();
	void	FocusAfterFTC(JXWidget* widget);

protected:

	~JXExpandWindowToFitContentTask() override;

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
