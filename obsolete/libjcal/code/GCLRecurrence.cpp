/******************************************************************************
 GCLRecurrence.cpp

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include <GCLRecurrence.h>

#include <JArray.h>

#include <jAssert.h>


/******************************************************************************
 Constructor

 *****************************************************************************/

GCLRecurrence::GCLRecurrence()
{
	itsSkipDays	= NULL;
}

GCLRecurrence::GCLRecurrence
	(
	std::istream& is
	)
{
	itsSkipDays	= NULL;
	int type;
	is >> type;
	type	= (CLRecurrenceType)type;
	is >> endDay;
	int frame;
	is >> frame;
	frame	= (CLRecurrenceFrame)frame;
	is >> skipInterval;

	for (JIndex i = 0; i < 7; i++)
		{
		is >> days[i];
		}

	is >> useOrd;
	is >> date;
	is >> ord;
	is >> day;
	int month;
	is >> month;
	month	= (CLMonth)month;
	JSize count;
	is >> count;
	if (count > 0)
		{
		if (itsSkipDays == NULL)
			{
			itsSkipDays	= jnew JArray<CLDay>;
			assert(itsSkipDays != NULL);
			itsSkipDays->SetCompareFunction(CLCompareDays);
			}
		CLDay day;
		for (JIndex i = 1; i <= count; i++)
			{
			is >> day;
			itsSkipDays->InsertSorted(day);
			}

		}
}

GCLRecurrence::GCLRecurrence
	(
	const GCLRecurrence& source
	)
{
	Set(source);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GCLRecurrence::~GCLRecurrence()
{
	jdelete itsSkipDays;
}

/******************************************************************************
 operator= (public)

 ******************************************************************************/

const GCLRecurrence&
GCLRecurrence::operator=
	(
	const GCLRecurrence& source
	)
{
	Set(source);
	return *this;
}

/******************************************************************************
 Set (public)

 ******************************************************************************/

void
GCLRecurrence::Set
	(
	const GCLRecurrence& source
	)
{
	type			= source.type;
	endDay			= source.endDay;
	frame			= source.frame;
	skipInterval	= source.skipInterval;
	useOrd			= source.useOrd;
	date			= source.date;
	ord				= source.ord;
	day				= source.day;
	month			= source.month;
	type			= source.type;
	for (JIndex i = 0; i < 7; i++)
		{
		days[i]		= source.days[i];
		}
	if (source.itsSkipDays	== NULL)
		{
		itsSkipDays	= NULL;
		}
	else
		{
		itsSkipDays	= jnew JArray<CLDay>(*(source.itsSkipDays));
		assert(itsSkipDays != NULL);
		}
}

/******************************************************************************
 WriteSetup (public)

 ******************************************************************************/

void
GCLRecurrence::WriteSetup
	(
	std::ostream& os
	)
{
	os << (int)type << ' ';
	os << endDay << ' ';
	os << (int)frame << ' ';
	os << skipInterval << ' ';

	for (JIndex i = 0; i < 7; i++)
		{
		os << days[i] << ' ';
		}

	os << useOrd << ' ';
	os << date << ' ';
	os << ord << ' ';
	os << day << ' ';
	os << (int)month << ' ';

	JSize count	= 0;
	if (itsSkipDays != NULL)
		{
		count	= itsSkipDays->GetElementCount();
		}
	os << count << ' ';
	for (JIndex i = 1; i <= count; i++)
		{
		os << itsSkipDays->GetElement(i) << ' ';
		}
}


/******************************************************************************
 ShouldSkipDay (public)

 ******************************************************************************/

void
GCLRecurrence::ShouldSkipDay
	(
	const CLDay day
	)
{
	if (itsSkipDays	== NULL)
		{
		itsSkipDays	= jnew JArray<CLDay>;
		assert(itsSkipDays != NULL);
		itsSkipDays->SetCompareFunction(CLCompareDays);
		}
	itsSkipDays->InsertSorted(day);
}

/******************************************************************************
 IsSkippingDay (public)

 ******************************************************************************/

JBoolean
GCLRecurrence::IsSkippingDay
	(
	const CLDay day
	)
{
	if (itsSkipDays == NULL)
		{
		return kJFalse;
		}
	JIndex findex;
	return itsSkipDays->SearchSorted(day, JListT::kAnyMatch, &findex);
}
