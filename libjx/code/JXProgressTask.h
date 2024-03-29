/******************************************************************************
 JXProgressTask.h

	Copyright (C) 2000 John Lindal.

 ******************************************************************************/

#ifndef _H_JXProgressTask
#define _H_JXProgressTask

#include "JXIdleTask.h"
#include <jx-af/jcore/JBroadcaster.h>

class JString;
class JProgressDisplay;

class JXProgressTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	JXProgressTask(const JString& message, const bool allowCancel);
	JXProgressTask(JProgressDisplay* pg);

	~JXProgressTask() override;

protected:

	void	Perform(const Time delta) override;

private:

	JProgressDisplay*	itsPG;
	bool				itsOwnsPGFlag;

private:

	void	JXProgressTaskX(const JString& message, const bool allowCancel);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kCancelled;

	class Cancelled : public JBroadcaster::Message
	{
	public:

		Cancelled()
			:
			JBroadcaster::Message(kCancelled)
			{ };
	};
};

#endif
