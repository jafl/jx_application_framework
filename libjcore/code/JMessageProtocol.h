/******************************************************************************
 JMessageProtocol.h

	Copyright (C) 1998-2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JMessageProtocol
#define _H_JMessageProtocol

#include "JNetworkProtocolBase.h"
#include "JPtrArray-JString.h"
#include "JRegex.h"

class JMessageProtocolT
{
public:

	// Protocol definitions

	static const JUtf8Byte* kUNIXSeparatorStr;
	static const JSize kUNIXSeparatorByteCount;

	static const JUtf8Byte* kMacintoshSeparatorStr;
	static const JSize kMacintoshSeparatorByteCount;

	static const JUtf8Byte* kDOSSeparatorStr;
	static const JSize kDOSSeparatorByteCount;

	static const JUtf8Byte* kStdDisconnectStr;
	static const JSize kStdDisconnectByteCount;

	// Other constants

	enum
		{
		kDefaultBufferSize = 65536
		};

public:

	// Broadcaster messages

	static const JUtf8Byte* kMessageReady;
	static const JUtf8Byte* kReceivedDisconnect;	// other end sent disconnect

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
	void	SetProtocol(const JUtf8Byte* separatorStr, const JSize separatorByteCount,
						const JUtf8Byte* disconnectStr, const JSize disconnectByteCount);
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
	void	SendData(const JString& data);

	// how much is read from the connection at one time

	JSize	GetBufferSize() const;
	void	SetBufferSize(const JSize bufferSize);

	// ACE_Svc_Handler functions

	virtual int	handle_input(ACE_HANDLE) override;

private:

	JPtrArray<JString>	itsMessageList;		// complete messages + empty one at the end

	JString		itsRecvData;				// buffer containing unprocessed characters
	JUtf8Byte*	itsBuffer;					// buffer to receive raw bytes
	JSize		itsBufferSize;
	JSize		itsBufferOffset;			// partial character from last recv

	JString		itsSeparatorStr;			// sent between messages
	JString		itsDisconnectStr;			// can be empty; sent to terminate connection
	JRegex		itsTokenPattern;			// separator or disconnect

	JBoolean	itsSentDisconnectFlag;		// kJTrue => we are finished
	JBoolean	itsReceivedDisconnectFlag;	// kJTrue => other end is finished
	JBoolean	itsInHandleInputFlag;		// kJTrue => stack passes through handle_input()

private:

	void		JMessageProtocolX();
	JString*	NewMessage();
	void		UpdateTokenPattern();

	// not allowed

	JMessageProtocol(const JMessageProtocol& source);
	JMessageProtocol& operator=(const JMessageProtocol& source);
};

#include "JMessageProtocol.tmpl"

#endif
