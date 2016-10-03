/******************************************************************************
 GARMBoxHeader.h

	Interface for the GARMBoxHeader class.

	Copyright (C) 2001 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GARMBoxHeader
#define _H_GARMBoxHeader

#include <JString.h>

class GMMIMEParser;

class GARMBoxHeader
{
public:

	friend istream& operator>>(istream&, GARMBoxHeader&);
	friend ostream& operator<<(ostream&, const GARMBoxHeader&);

public:

	enum MessageStatus
		{
		kNew = 1,
		kRead,
		kUrgent,
		kImportant,
		kDelete
		};

	enum ReplyStatus
		{
		kNoReply = 1,
		kReplied,
		kRepliedSender,
		kRepliedAll,
		kForwarded,
		kRedirected,
		};

public:

	GARMBoxHeader();
	virtual ~GARMBoxHeader();

	void SetHeader(const JString& header);
	void SetFrom(const JString& from);
	void SetAddress(const JString& address);
	void SetSubject(const JString& subject);
	void SetDate(const JString& date);
	void SetReplyTo(const JString& replyto);
	void SetCC(const JString& cc);
	void SetTo(const JString& to);
	void SetHeaderStart(const JIndex start);
	void SetHeaderEnd(const JIndex end);
	void SetMessageEnd(const JIndex end);
	void SetMessageStatus(const MessageStatus status);
	void SetIsMIME(const JBoolean mime);
	void SetHasAttachment(const JBoolean attachment);
	void SetReplyStatus(const ReplyStatus status);
	void SetMIMEParser(GMMIMEParser* parser);
	void SetAccount(const JString& account);

	const JString&	GetHeader() const;
	const JString&	GetFrom() const;
	const JString&	GetFromNames() const;
	const JString&	GetAddress() const;
	const JString&	GetSubject() const;
	const JString&	GetDate() const;
	const JString&	GetShortDate() const;
	const JString&	GetMonth() const;
	JIndex			GetYear() const;
	JIndex			GetDay() const;
	const JString&	GetTime() const;
	const JString&	GetReplyTo() const;
	const JString&	GetCC() const;
	const JString&	GetTo() const;
	JIndex			GetHeaderStart() const;
	JIndex			GetHeaderEnd() const;
	JIndex			GetMessageEnd() const;
	MessageStatus	GetMessageStatus() const;
	JBoolean		IsMIME() const;
	JBoolean		HasAttachment() const;
	ReplyStatus		GetReplyStatus() const;
	GMMIMEParser*	GetMIMEParser();
	const JString&	GetAccount() const;

	void			AdjustPosition(const JIndex newposition);

private:

	JString		itsHeader;
	JString		itsFrom;
	JString		itsFromNames;
	JString		itsAddress;
	JString		itsSubject;
	JString		itsDate;
	JString		itsShortDate;
	JString		itsReplyTo;
	JString		itsCC;
	JString		itsTo;
	JIndex			itsHeaderStart;
	JIndex			itsHeaderEnd;
	JIndex			itsMessageEnd;
	JIndex			itsYear;
	JString			itsMonth;
	JIndex			itsDay;
	JString			itsTime;
	MessageStatus	itsMessageStatus;
	JBoolean		itsIsMIME;
	JBoolean		itsHasAttachment;
	ReplyStatus		itsReplyStatus;
	GMMIMEParser*	itsMIMEParser;
	JBoolean		itsSubjectAdjusted;
	JBoolean		itsReplyToAdjusted;
	JBoolean		itsCCAdjusted;
	JBoolean		itsToAdjusted;
	JString			itsAccount;

private:

	void		DecodeMIMEHeader(JString* header);
	JString		DecodeMIMEWord(const JBoolean qType, JString* header, const JIndexRange range);
	JBoolean	RangeContainsNWS(const JString& text, const JIndex index1, const JIndex index2);

	// not allowed

	GARMBoxHeader(const GARMBoxHeader& source);
	const GARMBoxHeader& operator=(const GARMBoxHeader& source);
};

#endif
