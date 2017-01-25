/******************************************************************************
 GCLAlarmManager.cpp

	<Description>

	BASE CLASS = public JBroadcaster

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GCLAlarmManager.h>
#include "GCLAlarm.h"
#include "GCLAlarmDialog.h"
#include "GCLGlobals.h"
#include <JXTimerTask.h>
#include <jAssert.h>

const Time kMinutePeriod		= 5000;
const JCoordinate kSecsPerMin	= 60;

const JCharacter* GCLAlarmManager::kMinuteAlarm	= "kMinuteAlarm::GCLAlarmManager";

/******************************************************************************
 Constructor

 *****************************************************************************/

GCLAlarmManager::GCLAlarmManager()
	:
	JBroadcaster()
{
	itsAlarms	= jnew JPtrArray<GCLAlarm>(JPtrArrayT::kDeleteAll);
	assert(itsAlarms != NULL);

	itsAlarms->SetCompareFunction(GCLAlarm::CompareTimes);

	itsMinuteTask	= jnew JXTimerTask(kMinutePeriod);
	assert(itsMinuteTask != NULL);
	itsMinuteTask->Start();
	ListenTo(itsMinuteTask);

	itsLastBroadcast = GetCurrentEpochTime();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GCLAlarmManager::~GCLAlarmManager()
{
	jdelete itsAlarms;
	jdelete itsMinuteTask;
}

/******************************************************************************
 AddAlarm (public)

 ******************************************************************************/

void
GCLAlarmManager::AddAlarm
	(
	GCLAlarm* alarm
	)
{
	itsAlarms->InsertSorted(alarm);
//	std::cout << "Alarm added." << std::endl;
}

/******************************************************************************
 Receive (public)

 ******************************************************************************/

void
GCLAlarmManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsMinuteTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		CheckAlarms();
		}
}

/******************************************************************************
 CheckAlarms (private)

 ******************************************************************************/

void
GCLAlarmManager::CheckAlarms()
{
	time_t now = GetCurrentEpochTime();

	JSize count	= itsAlarms->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GCLAlarm* alarm	= itsAlarms->GetElement(i);
		if (alarm->GetAlarmTime() <= (JIndex)now)
			{
			AlarmTriggered(alarm);
			i--;
			count--;
			}
		else
			{
			break;
			}
		}

	if (now - itsLastBroadcast > kSecsPerMin)
		{
		Broadcast(MinuteAlarm());
		itsLastBroadcast	= now;
		}
}

/******************************************************************************
 AlarmTriggered (private)

 ******************************************************************************/

void
GCLAlarmManager::AlarmTriggered
	(
	GCLAlarm* alarm
	)
{
	GCLAlarmDialog* dialog	= jnew GCLAlarmDialog(JXGetApplication(), alarm);
	assert(dialog != NULL);
	dialog->BeginDialog();
	ListenTo(dialog);
	itsAlarms->Remove(alarm);
}
