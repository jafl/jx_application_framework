/******************************************************************************
 GARMBoxHeader.cc

	BASE CLASS = <NONE>

	Copyright (C) 2001 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

//Class Header
#include <GARMBoxHeader.h>
#include "GMMIMEParser.h"

#include <gMailUtils.h>

#include <JRegex.h>
#include <JPtrArray.h>
#include <jStreamUtil.h>

#include <ctype.h>
#include <time.h>
#include <sstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GARMBoxHeader::GARMBoxHeader()
{
	itsMessageStatus	= kNew;
	itsIsMIME			= kJFalse;
	itsHasAttachment	= kJFalse;
	itsReplyStatus		= kNoReply;
	itsMIMEParser		= NULL;
	itsSubjectAdjusted	= kJFalse;
	itsReplyToAdjusted	= kJFalse;
	itsCCAdjusted		= kJFalse;
	itsToAdjusted		= kJFalse;

	// these need to be initialized in case there is no Date header
	itsYear				= 0;
	itsDay				= 0;

}

/******************************************************************************
 Destructor

 *****************************************************************************/

GARMBoxHeader::~GARMBoxHeader()
{
	delete itsMIMEParser;
}

/******************************************************************************
 SetHeader

 *****************************************************************************/

static const JRegex yearRegex("[[:space:]]+([0-9]+)$");
static const JRegex timeRegex1("([0-9][0-9]:[0-9][0-9]:[0-9][0-9])[[:space:]]+([0-9]+)");
static const JRegex timeRegex2("([0-9][0-9]:[0-9][0-9])[[:space:]]+([0-9]+)");
static const JRegex dateRegex("(Mon|Tue|Wed|Thu|Fri|Sat|Sun)[[:space:]]+(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)[[:space:]]+([0-9]+)");

