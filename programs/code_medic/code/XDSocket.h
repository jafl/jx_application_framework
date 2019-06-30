/******************************************************************************
 XDSocket.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDSocket
#define _H_XDSocket

#include <JMessageProtocol.h>
#include <ace/SOCK_Stream.h>

class XDSocket : public JMessageProtocol<ACE_SOCK_STREAM>
{
public:

	XDSocket();

	virtual	~XDSocket();

	void	StartTimer();
	void	StopTimer();

	virtual int	open(void*) override;
	virtual int	handle_timeout(const ACE_Time_Value& time, const void*) override;
	virtual int handle_close(ACE_HANDLE h, ACE_Reactor_Mask m) override;

private:

	long	itsTimerID;
};

#endif
