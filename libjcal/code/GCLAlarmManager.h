/******************************************************************************
 GCLAlarmManager.h

	Interface for the GCLAlarmManager class

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GCLAlarmManager
#define _H_GCLAlarmManager

#include <JBroadcaster.h>
#include <JPtrArray.h>

#include "GCLUtil.h"

class JXTimerTask;

class GCLAlarm;

class GCLAlarmManager : public JBroadcaster
{
public:

	GCLAlarmManager();
	virtual ~GCLAlarmManager();

	void AddAlarm(GCLAlarm* alarm);

protected:

	virtual void Receive(JBroadcaster* sender, const Message& message);

private:

	JPtrArray<GCLAlarm>*	itsAlarms;
	JXTimerTask*			itsMinuteTask;
	time_t					itsLastBroadcast;

private:

	void	CheckAlarms();
	void	AlarmTriggered(GCLAlarm* alarm);

	// not allowed

	GCLAlarmManager(const GCLAlarmManager& source);
	const GCLAlarmManager& operator=(const GCLAlarmManager& source);

public:

	static const JCharacter* kMinuteAlarm;

	class MinuteAlarm: public JBroadcaster::Message
		{
		public:

			MinuteAlarm()
				:
				JBroadcaster::Message(kMinuteAlarm)
				{ };
		};

};



#endif
