/******************************************************************************
 JXCheckModTimeTask.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCheckModTimeTask
#define _H_JXCheckModTimeTask

#include "jx-af/jx/JXIdleTask.h"
#include <jx-af/jcore/JBroadcaster.h>
#include <jx-af/jcore/JString.h>

class JXCheckModTimeTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	JXCheckModTimeTask(const Time period, const JString& fullName);

	virtual ~JXCheckModTimeTask();

	const JString&	GetFileName() const;
	void			UpdateModTime();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JString	itsFullName;
	time_t	itsModTime;

public:

	static const JUtf8Byte* kFileChanged;

	class FileChanged: public JBroadcaster::Message
		{
		public:
		
			FileChanged()
				:
				JBroadcaster::Message(kFileChanged)
				{ };
		};
};


/******************************************************************************
 GetFileName

 ******************************************************************************/

inline const JString&
JXCheckModTimeTask::GetFileName()
	const
{
	return itsFullName;
}

#endif
