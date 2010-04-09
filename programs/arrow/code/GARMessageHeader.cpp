/******************************************************************************
 GARMessageHeader.cc

	BASE CLASS = <NONE>

	Copyright © 2001 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

//Class Header
#include <GARMessageHeader.h>

#include <JRegex.h>
#include <JPtrArray.h>

#include <gMailUtils.h>

#include <ctype.h>
#include <time.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GARMessageHeader::GARMessageHeader()
{
	itsMessageStatus	= kNew;
	itsHasAttachment	= kJFalse;
	itsReplyStatus		= kNoReply;

	// these need to be initialized in case there is no Date header
	itsYear				= 0;
	itsDay				= 0;

}

/******************************************************************************
 Destructor

 *****************************************************************************/

GARMessageHeader::~GARMessageHeader()
{
}

/******************************************************************************
 SetHeader

 *****************************************************************************/

static const JRegex yearRegex("[[:space:]]+([0-9]+)$");
static const JRegex timeRegex1("([0-9][0-9]:[0-9][0-9]:[0-9][0-9])[[:space:]]+([0-9]+)");
static const JRegex timeRegex2("([0-9][0-9]:[0-9][0-9])[[:space:]]+([0-9]+)");
static const JRegex dateRegex("(Mon|Tue|Wed|Thu|Fri|Sat|Sun)[[:space:]]+(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)[[:space:]]+([0-9]+)");

void
GARMessageHeader::SetHeader
	(
	const JString& header
	)
{
	itsHeader = header;
	itsHeader.TrimWhitespace();
	JArray<JIndexRange> subList;
	JBoolean matched = yearRegex.Match(itsHeader, &subList);
	if (matched)
		{
		JIndexRange sRange = subList.GetElement(2);
		JString year = itsHeader.GetSubstring(sRange);
		year.ConvertToUInt(&itsYear);
		}

	matched = timeRegex1.Match(itsHeader, &subList);
	if (matched)
		{
		JIndexRange sRange = subList.GetElement(2);
		itsTime = itsHeader.GetSubstring(sRange);
		if (itsYear == 0)
			{
			sRange = subList.GetElement(3);
			JString year = itsHeader.GetSubstring(sRange);
			year.ConvertToUInt(&itsYear);
			}
		}
	else
		{
		matched = timeRegex2.Match(itsHeader, &subList);
		if (matched)
			{
			JIndexRange sRange = subList.GetElement(2);
			itsTime = itsHeader.GetSubstring(sRange);
			if (itsYear == 0)
				{
				sRange = subList.GetElement(3);
				JString year = itsHeader.GetSubstring(sRange);
				year.ConvertToUInt(&itsYear);
				}
			}
		}
	matched = dateRegex.Match(itsHeader, &subList);
	if (matched)
		{
		JIndexRange sRange = subList.GetElement(2);
		JString dow	= itsHeader.GetSubstring(sRange);
		sRange		= subList.GetElement(3);
		itsMonth	= itsHeader.GetSubstring(sRange);
		sRange = subList.GetElement(4);
		JString day = itsHeader.GetSubstring(sRange);
		day.ConvertToUInt(&itsDay);
		itsShortDate	= itsMonth + " " + day;

		time_t now;
		struct tm *local_time;
		time(&now);
		local_time = localtime(&now);
		if (itsYear != (JIndex)(local_time->tm_year + 1900))
			{
			JString year	= JString(itsYear, JString::kBase10);
			itsShortDate += " " + year;
			}

		itsDate			= dow + ", " + day + " " + itsMonth + " " + JString(itsYear) + " " + itsTime;
		}
}

/******************************************************************************
 SetFrom

 *****************************************************************************/

void
GARMessageHeader::SetFrom
	(
	const JString& from
	)
{
	itsFrom = from;
	JPtrArray<JString> names(JPtrArrayT::kForgetAll);
	GGetRealNames(itsFrom, names);
	if (names.GetElementCount() > 0)
		{
		itsFromNames = *(names.NthElement(1));
		for (JSize i = 2; i <= names.GetElementCount(); i++)
			{
			itsFromNames += ", " + *(names.NthElement(i));
			}
		}
	else
		{
		itsFromNames = from;
		}
	names.DeleteAll();
}

/******************************************************************************
 SetAddress

 *****************************************************************************/

void
GARMessageHeader::SetAddress
	(
	const JString& address
	)
{
	itsAddress = address;
}

/******************************************************************************
 SetSubject

 *****************************************************************************/

void
GARMessageHeader::SetSubject
	(
	const JString& subject
	)
{
	itsSubject = subject;
	itsSubject.TrimWhitespace();
}

/******************************************************************************
 SetDate

 *****************************************************************************/

static const JRegex monthRegex1("^[a-zA-Z]+,[[:space:]]+([0-9]+)[[:space:]]+([a-zA-Z]+)");
static const JRegex monthRegex2("^([0-9]+)[[:space:]]+([a-zA-Z]+)");

