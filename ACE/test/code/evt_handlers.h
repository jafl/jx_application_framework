/******************************************************************************
 evt_handlers.h

	Interface for event handler classes.

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_evt_handlers
#define _H_evt_handlers

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <ace/Svc_Handler.h>
#include <ace/Synch_T.h>
#include <jTypes.h>

template <ACE_PEER_STREAM_1>
class ClientHandler : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>
{
public:

	virtual int	open(void*);
};

template <ACE_PEER_STREAM_1>
class ServerHandler : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>
{
public:

	virtual int	open(void*);
	virtual int	handle_input(ACE_HANDLE);
	virtual int	handle_output(ACE_HANDLE);
};

#endif
