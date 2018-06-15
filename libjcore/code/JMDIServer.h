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

	static JBoolean	WillBeMDIServer(const JUtf8Byte* signature,
									const int argc, char* argv[]);
	void			HandleCmdLineOptions(const int argc, char* argv[]);

	// called by event loop

	void	CheckForConnections();

protected:

	JBoolean	IsFirstTime() const;

	virtual JBoolean	CanAcceptMDIRequest() = 0;
	virtual void		PreprocessArgList(JPtrArray<JString>* argList);
	virtual void		HandleMDIRequest(const JString& dir,
										 const JPtrArray<JString>& argList) = 0;

private:

	JBoolean			itsFirstTimeFlag;
	ACE_LSOCK_Acceptor*	itsAcceptor;
	ACE_LSOCK_Stream*	itsSocket;		// always closed before returning

private:

	void			ProcessMDIMessage();
	static JString	GetMDISocketName(const JUtf8Byte* signature);

	static void		SendLine(ACE_LSOCK_Stream& socket, const JString& line);
	static JBoolean	ReceiveLine(ACE_LSOCK_Stream& socket, const JBoolean block,
								JString* line, JBoolean* receivedFinishedFlag);
	static void		WaitForFinished(ACE_LSOCK_Stream& socket,
									const JBoolean receivedFinishedFlag);

	// not allowed

	JMDIServer(const JMDIServer& source);
	const JMDIServer& operator=(const JMDIServer& source);
};


/******************************************************************************
 IsFirstTime (protected)

 ******************************************************************************/

inline JBoolean
JMDIServer::IsFirstTime()
	const
{
	return itsFirstTimeFlag;
}

#endif
