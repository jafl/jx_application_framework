/******************************************************************************
 SMTPMessage.h

	Interface for the SMTPMessage class

	Copyright (C) 1997 by Glenn W. Bach.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_SMTPMessage
#define _H_SMTPMessage

#include <JMessageProtocol.h>
#include <JPtrArray.h>

#include <j_prep_ace.h>
#include <ace/Connector.h>
#include <ace/INET_Addr.h>
#include <ace/UNIX_Addr.h>

#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>

class GMAttachmentTable;
class JTextEditor;
class GMAccount;
class JXTimerTask;

class SMTPMessage : virtual public JBroadcaster
{
public:

	SMTPMessage();
	virtual ~SMTPMessage();

	void	SetTo(const JString& to);
	void	SetFrom(const JString& from);
	void	SetSubject(const JString& subject);
	void	SetReplyTo(const JString& replyto);
	void	SetCC(const JString& cc);
	void	SetBCC(const JString& bcc);
	void	SetData(const JString& data);
	void	SetData(GMAttachmentTable* table, JTextEditor* editor);
	void	AddHeaders(const JString& headers);
	void	SetAccount(GMAccount* account);

	virtual void	Send();

	const JString&	GetTo() const;
	const JString&	GetFrom() const;
	const JString&	GetSubject() const;
	const JString&	GetReplyTo() const;
	const JString&	GetCC() const;
	const JString&	GetBCC() const;
	const JString&	GetData() const;
	const JString&	GetAddedHeaders() const;

	void	BlockUntilOkOrFail();

protected:

	virtual void	Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);

private:

	typedef JMessageProtocol<ACE_SOCK_STREAM>	InetLink;
	typedef ACE_Connector<InetLink, ACE_SOCK_CONNECTOR>	INETConnector;

	JXTimerTask*	itsDeleteTask;
	JXTimerTask*	itsTimeoutTask;

private:

	InetLink*		itsLink;
	INETConnector*	itsConnector;

	JString* itsTo;
	JString* itsFrom;
	JString* itsSubject;
	JString* itsReplyTo;
	JString* itsCC;
	JString* itsBCC;
	JString* itsData;
	JString* itsAddedHeaders;

	JIndex		itsCurrentIndex;
	JIndex		itsCurrentMode;

	JPtrArray<JString>*	itsToNames;
	JPtrArray<JString>*	itsCcNames;
	JPtrArray<JString>*	itsBccNames;

	JBoolean	itsSomethingRead;
	JBoolean	itsIsFinished;
	JBoolean	itsIsTryingToQuit;

	JString		itsSMTPServer;
	JString		itsFullName;
	JString		itsSMTPUser;

	JString		itsOutboxString;
	JString		itsOutbox;

private:

	void	SendLine(const JCharacter* line);
	void	SendLine(const JString& line);
	void	SendMultiLine(JString* data);

	void	SendData();

	void	HandleAddresses();
	void	ReadReturnValue();
	void	SendNextData();
	void	ReplaceAliases(JPtrArray<JString>& names);

	void	AppendToOutbox(const JCharacter* text);
	void	WriteToOutbox();

	// not allowed

	SMTPMessage(const SMTPMessage& source);
	const SMTPMessage& operator=(const SMTPMessage& source);

public:

// Broadcaster messages

	static const JCharacter* kMessageSent;
	static const JCharacter* kSendFailure;

	class MessageSent : public JBroadcaster::Message
		{
		public:

			MessageSent()
				:
				JBroadcaster::Message(kMessageSent)
				{ };
		};

	class SendFailure : public JBroadcaster::Message
		{
		public:

			SendFailure()
				:
				JBroadcaster::Message(kSendFailure)
				{ };
		};
};

/******************************************************************************
 GetTo

 *****************************************************************************/

inline const JString&
SMTPMessage::GetTo()
	const
{
	return *itsTo;
}

/******************************************************************************
 GetFrom

 *****************************************************************************/

inline const JString&
SMTPMessage::GetFrom()
	const
{
	return *itsFrom;
}

/******************************************************************************
 GetSubject

 *****************************************************************************/

inline const JString&
SMTPMessage::GetSubject()
	const
{
	return *itsSubject;
}

/******************************************************************************
 GetReplyTo

 *****************************************************************************/

inline const JString&
SMTPMessage::GetReplyTo()
	const
{
	return *itsReplyTo;
}

/******************************************************************************
 GetCC

 *****************************************************************************/

inline const JString&
SMTPMessage::GetCC()
	const
{
	return *itsCC;
}

/******************************************************************************
 GetBCC

 *****************************************************************************/

inline const JString&
SMTPMessage::GetBCC()
	const
{
	return *itsBCC;
}

/******************************************************************************
 GetData

 *****************************************************************************/

inline const JString&
SMTPMessage::GetData()
	const
{
	return *itsData;
}

/******************************************************************************
 GetAddedHeaders

 *****************************************************************************/

inline const JString&
SMTPMessage::GetAddedHeaders()
	const
{
	return *itsAddedHeaders;
}

#endif
