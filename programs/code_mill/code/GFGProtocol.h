/******************************************************************************
 GFGProtocol.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_GFGProtocol
#define _H_GFGProtocol

#include <JBroadcaster.h>
#include <JString.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch_Traits.h>

class GFGProtocolT
{
public:

	enum
		{
		kDefaultBufferSize = 10000
		};

public:

	// Broadcaster messages

	static const JCharacter* kReadReady;

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
class GFGProtocol : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>,
				   virtual public JBroadcaster
{
public:

	GFGProtocol();
	GFGProtocol(const ACE_HANDLE fd);

	virtual ~GFGProtocol();

	void	Read(JString* buffer);
	void	Write(const JCharacter* buffer);

	// ACE_Svc_Handler functions

	virtual int	handle_input(ACE_HANDLE) override;

private:

	JString		itsData;

	JCharacter*	itsBuffer;				// buffer to receive raw bytes
	JSize		itsBufferSize;

	bool	itsInHandleInputFlag;	// true => stack passes through handle_input()

private:

	void	GFGProtocolX();

	// not allowed

	GFGProtocol(const GFGProtocol& source);
	GFGProtocol& operator=(const GFGProtocol& source);
};

#include <GFGProtocol.tmpl>

#endif
