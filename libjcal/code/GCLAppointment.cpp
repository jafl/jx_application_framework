/******************************************************************************
 GCLAppointment.cpp

	<Description>

	BASE CLASS = <NONE>

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GCLAppointment.h>

#include <jAssert.h>


/******************************************************************************
 Constructor

 *****************************************************************************/

GCLAppointment::GCLAppointment
	(
	const CLDay day
	)
	:
	itsStartDay(day),
	itsEndDay(day),
	itsRecurrence(NULL),
	itsStillReminding(kJFalse)
{
}

GCLAppointment::GCLAppointment
	(
	std::istream& is
	)
	:
	itsRecurrence(NULL)
{
	is >> itsStartDay;
	is >> itsEndDay;
	is >> itsStartTime;
	is >> itsEndTime;
	is >> itsSubject;
	is >> itsLocation;
	is >> itsNote;

	JBoolean hasRecur;
	is >> hasRecur;
	if (hasRecur)
		{
		itsRecurrence	= jnew GCLRecurrence(is);
		assert(itsRecurrence != NULL);
		}

	is >> itsStillReminding;
	is >> itsAlarmTimeCount;

	int timeUnit;
	is >> timeUnit;
	itsAlarmTimeUnit	= (GCLAlarm::TimeUnit)timeUnit;
	is >> itsStillReminding;
	if (itsStillReminding)
		{
		is >> itsNextAlarmTime;
		}
}


/******************************************************************************
 Destructor

 *****************************************************************************/

GCLAppointment::~GCLAppointment()
{
}

/******************************************************************************
 SetStartDay (public)

 ******************************************************************************/

void
GCLAppointment::SetStartDay
	(
	const CLDay day
	)
{
	itsStartDay	= day;
}

/******************************************************************************
 SetEndDay (public)

 ******************************************************************************/

void
GCLAppointment::SetEndDay
	(
	const CLDay day
	)
{
	itsEndDay	= day;
}

/******************************************************************************
 SetStartTime (public)

 ******************************************************************************/

void
GCLAppointment::SetStartTime
	(
	const CLTime start
	)
{
	itsStartTime	= start;
}

/******************************************************************************
 SetEndTime (public)

 ******************************************************************************/

void
GCLAppointment::SetEndTime
	(
	const CLTime end
	)
{
	itsEndTime	= end;
}

/******************************************************************************
 SetSubject (public)

 ******************************************************************************/

void
GCLAppointment::SetSubject
	(
	const JString& subject
	)
{
	itsSubject	= subject;
}

/******************************************************************************
 SetLocation (public)

 ******************************************************************************/

void
GCLAppointment::SetLocation
	(
	const JString& location
	)
{
	itsLocation	= location;
}

/******************************************************************************
 SetNote (public)

 ******************************************************************************/

void
GCLAppointment::SetNote
	(
	const JString& note
	)
{
	itsNote	= note;
}

/******************************************************************************
 CompareByStartDay (public)

 ******************************************************************************/

JOrderedSetT::CompareResult
GCLAppointment::CompareByStartDay
	(
	GCLAppointment * const & a1,
	GCLAppointment * const & a2
	)
{
	return CLCompareDays(a1->GetStartDay(), a2->GetStartDay());
}

/******************************************************************************
 CompareByEndDay (public)

 ******************************************************************************/

JOrderedSetT::CompareResult
GCLAppointment::CompareByEndDay
	(
	GCLAppointment * const & a1,
	GCLAppointment * const & a2
	)
{
	return CLCompareDays(a1->GetEndDay(), a2->GetEndDay());
}

/******************************************************************************
 CompareByStart (public)

 ******************************************************************************/

JOrderedSetT::CompareResult
GCLAppointment::CompareByTime
	(
	GCLAppointment * const & a1,
	GCLAppointment * const & a2
	)
{
	if (a1->GetEndTime().hour == a2->GetEndTime().hour)
		{
		if (a1->GetEndTime().minute == a2->GetEndTime().minute)
			{
			return JOrderedSetT::kFirstEqualSecond;
			}
		else if (a1->GetEndTime().minute > a2->GetEndTime().minute)
			{
			return JOrderedSetT::kFirstGreaterSecond;
			}
		else
			{
			return JOrderedSetT::kFirstLessSecond;
			}
		}
	else if (a1->GetEndTime().hour > a2->GetEndTime().hour)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	return JOrderedSetT::kFirstLessSecond;
}

/******************************************************************************
 SetRecurrence (public)

 ******************************************************************************/

void
GCLAppointment::SetRecurrence
	(
	GCLRecurrence recurrence
	)
{
	if (itsRecurrence == NULL)
		{
		itsRecurrence	= jnew GCLRecurrence;
		assert(itsRecurrence != NULL);
		}
	*itsRecurrence	= recurrence;
}

/******************************************************************************
 ShouldStillRemind (public)

 ******************************************************************************/

void
GCLAppointment::ShouldStillRemind
	(
	const JBoolean remind
	)
{
	itsStillReminding	= remind;
}

/******************************************************************************
 WriteSetup (public

 ******************************************************************************/

void
GCLAppointment::WriteSetup
	(
	std::ostream& os
	)
{
	os << itsStartDay << ' ';
	os << itsEndDay << ' ';
	os << itsStartTime << ' ';
	os << itsEndTime << ' ';
	os << itsSubject << ' ';
	os << itsLocation << ' ';
	os << itsNote << ' ';

	JBoolean hasRecur	= JI2B(itsRecurrence != NULL);
	os << hasRecur << ' ';
	if (hasRecur)
		{
		itsRecurrence->WriteSetup(os);
		}

	os << itsStillReminding << ' ';
	os << itsAlarmTimeCount << ' ';
	os << (int)itsAlarmTimeUnit << ' ';
	os << itsStillReminding << ' ';
	if (itsStillReminding)
		{
		os << itsNextAlarmTime << ' ';
		}
}

/******************************************************************************
 SetAlarmTime (public)

 ******************************************************************************/

void
GCLAppointment::SetAlarmTime
	(
	const JIndex time
	)
{
	itsNextAlarmTime	= time;
}
