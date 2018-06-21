/******************************************************************************
 JMDIServer.cpp

	Base class for handling Multiple Document Interface (MDI) requests.
	It creates a UNIX domain socket so all jnew invocations of the application
	will be passed to us (via WillBeMDIServer()).

	If the application supports MDI, it must create a derived class and
	construct it with a unique application signature.

	The derived class must implement the following functions:

		CanAcceptMDIReqest
			Return kJFalse if the application is busy and cannot accept
			the request (e.g. due to re-entrancy problems).

		HandleMDIRequest
			The arguments to this function are the directory from which the
			MDI request was made and argv[].

	BASE CLASS = NONE

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "JMDIServer.h"
#include <ace/LSOCK_Acceptor.h>
#include <ace/LSOCK_Connector.h>
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include "JString.h"
#include "jSysUtil.h"
#include "jFileUtil.h"
#include "jDirUtil.h"
#include "jErrno.h"
#include <stdlib.h>
#include <stdio.h>
#include "jAssert.h"

const JSize kMDIServerQSize   = 1;
const JSize kMDIMaxWaitTime   = 2;		// seconds
const JUtf8Byte kEndOfLine    = '\n';
const JUtf8Byte kEndOfMessage = '\0';

const JUtf8Byte* JMDIServer::kQuitOptionName = "--quit";

static const JString kServerReadyMsg("JMDIServer ready", kJFalse);
static const JString kServerBusyMsg ("JMDIServer busy",  kJFalse);

/******************************************************************************
 Constructor

 ******************************************************************************/

