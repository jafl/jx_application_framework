/******************************************************************************
 GCLAlarm.h

	Interface for the GCLAlarm class

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GCLAlarm
#define _H_GCLAlarm

#include <GCLUtil.h>

class GCLAppointment;

class GCLAlarm
{
public:

	enum TimeUnit
	{
		kMinute = 1,
		kHour,
		kDay,
		kWeek,
		kMonth
	};


public:

	GCLAlarm(const JIndex count, const TimeUnit unit, GCLAppointment* appt,
			 const JBoolean timeFromNow = kJFalse);
	GCLAlarm(const JIndex count, const TimeUnit unit, GCLAppointment* appt,
			 const CLDay day, const CLTime startTime,
			 const JBoolean timeFromNow = kJFalse);
	virtual ~GCLAlarm();

	GCLAppointment*		GetAppt();
	JIndex				GetAlarmTime() const;
	CLDay				GetApptDay() const;
	CLTime				GetApptTime() const;

	static JOrderedSetT::CompareResult
		CompareTimes(GCLAlarm * const & a1, GCLAlarm * const & a2);

private:

	GCLAppointment*		itsAppt;
	JIndex				itsAlarmTime;
	CLDay				itsApptDay;
	CLTime				itsApptTime;

private:

	void GCLAlarmX(const JIndex count, const TimeUnit unit, GCLAppointment* appt,
				   const CLDay day, const CLTime startTime,
				   const JBoolean timeFromNow);

	// not allowed

	GCLAlarm(const GCLAlarm& source);
	const GCLAlarm& operator=(const GCLAlarm& source);

};

/******************************************************************************
 GetAppt() (public)

 ******************************************************************************/

inline GCLAppointment*
GCLAlarm::GetAppt()
{
	return itsAppt;
}

/******************************************************************************
 GetAlarmTime (public)

 ******************************************************************************/

inline JIndex
GCLAlarm::GetAlarmTime()
	const
{
	return itsAlarmTime;
}

/******************************************************************************
 GetApptDay (public)

 ******************************************************************************/

inline CLDay
GCLAlarm::GetApptDay()
	const
{
	return itsApptDay;
}

/******************************************************************************
 GetApptTime (public)

 ******************************************************************************/

inline CLTime
GCLAlarm::GetApptTime()
	const
{
	return itsApptTime;
}

#endif
