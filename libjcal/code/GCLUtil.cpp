/******************************************************************************
 GCLUtil.cpp

	Utilities and definitions for calendar

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 ******************************************************************************/

#include "GCLUtil.h"
#include "GCLAppointment.h"

#include <JMinMax.h>

#include <jMath.h>

#include <cal.h>
#include <jAssert.h>

static const char* gcl_months[] =
	{"", "January", "February", "March", "April", "May", "June", "July",
	"August", "September", "October", "November", "December"};

static const char* gcl_days[] =
	{"", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
	 "Saturday"};

static const char* gcl_ord[] =
	{"", "first", "second", "third", "fourth", "fifth"};

static const JSize kDaysPerWeek			= 7;

static const JSize kMaxApptLookAhead	= 400;

/******************************************************************************
 GetMonthString

 ******************************************************************************/

const JCharacter*
GetMonthString
	(
	const CLMonth month
	)
{
	return gcl_months[month];
}

/******************************************************************************
 GetDayString

 ******************************************************************************/

const JCharacter*
GetDayString
	(
	const JIndex day
	)
{
	return gcl_days[day - JLFloor(day/7) * 7];
}

/******************************************************************************
 GetOrdString

 ******************************************************************************/

const JCharacter*
GetOrdString
	(
	const JIndex day
	)
{
	JIndex ordDay	= (JIndex)(day/7) + 1;
	return gcl_ord[ordDay];
}

/******************************************************************************
 CLCompareDays

 ******************************************************************************/

JOrderedSetT::CompareResult
CLCompareDays
	(
	CLDay const & d1,
	CLDay const & d2
	)
{
	if (d1.year == d2.year)
		{
		if (d1.month == d2.month)
			{
			if (d1.day == d2.day)
				{
				return JOrderedSetT::kFirstEqualSecond;
				}
			else if (d1.day > d2.day)
				{
				return JOrderedSetT::kFirstGreaterSecond;
				}
			else
				{
				return JOrderedSetT::kFirstLessSecond;
				}
			}
		else if (d1.month > d2.month)
			{
			return JOrderedSetT::kFirstGreaterSecond;
			}
		else
			{
			return JOrderedSetT::kFirstLessSecond;
			}
		}
	else if (d1.year > d2.year)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	return JOrderedSetT::kFirstLessSecond;
}

/******************************************************************************
 operator==

 ******************************************************************************/

