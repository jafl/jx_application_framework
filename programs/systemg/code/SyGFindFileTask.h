/******************************************************************************
 SyGFindFileTask.h

	Copyright @ 2008-09 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGFindFileTask
#define _H_SyGFindFileTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JMessageProtocol.h>	// template; requires ace includes

class JProcess;
class SyGTreeDir;

class SyGFindFileTask : public JBroadcaster
{
public:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>	RecordLink;

public:

	static JBoolean	Create(SyGTreeDir* dir, const JCharacter* path,
						   const JCharacter* expr, SyGFindFileTask** task);

	virtual ~SyGFindFileTask();

protected:

	SyGFindFileTask(SyGTreeDir* dir, JProcess* p, int outFD, int errFD);

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	SyGTreeDir*			itsDirector;		// not owned
	JProcess*			itsProcess;			// can be NULL
	RecordLink*			itsMessageLink;		// can be NULL
	RecordLink*			itsErrorLink;		// can be NULL
	JString				itsErrors;			// cache while process is running

private:

	void	SetConnection(const int outFD, const int errFD);
	void	DeleteLinks();
	void	ReceiveMessageLine();
	void	ReceiveErrorLine();
	void	DisplayErrors();

	// not allowed

	SyGFindFileTask(const SyGFindFileTask& source);
	const SyGFindFileTask& operator=(const SyGFindFileTask& source);
};

#endif
