/******************************************************************************
 JMessageProtocol.h

	Copyright © 1998-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JMessageProtocol
#define _H_JMessageProtocol

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JNetworkProtocolBase.h>
#include <JPtrArray-JString.h>

class JMessageProtocolT
{
public:

	// Protocol definitions

	static const JCharacter* kUNIXSeparatorStr;
	static const JSize kUNIXSeparatorLength;

	static const JCharacter* kMacintoshSeparatorStr;
	static const JSize kMacintoshSeparatorLength;

	static const JCharacter* kDOSSeparatorStr;
	static const JSize kDOSSeparatorLength;

	static const JCharacter* kStdDisconnectStr;
	static const JSize kStdDisconnectLength;

	// Other constants

	enum
		{
		kDefaultBufferSize = 65536
		};

public:

	// Broadcaster messages

	static const JCharacter* kMessageReady;
	static const JCharacter* kReceivedDisconnect;	// other end sent disconnect

	class MessageReady : public JBroadcaster::Message
		{
		public:

			MessageReady(const JString& msg)
				:
				JBroadcaster::Message(kMessageReady),
				itsMessage(msg)
				{ };

			// This does not remove the message from the input queue.

			const JString&
			PeekMessage() const
			{
				return itsMessage;
			};

		private:

			const JString& itsMessage;
		};

	class ReceivedDisconnect : public JBroadcaster::Message
		{
		public:

			ReceivedDisconnect()
				:
				JBroadcaster::Message(kReceivedDisconnect)
				{ };
		};
};

template <ACE_PEER_STREAM_1>
class JMessageProtocol : public JNetworkProtocolBase<ACE_PEER_STREAM_2>,
						 virtual public JBroadcaster
{
public:

	JMessageProtocol(const JBoolean synchSend = kJFalse);
	JMessageProtocol(const ACE_HANDLE fd, const JBoolean synchSend = kJFalse);

	virtual ~JMessageProtocol();

	void	GetProtocol(JString* separator, JString* disconnect) const;
	void	SetProtocol(const JCharacter* separatorStr, const JSize separatorLength,
						const JCharacter* disconnectStr, const JSize disconnectLength);
	void	UseUNIXProtocol();
	void	UseMacintoshProtocol();
	void	UseDOSProtocol();

	JBoolean	ReceivedDisconnect() const;
	void		SendDisconnect();

	JBoolean	HasMessages() const;
	JSize		GetMessageCount() const;
	JBoolean	GetNextMessage(JString* message);
	JBoolean	PeekNextMessage(JString* message);
	JBoolean	PeekPartialMessage(JString* message);

	void	SendMessage(const JString& message);
	void	SendMessage(const JCharacter* message);
	void	SendMessage(const JCharacter* message, const JSize length);

	void	SendData(const JString& data);
	void	SendData(const JCharacter* data);
	void	SendData(const JCharacter* data, const JSize length);

	void	TranslateFromUNIXAndSend(const JString& data);
	void	TranslateFromUNIXAndSend(const JCharacter* data);
	void	TranslateFromUNIXAndSend(const JCharacter* data, const JSize length);

	void	TranslateFromMacintoshAndSend(const JString& data);
	void	TranslateFromMacintoshAndSend(const JCharacter* data);
	void	TranslateFromMacintoshAndSend(const JCharacter* data, const JSize length);

	void	TranslateFromDOSAndSend(const JString& data);
	void	TranslateFromDOSAndSend(const JCharacter* data);
	void	TranslateFromDOSAndSend(const JCharacter* data, const JSize length);

	void	TranslateAndSend(const JString& data,
							 const JCharacter* separatorStr, const JSize separatorLength);
	void	TranslateAndSend(const JCharacter* data,
							 const JCharacter* separatorStr, const JSize separatorLength);
	void	TranslateAndSend(const JCharacter* data, const JSize length,
							 const JCharacter* separatorStr, const JSize separatorLength);

	// how much is read from the connection at one time

	JSize	GetBufferSize() const;
	void	SetBufferSize(const JSize bufferSize);

	// ACE_Svc_Handler functions

	virtual int	handle_input(ACE_HANDLE);

private:

	JPtrArray<JString>*	itsMessageList;		// parsed messages, last one is incomplete

	JString		itsRecvData;				// buffer containing unprocessed bytes
	JCharacter*	itsBuffer;					// buffer to receive raw bytes
	JSize		itsBufferSize;

	JString		itsSeparatorStr;			// sent between messages
	JString		itsDisconnectStr;			// can be empty; sent to terminate connection

	JBoolean	itsSentDisconnectFlag;		// kJTrue => we are finished
	JBoolean	itsReceivedDisconnectFlag;	// kJTrue => other end is finished
	JBoolean	itsInHandleInputFlag;		// kJTrue => stack passes through handle_input()

private:

	void		JMessageProtocolX();
	JString*	NewMessage();

	JBoolean	LocateSequence(const JString& data, const JString& sequence,
							   JIndex* index, JBoolean* endsWithPartial);
	JBoolean	LocateNextSequence(const JCharacter* d, const JSize dLength,
								   const JString& sequence, JIndex* index,
								   JBoolean* endsWithPartial);

	// not allowed

	JMessageProtocol(const JMessageProtocol& source);
	JMessageProtocol& operator=(const JMessageProtocol& source);
};

#endif