int
operator==
	(
	const CLDay d1,
	const CLDay d2
	)
{
	if ((d1.day == d2.day) &&
		(d1.month == d2.month) &&
		(d1.year == d2.year))
		{
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 operator<

 ******************************************************************************/

int
operator<
	(
	const CLDay d1,
	const CLDay d2
	)
{
	JOrderedSetT::CompareResult r	= CLCompareDays(d1, d2);
	if (r == JOrderedSetT::kFirstLessSecond)
		{
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 operator>

 ******************************************************************************/

int
operator>
	(
	const CLDay d1,
	const CLDay d2
	)
{
	JOrderedSetT::CompareResult r	= CLCompareDays(d1, d2);
	if (r == JOrderedSetT::kFirstGreaterSecond)
		{
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 GetTimeString

 ******************************************************************************/

JString
GetTimeString
	(
	const CLTime time
	)
{
	JBoolean ustime	= kJTrue;
	JBoolean pm		= kJFalse;
	JString str;
	JIndex hour		= time.hour;
	if (ustime)
		{
		if (hour >= 12)
			{
			pm	= kJTrue;
			}
		if (hour > 12)
			{
			hour	= hour - 12;
			}
		}
	str = JString(hour) + ":";
	if (time.minute < 10)
		{
		str += "0";
		}
	str += JString(time.minute);
	if (ustime)
		{
		if (pm)
			{
			str += "pm";
			}
		else
			{
			str += "am";
			}
		}
	return str;
}

/******************************************************************************
 GetDayIndex

 ******************************************************************************/

JBoolean
GetDayIndex
	(
	const CLDay day,
	const CLDay base,
	JIndex*	index
	)
{
	JIndex testIndex;
	if (!GetEpochIndex(day, &testIndex))
		{
		return kJFalse;
		}

	JIndex baseIndex;
	if (!GetEpochIndex(base, &baseIndex))
		{
		return kJFalse;
		}

	if (baseIndex > testIndex)
		{
		return kJFalse;
		}
	*index				= testIndex - baseIndex + 1;
	return kJTrue;
}

/******************************************************************************
 GetEpochIndex

 ******************************************************************************/

JBoolean
GetEpochIndex
	(
	const CLDay day,
	JIndex*	index
	)
{
	struct tm local_time;

	local_time.tm_sec	= 0;
	local_time.tm_min	= 0;
	local_time.tm_hour	= 0;
	local_time.tm_mday	= day.day;
	local_time.tm_mon	= day.month - 1;
	local_time.tm_year	= day.year - 1900;
	local_time.tm_isdst	= - 1;

	time_t epochTime	= mktime(&local_time);
	if (epochTime == -1)
		{
		return kJFalse;
		}

	*index				= (JIndex)epochTime/60/60/24;
	return kJTrue;
}

/******************************************************************************
 GetEpochDay

 ******************************************************************************/

JBoolean
GetEpochDay
	(
	const JIndex	index,
	CLDay*			day
	)
{
	time_t aTime	= (index + 1) * 60 * 60 * 24;

	struct tm* local_time	= localtime(&aTime);

	day->day	= local_time->tm_mday;
	day->month	= (CLMonth)(local_time->tm_mon + 1);
	day->year	= local_time->tm_year + 1900;

	return kJTrue;
}

/******************************************************************************
 AppointmentInDay

 ******************************************************************************/

JBoolean
AppointmentInDay
	(
	GCLAppointment*	appt,
	const CLDay	day
	)
{
	GCLRecurrence r;
	JBoolean ok = appt->GetRecurrence(&r);
	assert(ok);

	if (r.IsSkippingDay(day))
		{
		return kJFalse;
		}

	CLDay aDay		= appt->GetStartDay();

	if (r.frame == kDaily)
		{
		if (r.skipInterval > 1)
			{
			JIndex start;
			if (!GetEpochIndex(aDay, &start))
				{
				return kJFalse;
				}
			JIndex current;
			if (!GetEpochIndex(day, &current))
				{
				return kJFalse;
				}
			assert(current >= start);
			JIndex delta	= current - start;
			if (JRound(delta/r.skipInterval) * r.skipInterval != delta)
				{
				return kJFalse;
				}
			}
		return kJTrue;
		}
	else if (r.frame == kWeekly)
		{
		int diw	= day_in_week(day.day, day.month, day.year);
		if (!r.days[diw])
			{
			return kJFalse;
			}
		if (r.skipInterval > 1)
			{
			JIndex start;
			if (!GetEpochIndex(aDay, &start))
				{
				return kJFalse;
				}
			JIndex current;
			if (!GetEpochIndex(day, &current))
				{
				return kJFalse;
				}
			assert(current >= start);

			int adiw = day_in_week(aDay.day, aDay.month, aDay.year);
			start	-= adiw;
			current	-= diw;
			JIndex delta	= (current - start)/7;
			if (JRound(delta/r.skipInterval) * r.skipInterval != delta)
				{
				return kJFalse;
				}
			}
		return kJTrue;
		}
	else if (r.frame == kMonthly)
		{
		if (r.useOrd)
			{
			JIndex ordDay	= (JIndex)(day.day/7) + 1;
			if (r.ord != ordDay)
				{
				return kJFalse;
				}
			int diw	= day_in_week(day.day, day.month, day.year);
			if (r.day != (JIndex)(diw + 1))
				{
				return kJFalse;
				}
			}
		else if (r.date != day.day)
			{
			return kJFalse;
			}
		if (r.skipInterval > 1)
			{
			JIndex delta	= (day.year - aDay.year)*12 + day.month - aDay.month;
			if (JRound(delta/r.skipInterval) * r.skipInterval != delta)
				{
				return kJFalse;
				}
			}
		return kJTrue;
		}
	else if (r.frame == kYearly)
		{
		if (r.month != day.month)
			{
			return kJFalse;
			}
		if (r.useOrd)
			{
			JIndex ordDay	= (JIndex)(day.day/7) + 1;
			if (r.ord != ordDay)
				{
				return kJFalse;
				}
			int diw	= day_in_week(day.day, day.month, day.year);
			if (r.day != (JIndex)(diw + 1))
				{
				return kJFalse;
				}
			}
		else if (r.date != day.day)
			{
			return kJFalse;
			}
		if (r.skipInterval > 1)
			{
			JIndex delta	= day.year - aDay.year;
			if (JRound(delta/r.skipInterval) * r.skipInterval != delta)
				{
				return kJFalse;
				}
			}
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 GetNextAppointmentDay

 ******************************************************************************/

JBoolean
GetNextAppointmentDay
	(
	GCLAppointment*	appt,
	const CLDay	start,
	const CLTime	startTime,
	const JBoolean	after,
	CLDay*			day
	)
{
	JBoolean realAfter	= after;

	CLDay today	= GetCurrentDate();
	CLDay end	= appt->GetEndDay();

	JIndex sIndex;
	if (!GetEpochIndex(start, &sIndex))
		{
		return kJFalse;
		}

	JIndex tIndex;
	if (!GetEpochIndex(today, &tIndex))
		{
		return kJFalse;
		}

	GCLRecurrence r;
	if (!appt->GetRecurrence(&r))
		{
		if (today < start ||
			(today == start &&
			 GetCurrentEpochTime() <= GetEpochTime(start, startTime)))
			{
			*day	= start;
			return kJTrue;
			}
		return kJFalse;
		}

	if (r.type	== kNoRepeat && today > start)
		{
		return kJFalse;
		}

	if (r.type	== kHasEnd && today > end)
		{
		return kJFalse;
		}

	JIndex startIndex	= sIndex;

	if (tIndex > sIndex)
		{
		startIndex	= tIndex;
		realAfter	= kJFalse;
		}

	CLDay startDay;
	if (!GetEpochDay(startIndex, &startDay))
		{
		return kJFalse;
		}

	if (realAfter || GetCurrentEpochTime() > GetEpochTime(startDay, startTime))
		{
		startIndex ++;
		}

	JIndex endIndex	= 0;
	if (r.type == kHasEnd)
		{
		if (!GetEpochIndex(r.endDay, &endIndex))
			{
			return kJFalse;
			}
		endIndex	= JMax(endIndex, startIndex + kMaxApptLookAhead);
		}
	else
		{
		endIndex	= startIndex + kMaxApptLookAhead;
		}
	for (JIndex i = startIndex; i <= endIndex; i++)
		{
		CLDay testDay;
		if (!GetEpochDay(i, &testDay))
			{
			return kJFalse;
			}
		if (AppointmentInDay(appt, testDay))
			{
			*day	= testDay;
			return kJTrue;
			}
		}
	return kJFalse;

/*
	if (r.frame == kDaily)
		{
		if (today < start)
			{
			*day	= start;
			return kJTrue;
			}

		JIndex delta	= tIndex - sIndex;
		JIndex aIndex	= sIndex + JLCeil(delta/r.skipInterval) * r.skipInterval;
		time_t aTime	= aIndex * 60 * 60 * 24;

		struct tm* local_time	= localtime(&aTime);

		day->day	= local_time->tm_mday;
		day->month	= (CLMonth)(local_time->tm_mon + 1);
		day->year	= local_time->tm_year + 1900;

		return kJTrue;
		}
	else if (r.frame == kWeekly)
		{
		int sdiw	= day_in_week(start.day, start.month, start.year);
		JIndex sSun = sIndex - sdiw;

		int tdiw	= day_in_week(today.day, today.month, today.year);
		JIndex tSun = tIndex - tdiw;

		// first check if they are in the same week
		if (sSun >= tSun)
			{
			// check for same day
			if (sIndex == tIndex && !after)
				{
				// is this a valid day
				if (r.days[sdiw])
					{
					// ok, it's valid. has the appt passed?
					if (GetCurrentEpochTime() <= GetEpochTime(start, startTime))
						{
						*day	= today;
						return kJTrue;
						}
					}
				for (JIndex i = sdiw + 1; i < kDaysPerWeek; i++)
					{
					if (r.days[i] && GetEpochDay(sSun + i, day))
						{
						return kJTrue;
						}
					}
				}
			else if (sIndex > tIndex && !after)
				{
				for (JIndex i = sdiw; i < kDaysPerWeek; i++)
					{
					if (r.days[i] && GetEpochDay(sSun + i, day))
						{
						return kJTrue;
						}
					}
				}
			else if (tIndex > sIndex)
				{
				for (JIndex i = tdiw; i < kDaysPerWeek; i++)
					{
					if (r.days[i] && GetEpochDay(tSun + i, day))
						{
						return kJTrue;
						}
					}
				}

			JIndex aSun	= sSun + r.skipInterval * kDaysPerWeek;
			for (JIndex i = 0; i < kDaysPerWeek; i++)
				{
				if (r.days[i] && GetEpochDay(aSun + i, day))
					{
					return kJTrue;
					}
				}
			}
		else // tSun > sSun - ie. We are after the first week
			{
			JSize delta		= (tSun - sSun)/kDaysPerWeek;
			JIndex aIndex	= sIndex + (JLCeil(delta/r.skipInterval) * r.skipInterval) * kDaysPerWeek;
			time_t aTime	= aIndex * 60 * 60 * 24;

			struct tm* local_time	= localtime(&aTime);

			day->day	= local_time->tm_mday;
			day->month	= (CLMonth)(local_time->tm_mon + 1);
			day->year	= local_time->tm_year + 1900;

			return kJTrue;
			}
		}
	else if (r.frame == kMonthly)
		{
		}
	else
		{
		}
	return kJFalse;
*/
}

/******************************************************************************
 GetNextAppointmentDay

 ******************************************************************************/

JBoolean
GetNextAppointmentDay
	(
	GCLAppointment*	appt,
	CLDay*			day
	)
{
	return
		GetNextAppointmentDay(appt, appt->GetStartDay(),
							  appt->GetStartTime(), kJFalse, day);
}

/******************************************************************************
 GetCurrentEpochTime

 ******************************************************************************/

time_t
GetCurrentEpochTime()
{
	time_t now;
	time(&now);
	return now;
}

/******************************************************************************
 GetEpochTime

 ******************************************************************************/

time_t
GetEpochTime
	(
	const CLDay	day,
	const CLTime	appTime
	)
{
	struct tm local_time;

	local_time.tm_sec	= 0;
	local_time.tm_min	= appTime.minute;
	local_time.tm_hour	= appTime.hour;
	local_time.tm_mday	= day.day;
	local_time.tm_mon	= day.month - 1;
	local_time.tm_year	= day.year - 1900;
	local_time.tm_isdst	= - 1;

	time_t epochTime	= mktime(&local_time);
	return epochTime;
}

/******************************************************************************
 GetCurrentDate

 ******************************************************************************/

CLDay
GetCurrentDate()
{
	time_t now;
	time(&now);

	struct tm* local_time	= localtime(&now);

	CLDay day;
	day.day		= local_time->tm_mday;
	day.month	= (CLMonth)(local_time->tm_mon + 1);
	day.year	= local_time->tm_year + 1900;

	return day;
}

/******************************************************************************
 GetCurrentTime

 ******************************************************************************/

CLTime
GetCurrentTime()
{
	time_t now;
	time(&now);

	struct tm* local_time	= localtime(&now);

	CLTime time;
	time.hour	= local_time->tm_hour;
	time.minute	= local_time->tm_min;

	return time;
}

/******************************************************************************
 GetDateString

 ******************************************************************************/

JString
GetDateString
	(
	const CLDay day
	)
{
	JString dateStr	= JString(GetDayString(day.day)) + ", " + JString(GetMonthString(day.month)) + " " + JString(day.day);
	return dateStr;
}

/******************************************************************************
 Stream operators for CLDay

 ******************************************************************************/

istream&
operator>>
	(
	istream&	input,
	CLDay&		day
	)
{
	int month;
	input >> day.day >> month >> day.year >> day.shaded;
	day.month	= (CLMonth)month;
	return input;
}

ostream&
operator<<
	(
	ostream&		output,
	const CLDay&	day
	)
{
	output << day.day << ' ' << (int)day.month << ' ' << day.year << ' ' << day.shaded;
	return output;
}

/******************************************************************************
 Stream operators for CLTime

 ******************************************************************************/

istream&
operator>>
	(
	istream&	input,
	CLTime&		t
	)
{
	input >> t.hour >> t.minute;
	return input;
}

ostream&
operator<<
	(
	ostream&		output,
	const CLTime&	t
	)
{
	output << t.hour << ' ' << t.minute;
	return output;
}
