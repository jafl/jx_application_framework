/******************************************************************************
 JMDIServer.h

	Interface for the JMDIServer class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JMDIServer
#define _H_JMDIServer

#include "JPtrArray.h"

class JString;
class ACE_LSOCK_Acceptor;
class ACE_LSOCK_Stream;

class JMDIServer
{
public:

	static const JUtf8Byte* kQuitOptionName;

public:

	JMDIServer(const JUtf8Byte* signature);

	virtual ~JMDIServer();

	// for use by main()

	static bool	WillBeMDIServer(const JUtf8Byte* signature,
								const int argc, char* argv[]);
	void		HandleCmdLineOptions(const int argc, char* argv[]);

	// called by event loop

	void	CheckForConnections();

protected:

	bool	IsFirstTime() const;

	virtual bool	CanAcceptMDIRequest() = 0;
	virtual void	PreprocessArgList(JPtrArray<JString>* argList);
	virtual void	HandleMDIRequest(const JString& dir,
									 const JPtrArray<JString>& argList) = 0;

private:

	bool				itsFirstTimeFlag;
	ACE_LSOCK_Acceptor*	itsAcceptor;
	ACE_LSOCK_Stream*	itsSocket;		// always closed before returning

private:

	void			ProcessMDIMessage();
	static JString	GetMDISocketName(const JUtf8Byte* signature);

	static void	SendLine(ACE_LSOCK_Stream& socket, const JString& line);
	static bool	ReceiveLine(ACE_LSOCK_Stream& socket, const bool block,
							JString* line, bool* receivedFinishedFlag);
	static void	WaitForFinished(ACE_LSOCK_Stream& socket,
								const bool receivedFinishedFlag);

	// not allowed

	JMDIServer(const JMDIServer&) = delete;
	JMDIServer& operator=(const JMDIServer&) = delete;
};


/******************************************************************************
 IsFirstTime (protected)

 ******************************************************************************/

inline bool
JMDIServer::IsFirstTime()
	const
{
	return itsFirstTimeFlag;
}

#endif
