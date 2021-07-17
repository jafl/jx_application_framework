/******************************************************************************
 CMPipe.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMPipe
#define _H_CMPipe

#include <JBroadcaster.h>
#include <JString.h>
#include <JUtf8ByteBuffer.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch_Traits.h>

class CMPipeT
{
public:

	enum
		{
		kDefaultBufferSize = 524288		// 512KB
		};

public:

	// Broadcaster messages

	static const JUtf8Byte* kReadReady;

	// Broadcaster message classes

	class ReadReady : public JBroadcaster::Message
		{
		public:

			ReadReady()
				:
				JBroadcaster::Message(kReadReady)
			{ };
		};
};

template <ACE_PEER_STREAM_1>
class CMPipe : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>,
			   virtual public JBroadcaster
{
public:

	CMPipe();
	CMPipe(const ACE_HANDLE fd);

	virtual ~CMPipe();

	void	Read(JString* buffer);
	void	Write(const JString& buffer);

	// ACE_Svc_Handler functions

	virtual int	handle_input(ACE_HANDLE) override;

private:

	JUtf8Byte*		itsRecvBuffer;		// buffer to receive raw bytes
	JSize			itsRecvBufferSize;
	JUtf8ByteBuffer	itsByteBuffer;		// buffer containing unprocessed bytes
	JString			itsRecvData;		// buffer containing unprocessed characters

	bool	itsInHandleInputFlag;	// true => stack passes through handle_input()

private:

	void	CMPipeX();

	// not allowed

	CMPipe(const CMPipe& source);
	CMPipe& operator=(const CMPipe& source);
};

#include "CMPipe.tmpl"

#endif
