/******************************************************************************
 GCLUtil.h

	Utilities and definitions for calendar

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GCLUtil
#define _H_GCLUtil

#include <JString.h>
#include <JList.h>
#include <time.h>

class GCLAppointment;

enum CLMonth
{
	kCLJanuary = 1,
	kCLFebruary,
	kCLMarch,
	kCLApril,
	kCLMay,
	kCLJune,
	kCLJuly,
	kCLAugust,
	kCLSeptember,
	kCLOctober,
	kCLNovember,
	kCLDecember
};

struct CLDay
{
	JIndex		day;
	CLMonth		month;
	JIndex		year;
	JBoolean	shaded;
};

struct CLTime
{
	JIndex		hour;
	JIndex		minute;
};

enum CLRecurrenceType
{
	kNoRepeat = 1,
	kNoEnd,
	kHasEnd
};

enum CLRecurrenceFrame
{
	kDaily = 1,
	kWeekly,
	kMonthly,
	kYearly
};

const JCharacter* GetMonthString(const CLMonth month);
const JCharacter* GetDayString(const JIndex day);
const JCharacter* GetOrdString(const JIndex day);

JString		GetTimeString(const CLTime time);
JString		GetDateString(const CLDay day);
JBoolean	GetDayIndex(const CLDay day, const CLDay base, JIndex* index);
JBoolean	GetEpochIndex(const CLDay day, JIndex* index);
JBoolean	GetEpochDay(const JIndex index, CLDay* day);
JBoolean	AppointmentInDay(GCLAppointment* appt, const CLDay day);
JBoolean	GetNextAppointmentDay(GCLAppointment* appt, CLDay* day);
JBoolean	GetNextAppointmentDay(GCLAppointment* appt, const CLDay start, const CLTime startTime, const JBoolean after, CLDay* day);

time_t		GetCurrentEpochTime();
time_t		GetEpochTime(const CLDay day, const CLTime appTime);
CLDay		GetCurrentDate();
CLTime		GetCurrentTime();

JListT::CompareResult
CLCompareDays(CLDay const & d1, CLDay const & d2);

int operator== (const CLDay d1, const CLDay d2);
int operator> (const CLDay d1, const CLDay d2);
int operator< (const CLDay d1, const CLDay d2);

std::istream& operator>>(std::istream& input, CLDay& day);
std::ostream& operator<<(std::ostream& output, const CLDay& day);

std::istream& operator>>(std::istream& input, CLTime& t);
std::ostream& operator<<(std::ostream& output, const CLTime& t);

#endif
