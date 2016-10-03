/******************************************************************************
 GCLAlarm.cpp

	<Description>

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GCLAlarm.h>
#include "GCLAppointment.h"
#include "GCLUtil.h"

#include <jAssert.h>


/******************************************************************************
 Constructor

 *****************************************************************************/

GCLAlarm::GCLAlarm
	(
	const JIndex	count,
	const TimeUnit	unit,
	GCLAppointment* appt,
	const JBoolean	timeFromNow
	)
	:
	itsAppt(appt)
{
	GCLAlarmX(count, unit, appt, appt->GetStartDay(), appt->GetStartTime(), timeFromNow);
}

GCLAlarm::GCLAlarm
	(
	const JIndex	count,
	const TimeUnit	unit,
	GCLAppointment* appt,
	const CLDay	day,
	const CLTime	startTime,
	const JBoolean	timeFromNow
	)
	:
	itsAppt(appt)
{
	GCLAlarmX(count, unit, appt, day, startTime, timeFromNow);
}

void
GCLAlarm::GCLAlarmX
	(
	const JIndex	count,
	const TimeUnit	unit,
	GCLAppointment* appt,
	const CLDay	day,
	const CLTime	startTime,
	const JBoolean	timeFromNow
	)
{
	itsAlarmTime	= count;
	if (unit >= kMinute)
		{
		itsAlarmTime *= 60;
		}
	if (unit >= kHour)
		{
		itsAlarmTime *= 60;
		}
	if (unit >= kDay)
		{
		itsAlarmTime *= 24;
		}
	if (unit == kWeek)
		{
		itsAlarmTime *= 7;
		}
	else if (unit == kMonth)
		{
		itsAlarmTime *= 30;
		}

	if (timeFromNow)
		{
		time_t now			= GetCurrentEpochTime();
		time_t epochTime	= GetEpochTime(day, startTime);
		itsAlarmTime += now;
		if (itsAlarmTime > (JIndex)epochTime)
			{
			itsAlarmTime = epochTime;
			}
		}
	else
		{
		CLTime apptTime		= startTime;
		time_t epochTime	= GetEpochTime(day, apptTime);
		itsAlarmTime		= epochTime - itsAlarmTime;
		}

	itsApptDay	= day;
	itsApptTime	= startTime;
	appt->SetAlarmTime(itsAlarmTime);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GCLAlarm::~GCLAlarm()
{
}

/******************************************************************************
 CompareByStart (public)

 ******************************************************************************/

JOrderedSetT::CompareResult
GCLAlarm::CompareTimes
	(
	GCLAlarm * const & a1,
	GCLAlarm * const & a2
	)
{
	JIndex t1	= a1->GetAlarmTime();
	JIndex t2	= a2->GetAlarmTime();

	if (t1 == t2)
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
	else if (t1 > t2)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else
		{
		return JOrderedSetT::kFirstLessSecond;
		}
}