void
GARMBoxHeader::SetHeader
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
GARMBoxHeader::SetFrom
	(
	const JString& from
	)
{
	itsFrom = from;
	DecodeMIMEHeader(&itsFrom);
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
GARMBoxHeader::SetAddress
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
GARMBoxHeader::SetSubject
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
GARMBoxHeader::SetDate
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
GARMBoxHeader::SetReplyTo
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
GARMBoxHeader::SetCC
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
GARMBoxHeader::SetTo
	(
	const JString& to
	)
{
	itsTo = to;
	itsTo.TrimWhitespace();
}

/******************************************************************************
 SetHeaderStart

 *****************************************************************************/

void
GARMBoxHeader::SetHeaderStart
	(
	const JIndex start
	)
{
	itsHeaderStart = start;
}

/******************************************************************************
 SetHeaderEnd

 *****************************************************************************/

void
GARMBoxHeader::SetHeaderEnd
	(
	const JIndex end
	)
{
	itsHeaderEnd = end;
}

/******************************************************************************
 SetMessageEnd

 *****************************************************************************/

void
GARMBoxHeader::SetMessageEnd
	(
	const JIndex end
	)
{
	itsMessageEnd = end;
}

/******************************************************************************
 SetMessageStatus

 *****************************************************************************/

void
GARMBoxHeader::SetMessageStatus
	(
	const MessageStatus status
	)
{
	itsMessageStatus = status;
}

/******************************************************************************
 SetIsMIME (public)

 ******************************************************************************/


void
GARMBoxHeader::SetIsMIME
	(
	const JBoolean mime
	)
{
	itsIsMIME	= mime;
}

/******************************************************************************
 SetHasAttachment (public)

 ******************************************************************************/

void
GARMBoxHeader::SetHasAttachment
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
GARMBoxHeader::SetReplyStatus
	(
	const ReplyStatus status
	)
{
	itsReplyStatus	= status;
}

/******************************************************************************
 SetMIMEParser (public)

 ******************************************************************************/

void
GARMBoxHeader::SetMIMEParser
	(
	GMMIMEParser* parser
	)
{
	itsMIMEParser	= parser;
}

/******************************************************************************
 SetAccount

 *****************************************************************************/

void
GARMBoxHeader::SetAccount
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
GARMBoxHeader::GetHeader()
	const
{
	return itsHeader;
}

/******************************************************************************
 GetFrom

 *****************************************************************************/

const JString&
GARMBoxHeader::GetFrom()
	const
{
	return itsFrom;
}

/******************************************************************************
 GetFromNames

 *****************************************************************************/

const JString&
GARMBoxHeader::GetFromNames()
	const
{
	return itsFromNames;
}

/******************************************************************************
 GetTo

 *****************************************************************************/

const JString&
GARMBoxHeader::GetTo()
	const
{
	if (!itsToAdjusted)
		{
		GARMBoxHeader* th	= const_cast<GARMBoxHeader*>(this);
		th->DecodeMIMEHeader(&(th->itsTo));
		th->itsToAdjusted	= kJTrue;
		}
	return itsTo;
}

/******************************************************************************
 GetAddress

 *****************************************************************************/

const JString&
GARMBoxHeader::GetAddress()
	const
{
	return itsAddress;
}


/******************************************************************************
 GetSubject

 *****************************************************************************/

const JString&
GARMBoxHeader::GetSubject()
	const
{
	if (!itsSubjectAdjusted)
		{
		GARMBoxHeader* th	= const_cast<GARMBoxHeader*>(this);
		th->DecodeMIMEHeader(&(th->itsSubject));
		th->itsSubjectAdjusted	= kJTrue;
		}
	return itsSubject;
}

/******************************************************************************
 GetDate

 *****************************************************************************/

const JString&
GARMBoxHeader::GetDate()
	const
{
	return itsDate;
}

/******************************************************************************
 GetShortDate

 *****************************************************************************/

const JString&
GARMBoxHeader::GetShortDate()
	const
{
	return itsShortDate;
}

/******************************************************************************
 GetMonth

 *****************************************************************************/

const JString&
GARMBoxHeader::GetMonth()
	const
{
	return itsMonth;
}

/******************************************************************************
 GetYear

 *****************************************************************************/

JIndex
GARMBoxHeader::GetYear()
	const
{
	return itsYear;
}

/******************************************************************************
 GetDay

 *****************************************************************************/

JIndex
GARMBoxHeader::GetDay()
	const
{
	return itsDay;
}

/******************************************************************************
 GetTime

 *****************************************************************************/

const JString&
GARMBoxHeader::GetTime()
	const
{
	return itsTime;
}

/******************************************************************************
 GetReplyTo

 *****************************************************************************/

const JString&
GARMBoxHeader::GetReplyTo()
	const
{
	if (!itsReplyToAdjusted)
		{
		GARMBoxHeader* th	= const_cast<GARMBoxHeader*>(this);
		th->DecodeMIMEHeader(&(th->itsReplyTo));
		th->itsReplyToAdjusted	= kJTrue;
		}
	return itsReplyTo;
}

/******************************************************************************
 GetCC

 *****************************************************************************/

const JString&
GARMBoxHeader::GetCC()
	const
{
	if (!itsCCAdjusted)
		{
		GARMBoxHeader* th	= const_cast<GARMBoxHeader*>(this);
		th->DecodeMIMEHeader(&(th->itsCC));
		th->itsCCAdjusted	= kJTrue;
		}
	return itsCC;
}

/******************************************************************************
 GetHeaderStart

 *****************************************************************************/

JIndex
GARMBoxHeader::GetHeaderStart()
	const
{
	return itsHeaderStart;
}

/******************************************************************************
 GetHeaderEnd

 *****************************************************************************/

JIndex
GARMBoxHeader::GetHeaderEnd()
	const
{
	return itsHeaderEnd;
}

/******************************************************************************
 GetMessageEnd

 *****************************************************************************/

JIndex
GARMBoxHeader::GetMessageEnd()
	const
{
	return itsMessageEnd;
}



/******************************************************************************
 GetMessageStatus

 *****************************************************************************/

GARMBoxHeader::MessageStatus
GARMBoxHeader::GetMessageStatus()
	const
{
	return itsMessageStatus;
}

/******************************************************************************
 IsMIME (public)

 ******************************************************************************/

JBoolean
GARMBoxHeader::IsMIME()
	const
{
	return itsIsMIME;
}

/******************************************************************************
 HasAttachment (public)

 ******************************************************************************/

JBoolean
GARMBoxHeader::HasAttachment()
	const
{
	return itsHasAttachment;
}

/******************************************************************************
 GetReplyStatus (public)

 ******************************************************************************/

GARMBoxHeader::ReplyStatus
GARMBoxHeader::GetReplyStatus()
	const
{
	return itsReplyStatus;
}

/******************************************************************************
 GetMIMEParser (public)

 ******************************************************************************/

GMMIMEParser*
GARMBoxHeader::GetMIMEParser()
{
	return itsMIMEParser;
}

/******************************************************************************
 GetAccount

 *****************************************************************************/

const JString&
GARMBoxHeader::GetAccount()
	const
{
	return itsAccount;
}

/******************************************************************************
 DecodeMIMEHeader (private)

 ******************************************************************************/

static const JRegex encodedMIMEQRegex("=\\?(ISO|iso)-8859-[1-9]\\?[Q|q]\\?[^?]+\\?=");
static const JRegex encodedMIMEBRegex("=\\?(ISO|iso)-8859-[1-9]\\?[B|b]\\?[^?]+\\?=");

void
GARMBoxHeader::DecodeMIMEHeader
	(
	JString* header
	)
{
	JArray<JIndexRange> subList;
	JSize count	= encodedMIMEQRegex.MatchAll(*header, &subList);
	JBoolean qType	= kJTrue;
	if (count == 0)
		{
		count	= encodedMIMEBRegex.MatchAll(*header, &subList);
		qType	= kJFalse;
		}
	if (count > 0)
		{
		JString temp;
		JIndex mIndex		= 1;
		const JSize count	= subList.GetElementCount();
		for (JIndex i = 1; i <= count; i++)
			{
			JIndexRange range	= subList.GetElement(i);
//			if ((range.first != mIndex) &&
//				RangeContainsNWS(*header, mIndex, range.first))
//				{
//				temp += header->GetSubstring(mIndex, range.first - 1);
//				}
			if (range.first != mIndex)
				{
				JString trimmed = header->GetSubstring(mIndex, range.first - 1);
				trimmed.TrimWhitespace();
				if (!trimmed.IsEmpty())
					{
					temp += header->GetSubstring(mIndex, range.first - 1);
					}
				}
			temp   += DecodeMIMEWord(qType, header, range);
			mIndex	= range.last + 1;
			}
		if (mIndex < header->GetLength())
			{
			temp   += header->GetSubstring(mIndex, header->GetLength());
			}
		*header = temp;
		}
}

/******************************************************************************
 RangeContainsNWS (private)

 ******************************************************************************/

JBoolean
GARMBoxHeader::RangeContainsNWS
	(
	const JString&	text,
	const JIndex	index1,
	const JIndex	index2
	)
{
	for (JIndex i = index1; i <= index2; i++)
		{
		JCharacter c	= text.GetCharacter(i);
		if (!isspace(c) && (c != '\n'))
			{
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 DecodeMIMEWord (private)

  format:		=?iso-8859-1?q?encoded_text?=
  reference:	RFC 2047

 ******************************************************************************/

JString
GARMBoxHeader::DecodeMIMEWord
	(
	const JBoolean		qType,
	JString*			header,
	const JIndexRange	range
	)
{
	JString temp;
	JIndex findex	= range.first;
	// first ? '=?'
	JBoolean ok		= header->LocateNextSubstring("?", &findex);
	if (!ok)
		{
		return temp;
		}
	findex++;
	// second ? '?Q'
	ok		= header->LocateNextSubstring("?", &findex);
	if (!ok)
		{
		return temp;
		}
	// third ? 'Q?'
	findex++;
	ok		= header->LocateNextSubstring("?", &findex);
	if (!ok || (findex > range.last))
		{
		return temp;
		}
	JIndex endIndex	= findex + 1;
	// final ? '?='
	ok		= header->LocateNextSubstring("?", &endIndex);
	if (!ok || (endIndex > range.last))
		{
		return temp;
		}
	// so the encoded text is between findex and endIndex.
	if (qType)
		{
		JIndex dIndex	= findex + 1;
		while (dIndex < endIndex)
			{
			JCharacter c	= header->GetCharacter(dIndex);
			if (c == '=')
				{
				JString hex	= header->GetSubstring(dIndex + 1, dIndex + 2);
				hex.Prepend("0x");
				JInteger hexVal;
				if (hex.ConvertToInteger(&hexVal))
					{
					c	= (JCharacter)hexVal;
					temp.AppendCharacter(c);
					dIndex += 3;
					}
				}
			else if (c == '_')
				{
				temp.AppendCharacter(' ');
				dIndex++;
				}
			else
				{
				temp.AppendCharacter(c);
				dIndex++;
				}
			}
		}
	else
		{
		if (findex + 1 > header->GetLength())
			{
			return temp;
			}
		if (endIndex - 1 < findex + 1)
			{
			return temp;
			}
		temp = header->GetSubstring(findex + 1, endIndex - 1);
		const std::string s(temp.GetCString(), temp.GetLength());
		std::istringstream is(s);
		std::ostringstream os;
		JDecodeBase64(is, os);
		temp = os.str();
		}
	return temp;
}

/******************************************************************************
 AdjustPosition

 *****************************************************************************/

void
GARMBoxHeader::AdjustPosition
	(
	const JIndex newposition
	)
{
	int delta = itsHeaderStart - newposition;
	itsHeaderStart = newposition;
	itsHeaderEnd -= delta;
	itsMessageEnd -= delta;
}

/******************************************************************************
 Stream operators

	The string data is delimited by double quotes:  "this is a string".

	To include double quotes in a string, use \"
	To include a backslash in a string, use \\

	An exception is made if the streams are cin or cout.
	For input, characters are simply read until 'return' is pressed.
	For output, Print() is used.

 ******************************************************************************/

istream&
operator>>
	(
	istream&	input,
	GARMBoxHeader&	header
	)
{
	JString temp;
	input >> temp;
	header.SetHeader(temp);
	input >> temp;
	header.SetFrom(temp);
	input >> temp;
	header.SetTo(temp);
	input >> temp;
	header.SetAddress(temp);
	input >> temp;
	header.SetSubject(temp);
	input >> temp;
	header.SetDate(temp);
	int tempi;
	input >> tempi;
	header.SetHeaderStart(tempi);
	input >> tempi;
	header.SetHeaderEnd(tempi);
	input >> tempi;
	header.SetMessageEnd(tempi);
	input >> tempi;
	header.SetMessageStatus((GARMBoxHeader::MessageStatus)tempi);
	input >> tempi;
	header.SetReplyStatus((GARMBoxHeader::ReplyStatus)tempi);
	JBoolean tempb;
	input >> tempb;
	header.SetIsMIME(tempb);
	input >> tempb;
	header.SetHasAttachment(tempb);
	return input;
}

ostream&
operator<<
	(
	ostream&		output,
	const GARMBoxHeader&	header
	)
{
	output << header.GetHeader();
	output << header.GetFrom();
	output << header.GetTo();
	output << header.GetAddress();
	output << header.GetSubject();
	output << header.GetDate();
	output << ' ';
	output << header.GetHeaderStart();
	output << ' ';
	output << header.GetHeaderEnd();
	output << ' ';
	output << header.GetMessageEnd();
	output << ' ';
	GARMBoxHeader::MessageStatus status = header.GetMessageStatus();
	if (status == GARMBoxHeader::kDelete)
		{
		status = GARMBoxHeader::kRead;
		}
	output << (int)status;
	output << ' ';
	GARMBoxHeader::ReplyStatus rStatus	= header.GetReplyStatus();
	output << (int)rStatus;
	output << ' ';
	output << header.IsMIME() << ' ';
	output << header.HasAttachment() << ' ';
	return output;
}
