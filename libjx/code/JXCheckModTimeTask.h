/******************************************************************************
 JXCheckModTimeTask.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXCheckModTimeTask
#define _H_JXCheckModTimeTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>
#include <JBroadcaster.h>
#include <JString.h>

class JXCheckModTimeTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	JXCheckModTimeTask(const Time period, const JCharacter* fullName);

	virtual ~JXCheckModTimeTask();

	const JString&	GetFileName() const;
	void			UpdateModTime();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JString	itsFullName;
	time_t	itsModTime;

private:

	// not allowed

	JXCheckModTimeTask(const JXCheckModTimeTask& source);
	const JXCheckModTimeTask& operator=(const JXCheckModTimeTask& source);

public:

	static const JCharacter* kFileChanged;

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
