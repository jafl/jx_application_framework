/******************************************************************************
 JNetworkProtocolBase.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JNetworkProtocolBase
#define _H_JNetworkProtocolBase

#include <ace/Svc_Handler.h>
#include <ace/Synch_Traits.h>
#include "jx-af/jcore/JLinkedList.h"

class JString;

template <ACE_PEER_STREAM_1>
class JNetworkProtocolBase : public ACE_Svc_Handler<ACE_PEER_STREAM_2, ACE_SYNCH>
{
public:

	JNetworkProtocolBase(const bool synch);

	virtual ~JNetworkProtocolBase();

	bool	DataPending() const;
	void		Flush();

	// whether data is sent synch or asynch

	bool	WillSendSynch() const;
	void		ShouldSendSynch(const bool synch = true);

	// ACE_Svc_Handler functions

	virtual int	handle_output(ACE_HANDLE) override;

protected:

	void	Send(const JString& data);
	void	Send(const JUtf8Byte* data, const JSize count);
	void	Send(const iovec data[], const JSize count);

public:

	struct Chunk
	{
		char* data;
		JSize count;
		JSize sendOffset;

		Chunk()
			:
			data(nullptr), count(0), sendOffset(0)
		{ };

		Chunk(const char* data, const JSize count);

		// accept default copy constructor

		void CleanOut()
		{
			jdelete [] data;
			data = nullptr;
		}
	};

private:

	bool			itsSynchFlag;	// true => synch send
	JLinkedList<Chunk>	itsSendData;	// data waiting to be sent

private:

	// not allowed

	JNetworkProtocolBase(const JNetworkProtocolBase& source);
	JNetworkProtocolBase& operator=(const JNetworkProtocolBase& source);
};

#include "JNetworkProtocolBase.tmpl"

#endif
