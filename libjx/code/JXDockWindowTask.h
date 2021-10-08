/******************************************************************************
 JXDockWindowTask.h

	Copyright (C) 2004 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDockWindowTask
#define _H_JXDockWindowTask

#include "jx-af/jx/JXIdleTask.h"
#include <jx-af/jcore/JBroadcaster.h>
#include <jx-af/jcore/JPoint.h>

class JXWindow;
class JXDockWidget;

class JXDockWindowTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	JXDockWindowTask(JXWindow* window, const Window parent, const JPoint& topLeft,
					 JXDockWidget* dock);

	~JXDockWindowTask() override;

	void	Perform(const Time delta, Time* maxSleepTime) override;
	bool	IsDone() const;

	static void	PrepareForDockAll();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXWindow*	itsWindow;
	Window		itsParent;
	JPoint		itsPoint;
	JIndex		itsState;
};

#endif