void
GARMessageHeader::SetDate
	(
	const JString& date
	)
{
	itsDate = date;
	itsDate.TrimWhitespace();
	JArray<JIndexRange> subList;
	JString year	= JString(itsYear, JString::kBase10);
	JBoolean matched = monthRegex1.Match(itsDate, &subList);
	if (matched)
		{
		JIndexRange sRange = subList.GetElement(3);
		itsMonth = itsDate.GetSubstring(sRange);
		sRange = subList.GetElement(2);
		JString day = itsDate.GetSubstring(sRange);
		if (!day.IsEmpty())
			{
			if (day.GetFirstCharacter() == '0')
				{
				day.RemoveSubstring(1,1);
				}
			}
		day.ConvertToUInt(&itsDay);
		itsShortDate = itsMonth + " " + day;
		time_t now;
		struct tm *local_time;
		time(&now);
		local_time = localtime(&now);
		if (itsYear != (JIndex)(local_time->tm_year + 1900))
			{
			JString year	= JString(itsYear, JString::kBase10);
			itsShortDate += " " + year;
			}

		return;
		}
	matched = monthRegex2.Match(itsDate, &subList);
	if (matched)
		{
		JIndexRange sRange = subList.GetElement(3);
		itsMonth = itsDate.GetSubstring(sRange);
		sRange = subList.GetElement(2);
		JString day = itsDate.GetSubstring(sRange);
		if (!day.IsEmpty())
			{
			if (day.GetFirstCharacter() == '0')
				{
				day.RemoveSubstring(1,1);
				}
			}
		day.ConvertToUInt(&itsDay);
		itsShortDate = itsMonth + " " + day;
		time_t now;
		struct tm *local_time;
		time(&now);
		local_time = localtime(&now);
		if (itsYear != (JIndex)(local_time->tm_year + 1900))
			{
			JString year	= JString(itsYear, JString::kBase10);
			itsShortDate += " " + year;
			}
		return;
		}
	else
		{
		itsShortDate = itsDate;
		}
}

/******************************************************************************
 SetReplyTo

 *****************************************************************************/

void
GARMessageHeader::SetReplyTo
	(
	const JString& replyto
	)
{
	itsReplyTo = replyto;
}

/******************************************************************************
 SetCC

 *****************************************************************************/

void
GARMessageHeader::SetCC
	(
	const JString& cc
	)
{
	itsCC = cc;
}

/******************************************************************************
 SetTo

 *****************************************************************************/

void
GARMessageHeader::SetTo
	(
	const JString& to
	)
{
	itsTo = to;
	itsTo.TrimWhitespace();
}

/******************************************************************************
 SetMessageStatus

 *****************************************************************************/

void
GARMessageHeader::SetMessageStatus
	(
	const MessageStatus status
	)
{
	itsMessageStatus = status;
}

/******************************************************************************
 SetHasAttachment (public)

 ******************************************************************************/

void
GARMessageHeader::SetHasAttachment
	(
	const JBoolean attachment
	)
{
	itsHasAttachment	= attachment;
}

/******************************************************************************
 SetReplyStatus (public)

 ******************************************************************************/

void
GARMessageHeader::SetReplyStatus
	(
	const ReplyStatus status
	)
{
	itsReplyStatus	= status;
}

/******************************************************************************
 SetAccount

 *****************************************************************************/

void
GARMessageHeader::SetAccount
	(
	const JString& account
	)
{
	itsAccount = account;
	itsAccount.TrimWhitespace();
}

/******************************************************************************
 GetHeader

 *****************************************************************************/

const JString&
GARMessageHeader::GetHeader()
	const
{
	return itsHeader;
}

/******************************************************************************
 GetFrom

 *****************************************************************************/

const JString&
GARMessageHeader::GetFrom()
	const
{
	return itsFrom;
}

/******************************************************************************
 GetFromNames

 *****************************************************************************/

const JString&
GARMessageHeader::GetFromNames()
	const
{
	return itsFromNames;
}

/******************************************************************************
 GetTo

 *****************************************************************************/

const JString&
GARMessageHeader::GetTo()
	const
{
	return itsTo;
}

/******************************************************************************
 GetAddress

 *****************************************************************************/

const JString&
GARMessageHeader::GetAddress()
	const
{
	return itsAddress;
}


/******************************************************************************
 GetSubject

 *****************************************************************************/

const JString&
GARMessageHeader::GetSubject()
	const
{
	return itsSubject;
}

/******************************************************************************
 GetDate

 *****************************************************************************/

const JString&
GARMessageHeader::GetDate()
	const
{
	return itsDate;
}

/******************************************************************************
 GetShortDate

 *****************************************************************************/

const JString&
GARMessageHeader::GetShortDate()
	const
{
	return itsShortDate;
}

/******************************************************************************
 GetMonth

 *****************************************************************************/

const JString&
GARMessageHeader::GetMonth()
	const
{
	return itsMonth;
}

/******************************************************************************
 GetYear

 *****************************************************************************/

JIndex
GARMessageHeader::GetYear()
	const
{
	return itsYear;
}

/******************************************************************************
 GetDay

 *****************************************************************************/

JIndex
GARMessageHeader::GetDay()
	const
{
	return itsDay;
}

/******************************************************************************
 GetTime

 *****************************************************************************/

const JString&
GARMessageHeader::GetTime()
	const
{
	return itsTime;
}

/******************************************************************************
 GetReplyTo

 *****************************************************************************/

const JString&
GARMessageHeader::GetReplyTo()
	const
{
	return itsReplyTo;
}

/******************************************************************************
 GetCC

 *****************************************************************************/

const JString&
GARMessageHeader::GetCC()
	const
{
	return itsCC;
}

/******************************************************************************
 HasAttachment (public)

 ******************************************************************************/

JBoolean
GARMessageHeader::HasAttachment()
	const
{
	return itsHasAttachment;
}

/******************************************************************************
 GetReplyStatus (public)

 ******************************************************************************/

GARMessageHeader::ReplyStatus
GARMessageHeader::GetReplyStatus()
	const
{
	return itsReplyStatus;
}

/******************************************************************************
 GetAccount

 *****************************************************************************/

const JString&
GARMessageHeader::GetAccount()
	const
{
	return itsAccount;
}
