/******************************************************************************
 JMessageProtocol.h

	Copyright (C) 1998-2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JMessageProtocol
#define _H_JMessageProtocol

#include "JNetworkProtocolBase.h"
#include "JPtrArray-JString.h"
#include "JUtf8ByteBuffer.h"
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

	JMessageProtocol(const bool synchSend = false);
	JMessageProtocol(const ACE_HANDLE fd, const bool synchSend = false);

	~JMessageProtocol() override;

	void	GetProtocol(JString* separator, JString* disconnect) const;
	void	SetProtocol(const JUtf8Byte* separatorStr, const JSize separatorByteCount,
						const JUtf8Byte* disconnectStr, const JSize disconnectByteCount);
	void	UseUNIXProtocol();
	void	UseMacintoshProtocol();
	void	UseDOSProtocol();

	bool	ReceivedDisconnect() const;
	void	SendDisconnect();

	bool	HasMessages() const;
	JSize	GetMessageCount() const;
	bool	GetNextMessage(JString* message);
	bool	PeekNextMessage(JString* message);
	bool	PeekPartialMessage(JString* message);

	void	SendMessage(const JString& message);
	void	SendData(const JString& data);

	bool	WantsBlankMessages() const;
	void	SetWantsBlankMessages(const bool wantsBlanks = true);

	// how much is read from the connection at one time

	JSize	GetBufferSize() const;
	void	SetBufferSize(const JSize bufferSize);

	// ACE_Svc_Handler functions

	int	handle_input(ACE_HANDLE) override;

protected:

	bool	BytesEqual(const JString& s1, const JString& s2) const;

private:

	JUtf8Byte*			itsRecvBuffer;		// buffer to receive raw bytes
	JSize				itsRecvBufferSize;
	JUtf8ByteBuffer		itsByteBuffer;		// buffer containing unprocessed bytes
	JString				itsRecvData;		// buffer containing unprocessed characters
	JPtrArray<JString>	itsMessageList;		// complete messages

	JString	itsSeparatorStr;				// sent between messages
	JString	itsDisconnectStr;				// can be empty; sent to terminate connection
	JRegex	itsTokenPattern;				// separator or disconnect

	bool	itsWantsBlankMessagesFlag;
	bool	itsSentDisconnectFlag;			// true => we are finished
	bool	itsReceivedDisconnectFlag;		// true => other end is finished
	bool*	itsCancelTaskFlag;				// non-null => handle_input() is in progress

private:

	void	JMessageProtocolX();
	void	UpdateTokenPattern();

	// not allowed

	JMessageProtocol(const JMessageProtocol& source);
	JMessageProtocol& operator=(const JMessageProtocol& source);
};

#include "JMessageProtocol.tmpl"

#endif
