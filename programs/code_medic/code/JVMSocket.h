/******************************************************************************
 JVMSocket.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMSocket
#define _H_JVMSocket

#include <JNetworkProtocolBase.h>
#include <ace/SOCK_Stream.h>

class JVMSocket : public JNetworkProtocolBase<ACE_SOCK_STREAM>,
				  virtual public JBroadcaster
{
public:

	JVMSocket();

	virtual	~JVMSocket();

	void	Send(const JIndex id, const JIndex cmdSet, const JIndex cmd,
				 const unsigned char* data, const JSize count);

	static void		Pack(const JSize size, const JUInt64 value, unsigned char* data);
	static JUInt64	Unpack(const JSize size, const unsigned char* data);

	static void		Pack2(const JUInt32 value, unsigned char* data);
	static JUInt32	Unpack2(const unsigned char* data);

	static void		Pack4(const JUInt32 value, unsigned char* data);
	static JUInt32	Unpack4(const unsigned char* data);

	static void		Pack8(const JUInt64 value, unsigned char* data);
	static JUInt64	Unpack8(const unsigned char* data);

	static void		PackString(const JString& s, unsigned char* data);
	static JString	UnpackString(const unsigned char* data, JSize* dataLength);

	// ACE_Svc_Handler functions

	virtual int	open(void*) override;
	virtual int	handle_timeout(const ACE_Time_Value& time, const void*) override;
	virtual int handle_close(ACE_HANDLE h, ACE_Reactor_Mask m) override;
	virtual int	handle_input(ACE_HANDLE) override;

private:

	bool				itsHandshakeFinishedFlag;	// true => handshake is complete
	unsigned char*			itsBuffer;					// buffer to receive raw bytes
	JArray<unsigned char>*	itsRecvData;				// buffer containing unprocessed bytes

	long		itsTimerID;
	bool	itsInHandleInputFlag;		// true => stack passes through handle_input()

private:

	void	StartTimer();
	void	StopTimer();

public:

	// Broadcaster messages

	static const JUtf8Byte* kMessageReady;

	class MessageReady : public JBroadcaster::Message
		{
		public:

			MessageReady(const JIndex id, const bool isReply,
						 const JIndex cmdSet, const JIndex cmd, const JIndex errorCode,
						 const unsigned char* data, const JSize length)
				:
				JBroadcaster::Message(kMessageReady),
				itsID(id), itsIsReplyFlag(isReply),
				itsCmdSet(cmdSet), itsCmd(cmd), itsErrorCode(errorCode),
				itsData(data), itsDataLength(length)
				{ };

			JIndex
			GetID() const
			{
				return itsID;
			};

			bool
			IsReply() const
			{
				return itsIsReplyFlag;
			};

			JIndex
			GetErrorCode() const		// for replies
			{
				return itsErrorCode;
			};

			JIndex
			GetCommandSet() const		// for non-replies
			{
				return itsCmdSet;
			};

			JIndex
			GetCommand() const			// for non-replies
			{
				return itsCmd;
			};

			const unsigned char*
			GetData() const
			{
				return itsData;
			};

			JSize
			GetDataLength() const
			{
				return itsDataLength;
			};

		private:

			const JIndex			itsID;
			const bool			itsIsReplyFlag;
			const JIndex			itsCmdSet;
			const JIndex			itsCmd;
			const JIndex			itsErrorCode;
			const unsigned char*	itsData;
			const JSize				itsDataLength;
		};
};

#endif
