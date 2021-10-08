/******************************************************************************
 JXDeleteObjectTask.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDeleteObjectTask
#define _H_JXDeleteObjectTask

#include "jx-af/jx/JXIdleTask.h"
#include <jx-af/jcore/JBroadcaster.h>

template <class T>
class JXDeleteObjectTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	static void	Delete(T* obj);

	~JXDeleteObjectTask() override;

	void	Perform(const Time delta, Time* maxSleepTime) override;

protected:

	JXDeleteObjectTask(T* obj);

	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	T*	itsObject;
};

#include "JXDeleteObjectTask.tmpl"

#endif
