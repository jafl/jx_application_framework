/******************************************************************************
 JAsynchDataReceiver.h

	Copyright © 1999-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JAsynchDataReceiver
#define _H_JAsynchDataReceiver

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray-JString.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch_T.h>

class JAsynchDataReceiverT
{
public:

	// Other constants

	enum
		{
		kDefaultBufferSize = 1000
		};

public:

	// Broadcaster messages

	static const JCharacter* kDataReady;

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

	virtual int	handle_input(ACE_HANDLE);

private:

	JCharacter*	itsBuffer;					// buffer to receive raw bytes
	JSize		itsBufferSize;

	JBoolean	itsInHandleInputFlag;		// kJTrue => stack passes through handle_input()

private:

	void	JAsynchDataReceiverX();

	// not allowed

	JAsynchDataReceiver(const JAsynchDataReceiver& source);
	JAsynchDataReceiver& operator=(const JAsynchDataReceiver& source);
};

#endif