JMDIServer::JMDIServer
	(
	const JUtf8Byte* signature
	)
	:
	itsFirstTimeFlag(kJTrue)
{
	const JString socketName = GetMDISocketName(signature);
	ACE_OS::unlink(socketName.GetBytes());

	ACE_UNIX_Addr addr(socketName.GetBytes());
	itsAcceptor = jnew ACE_LSOCK_Acceptor(addr, 0, PF_UNIX, kMDIServerQSize);
	assert( itsAcceptor != nullptr );

	itsSocket = jnew ACE_LSOCK_Stream;
	assert( itsSocket != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JMDIServer::~JMDIServer()
{
	itsSocket->close();
	jdelete itsSocket;

	itsAcceptor->remove();
	jdelete itsAcceptor;
}

/******************************************************************************
 WillBeMDIServer (static)

	If the application supports MDI, main() should call this before creating
	an application object.  If this function returns kJFalse, main() can
	exit immediately.

 ******************************************************************************/

JBoolean
JMDIServer::WillBeMDIServer
	(
	const JUtf8Byte*	signature,
	const int			argc,
	char*				argv[]
	)
{
	assert( argc >= 1 );

	// If the socket doesn't exist, we will be the server.

	const JString socketName = GetMDISocketName(signature);
	if (!JUNIXSocketExists(socketName))
		{
		for (JUnsignedOffset i=0; i<argc; i++)
			{
			if (strcmp(argv[i], JMDIServer::kQuitOptionName) == 0)
				{
				return kJFalse;
				}
			}

		if (JNameUsed(socketName))
			{
			std::cerr << "Unable to initiate MDI because " << socketName << std::endl;
			std::cerr << "exists as something else." << std::endl;
			exit(1);
			}
		return kJTrue;
		}

	// open a connection to the existing server

	ACE_UNIX_Addr addr(socketName.GetBytes());
	ACE_LSOCK_Connector connector;
	ACE_LSOCK_Stream socket;
	if (connector.connect(socket, addr) == -1)
		{
		ACE_OS::unlink(socketName.GetBytes());
		return kJTrue;
		}

	JBoolean receivedFinishedFlag = kJFalse;

	// wait for "server ok" message

	JString serverStatus;
	const JBoolean serverOK =
		ReceiveLine(socket, kJTrue, &serverStatus, &receivedFinishedFlag);
	if (!serverOK && !JUNIXSocketExists(socketName))		// user deleted dead socket
		{
		socket.close();
		return kJTrue;
		}
	else if (!serverOK && ACE_OS::unlink(socketName.GetBytes()) == -1)
		{
		std::cerr << "Unable to transmit MDI request." << std::endl;
		std::cerr << socketName << "appears to be dead," << std::endl;
		std::cerr << "but an error ocurred while trying to delete it." << std::endl;
		exit(1);
		}
	else if (!serverOK)
		{
		socket.close();
		return kJTrue;
		}

	if (serverStatus == kServerBusyMsg)
		{
		std::cerr << argv[0] << " is busy, probably because of a blocking window." << std::endl;
		std::cerr << "(e.g. a dialog or an error message)" << std::endl;
		WaitForFinished(socket, receivedFinishedFlag);
		return kJFalse;
		}

	assert( serverStatus == kServerReadyMsg );

	// send our message

	const JString dir = JGetCurrentDirectory();
	SendLine(socket, dir);

	for (JUnsignedOffset i=0; i<argc; i++)
		{
		SendLine(socket, JString(argv[i], kJFalse));
		}

	WaitForFinished(socket, receivedFinishedFlag);
	return kJFalse;
}

/******************************************************************************
 HandleCmdLineOptions

	Convenience function to convert the command line options from the
	first invocation of the program into an MDI request.  This allows the
	argument parsing code to be in one place.

 ******************************************************************************/

void
JMDIServer::HandleCmdLineOptions
	(
	const int	argc,
	char*		argv[]
	)
{
	const JString dir = JGetCurrentDirectory();

	JPtrArray<JString> argList(JPtrArrayT::kDeleteAll);
	for (JUnsignedOffset i=0; i < (JIndex) argc; i++)
		{
		argList.Append(JString(argv[i], kJFalse));
		}

	HandleMDIRequest(dir, argList);
}

/******************************************************************************
 CheckForConnections

	By having an existing ACE_LSOCK_Stream, accept() is as fast as possible.

 ******************************************************************************/

void
JMDIServer::CheckForConnections()
{
	ACE_Time_Value dontWait(0,0);
	if (itsAcceptor->accept(*itsSocket, nullptr, &dontWait) != -1)
		{
		ProcessMDIMessage();
		}
}

/******************************************************************************
 ProcessMDIMessage (private)

	Unpacks MDI request and calls HandleMDIRequest().
	The socket is closed before returning.

 ******************************************************************************/

void
JMDIServer::ProcessMDIMessage()
{
	itsFirstTimeFlag = kJFalse;

	JBoolean receivedFinishedFlag = kJFalse;

	// tell them our status

	if (CanAcceptMDIRequest())
		{
		SendLine(*itsSocket, kServerReadyMsg);
		}
	else
		{
		SendLine(*itsSocket, kServerBusyMsg);
		WaitForFinished(*itsSocket, receivedFinishedFlag);
		return;
		}

	// receive their message

	JString dir;
	if (!ReceiveLine(*itsSocket, kJFalse, &dir, &receivedFinishedFlag))
		{
		itsSocket->close();
		return;
		}

	JPtrArray<JString> argList(JPtrArrayT::kDeleteAll);
	JString tempStr;
	while (ReceiveLine(*itsSocket, kJFalse, &tempStr, &receivedFinishedFlag))
		{
		argList.Append(tempStr);
		}

	WaitForFinished(*itsSocket, receivedFinishedFlag);

	// handle the request

	PreprocessArgList(&argList);

	if (!argList.IsEmpty())
		{
		HandleMDIRequest(dir, argList);
		}
}

/******************************************************************************
 PreprocessArgList (virtual protected)

	Derived classes can override to remove options that only make sense for
	an initial invocation.

 ******************************************************************************/

void
JMDIServer::PreprocessArgList
	(
	JPtrArray<JString>* argList
	)
{
}

/******************************************************************************
 GetMDISocketName (static private)

 ******************************************************************************/

JString
JMDIServer::GetMDISocketName
	(
	const JUtf8Byte* signature
	)
{
	JString path;
	const JBoolean ok = JGetTempDirectory(&path);
	assert( ok );

	JString name = path;
	name        += ".";
	name        += signature;
	name        += "_MDI_";
	name        += JGetUserName();
	return name;
}

/******************************************************************************
 SendLine (static private)

 ******************************************************************************/

void
JMDIServer::SendLine
	(
	ACE_LSOCK_Stream&	socket,
	const JString&		line
	)
{
	socket.send_n(line.GetBytes(), line.GetByteCount());

	JUtf8Byte c = kEndOfLine;
	socket.send_n(&c, 1);
}

/******************************************************************************
 ReceiveLine (static private)

	Returns kJFalse if the connection times out.

 ******************************************************************************/

JBoolean
JMDIServer::ReceiveLine
	(
	ACE_LSOCK_Stream&	socket,
	const JBoolean		block,
	JString*			line,
	JBoolean*			receivedFinishedFlag
	)
{
	line->Clear();

	const ACE_Time_Value timeout(kMDIMaxWaitTime);
	while (!(*receivedFinishedFlag))
		{
		JUtf8Byte c;
		int result;
		if (block)
			{
			result = socket.recv_n(&c, 1);
			}
		else
			{
			result = socket.recv(&c, 1, &timeout);
			}

		if (result == 1 && c == kEndOfLine)
			{
			return kJTrue;
			}
		else if ((result == 1 && c == kEndOfMessage) ||
				 jerrno() == ETIME)
			{
			*receivedFinishedFlag = kJTrue;
			return kJFalse;
			}
		else if (result == 1)
			{
			line->Append(JUtf8Character(c));
			}
		}

	return kJFalse;
}

/******************************************************************************
 WaitForFinished (static private)

	This tosses all unread data.

 ******************************************************************************/

void
JMDIServer::WaitForFinished
	(
	ACE_LSOCK_Stream&	socket,
	const JBoolean		receivedFinishedFlag
	)
{
	// Tell the other end that we are finished.

	JUtf8Byte c = kEndOfMessage;
	socket.send_n(&c, 1);

	// To avoid a broken pipe error, we have to wait until they are also finished.
	// We use recv() instead of recv_n() so we get -1 if the sender dies.
	// (Note that this also returns if recv() returns -1.)

	if (!receivedFinishedFlag)
		{
		while (socket.recv(&c, 1) == 0 || c != kEndOfMessage)
			{ };
		}

	socket.close();
}
