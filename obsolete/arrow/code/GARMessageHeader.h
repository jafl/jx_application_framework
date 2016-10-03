/******************************************************************************
 GARMessageHeader.h

	Interface for the GARMessageHeader class.

	Copyright (C) 2001 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GARMessageHeader
#define _H_GARMessageHeader

#include <JString.h>

#include <jTypes.h>

class GARMessageHeader
{
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

	GARMessageHeader();
	virtual ~GARMessageHeader();

	void SetHeader(const JString& header);
	void SetFrom(const JString& from);
	void SetAddress(const JString& address);
	void SetSubject(const JString& subject);
	void SetDate(const JString& date);
	void SetReplyTo(const JString& replyto);
	void SetCC(const JString& cc);
	void SetTo(const JString& to);
	void SetMessageStatus(const MessageStatus status);
	void SetHasAttachment(const JBoolean attachment);
	void SetReplyStatus(const ReplyStatus status);
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
	JBoolean		HasAttachment() const;
	ReplyStatus		GetReplyStatus() const;
	const JString&	GetAccount() const;

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
	JIndex			itsYear;
	JString			itsMonth;
	JIndex			itsDay;
	JString			itsTime;
	MessageStatus	itsMessageStatus;
	JBoolean		itsHasAttachment;
	ReplyStatus		itsReplyStatus;
	JString			itsAccount;

private:

	// not allowed

	GARMessageHeader(const GARMessageHeader& source);
	const GARMessageHeader& operator=(const GARMessageHeader& source);
};

#endif
