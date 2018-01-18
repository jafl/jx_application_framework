/******************************************************************************
 evt_handlers.h

	Interface for event handler classes.

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_evt_handlers
#define _H_evt_handlers

#include <jTypes.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch_Traits.h>

template <ACE_PEER_STREAM_1>
class ClientHandler : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>
{
public:

	virtual int	open(void*) override;
};

template <ACE_PEER_STREAM_1>
class ServerHandler : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>
{
public:

	virtual int	open(void*) override;
	virtual int	handle_input(ACE_HANDLE) override;
	virtual int	handle_output(ACE_HANDLE) override;
};

#include "evt_handlers.tmpl"

#endif
