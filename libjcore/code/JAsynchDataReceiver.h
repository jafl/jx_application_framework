/******************************************************************************
 JAsynchDataReceiver.h

	Copyright (C) 1999-2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JAsynchDataReceiver
#define _H_JAsynchDataReceiver

#include "jx-af/jcore/JPtrArray-JString.h"
#include "jx-af/jcore/JUtf8ByteBuffer.h"
#include <ace/Svc_Handler.h>
#include <ace/Synch_Traits.h>

class JAsynchDataReceiverT
{
public:

	// Other constants

	enum
		{
		kDefaultBufferSize = 65536
		};

public:

	// Broadcaster messages

	static const JUtf8Byte* kDataReady;

	class DataReady : public JBroadcaster::Message
		{
		public:

			DataReady(const JString& data)
				:
				JBroadcaster::Message(kDataReady),
				itsData(data)
				{ };

			const JString&
			GetData() const
			{
				return itsData;
			};

		private:

			const JString& itsData;
		};
};

template <ACE_PEER_STREAM_1>
class JAsynchDataReceiver : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>,
							virtual public JBroadcaster
{
public:

	JAsynchDataReceiver();
	JAsynchDataReceiver(const ACE_HANDLE fd);

	virtual ~JAsynchDataReceiver();

	// how much is read from the connection at one time

	JSize	GetBufferSize() const;
	void	SetBufferSize(const JSize bufferSize);

	// ACE_Svc_Handler functions

	virtual int	handle_input(ACE_HANDLE) override;

private:

	JUtf8Byte*		itsRecvBuffer;					// buffer to receive raw bytes
	JSize			itsRecvBufferSize;
	JUtf8ByteBuffer	itsByteBuffer;

	bool	itsInHandleInputFlag;		// true => stack passes through handle_input()

private:

	void	JAsynchDataReceiverX();

	// not allowed

	JAsynchDataReceiver(const JAsynchDataReceiver&) = delete;
	JAsynchDataReceiver& operator=(const JAsynchDataReceiver&) = delete;
};

#include "JAsynchDataReceiver.tmpl"

#endif
