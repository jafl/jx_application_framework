/******************************************************************************
 JNetworkProtocolBase.h

	Copyright (C) 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JNetworkProtocolBase
#define _H_JNetworkProtocolBase

#include <ace/Svc_Handler.h>
#include <ace/Synch_Traits.h>
#include <JString.h>

template <ACE_PEER_STREAM_1>
class JNetworkProtocolBase : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>
{
public:

	JNetworkProtocolBase(const JBoolean synch);

	virtual ~JNetworkProtocolBase();

	JBoolean	DataPending() const;
	void		Flush();

	// whether data is sent synch or asynch

	JBoolean	WillSendSynch() const;
	void		ShouldSendSynch(const JBoolean synch = kJTrue);

	// ACE_Svc_Handler functions

	virtual int	handle_output(ACE_HANDLE);

protected:

	void	Send(const JString& data);
	void	Send(const iovec data[], const JSize count);

private:

	JBoolean	itsSynchFlag;	// kJTrue => synch send
	JString		itsSendData;	// data waiting to be sent

private:

	// not allowed

	JNetworkProtocolBase(const JNetworkProtocolBase& source);
	JNetworkProtocolBase& operator=(const JNetworkProtocolBase& source);
};

#include <JNetworkProtocolBase.tmpl>

#endif
