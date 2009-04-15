/******************************************************************************
 GCLAppointment.h

	Interface for the GCLAppointment class

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GCLAppointment
#define _H_GCLAppointment

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <GCLAlarm.h>
#include <GCLRecurrence.h>
#include <GCLUtil.h>
#include <JString.h>



class GCLAppointment
{
public:

	GCLAppointment(const CLDay day);
	GCLAppointment(istream& is);
	virtual ~GCLAppointment();

	void	WriteSetup(ostream& os);

	void	SetStartDay(const CLDay day);
	CLDay	GetStartDay() const;

	void	SetEndDay(const CLDay day);
	CLDay	GetEndDay() const;

	void	SetStartTime(const CLTime start);
	CLTime	GetStartTime() const;

	void	SetEndTime(const CLTime end);
	CLTime	GetEndTime() const;

	void	SetDefaultAlarm(const JIndex count, const GCLAlarm::TimeUnit unit);
	void	GetDefaultAlarm(JIndex* count, GCLAlarm::TimeUnit* unit) const;

	void			SetSubject(const JString& subject);
	const JString&	GetSubject() const;

	void			SetLocation(const JString& location);
	const JString&	GetLocation() const;

	void			SetNote(const JString& note);
	const JString&	GetNote() const;

	void			SetRecurrence(GCLRecurrence recurrence);
	JBoolean		GetRecurrence(GCLRecurrence* recurrence);

	void			ShouldStillRemind(const JBoolean remind);
	JBoolean		IsStillReminding() const;

	void			SetAlarmTime(const JIndex time);
	JIndex			GetAlarmTime() const;

	void			ReadSetup();
	void			WriteSetup();

	static JOrderedSetT::CompareResult
		CompareByStartDay(GCLAppointment * const & a1, GCLAppointment * const & a2);

	static JOrderedSetT::CompareResult
		CompareByEndDay(GCLAppointment * const & a1, GCLAppointment * const & a2);

	static JOrderedSetT::CompareResult
		CompareByTime(GCLAppointment * const & a1, GCLAppointment * const & a2);

protected:

private:

	CLDay		itsStartDay;
	CLDay		itsEndDay;
	CLTime		itsStartTime;
	CLTime		itsEndTime;
	JString		itsSubject;
	JString	itsLocation;
	JString	itsNote;

	GCLRecurrence*		itsRecurrence;
	JBoolean			itsStillReminding;
	JIndex				itsAlarmTimeCount;
	GCLAlarm::TimeUnit	itsAlarmTimeUnit;
	JIndex				itsNextAlarmTime;

private:

	// not allowed

	GCLAppointment(const GCLAppointment& source);
	const GCLAppointment& operator=(const GCLAppointment& source);
};

/******************************************************************************
 GetStartDay (public)

 ******************************************************************************/

inline CLDay
GCLAppointment::GetStartDay()
	const
{
	return itsStartDay;
}

/******************************************************************************
 GetEndDay (public)

 ******************************************************************************/

inline CLDay
GCLAppointment::GetEndDay()
	const
{
	return itsEndDay;
}

/******************************************************************************
 GetStartTime (public)

 ******************************************************************************/

inline CLTime
GCLAppointment::GetStartTime()
	const
{
	return itsStartTime;
}

/******************************************************************************
 GetEndTime (public)

 ******************************************************************************/

inline CLTime
GCLAppointment::GetEndTime()
	const
{
	return itsEndTime;
}

/******************************************************************************
 GetSubject

 ******************************************************************************/

inline const JString&
GCLAppointment::GetSubject()
	const
{
	return itsSubject;
}

/******************************************************************************
 GetLocation

 ******************************************************************************/

inline const JString&
GCLAppointment::GetLocation()
	const
{
	return itsLocation;
}

/******************************************************************************
 GetNote

 ******************************************************************************/

inline const JString&
GCLAppointment::GetNote()
	const
{
	return itsNote;
}

/******************************************************************************
 GetRecurrence

 ******************************************************************************/

inline JBoolean
GCLAppointment::GetRecurrence
	(
	GCLRecurrence* recurrence
	)
{
	if (itsRecurrence == NULL)
		{
		return kJFalse;
		}
	*recurrence = *itsRecurrence;
	return kJTrue;
}

/******************************************************************************
 IsStillReminding (public)

 ******************************************************************************/

inline JBoolean
GCLAppointment::IsStillReminding()
	const
{
	return itsStillReminding;
}

/******************************************************************************
 DefaultAlarm (public)

 ******************************************************************************/

inline void
GCLAppointment::SetDefaultAlarm
	(
	const JIndex				count,
	const GCLAlarm::TimeUnit	unit
	)
{
	itsAlarmTimeCount	= count;
	itsAlarmTimeUnit	= unit;
}

inline void
GCLAppointment::GetDefaultAlarm
	(
	JIndex*			count,
	GCLAlarm::TimeUnit*	unit
	)
	const
{
	*count	= itsAlarmTimeCount;
	*unit	= itsAlarmTimeUnit;
}

/******************************************************************************
 GetAlarmTime (public)

 ******************************************************************************/

inline JIndex
GCLAppointment::GetAlarmTime()
	const
{
	return itsNextAlarmTime;
}


#endif
