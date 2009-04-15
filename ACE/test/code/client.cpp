/******************************************************************************
 client.cpp

	Bare bones ACE client that can use either an INET or a UNIX socket.

	Check OpenConnection() for cmd line arguments.

	Written by John Lindal.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include "util.h"
#include <ace/SOCK_Connector.h>
#include <ace/LSOCK_Connector.h>
#include <stdlib.h>
#include <jErrno.h>
#include <jAssert.h>

// Prototypes

ACE_SOCK_Stream*	OpenConnection(int argc, char* argv[]);
ACE_SOCK_Stream*	INETConnect(const JCharacter* inetAddr);
ACE_SOCK_Stream*	UNIXConnect(const JCharacter* socketName);
void				ReportConnectError();

/******************************************************************************
 main

 ******************************************************************************/

int
main
	(
	int		argc,
	char*	argv[]
	)
{
	// Open the requested connection.

	ACE_SOCK_Stream* socket = OpenConnection(argc, argv);

	// Send and receive messages.

	cout << "Connection established" << endl;
	TalkToServer(*socket);

	delete socket;
	return 0;
}

/******************************************************************************
 OpenConnection

	-i [host_name:]port_number	=> INET socket
	-u socket_name				=> UNIX socket

 ******************************************************************************/

ACE_SOCK_Stream*
OpenConnection
	(
	int		argc,
	char*	argv[]
	)
{
	if (argc == 3 && strcmp(argv[1], "-i") == 0)
		{
		return INETConnect(argv[2]);
		}
	else if (argc == 3 && strcmp(argv[1], "-u") == 0)
		{
		return UNIXConnect(argv[2]);
		}
	else
		{
		cerr << "usage: " << argv[0];
		cerr << " (-i [host_name:]port_number)|(-u socket_name)" << endl;
		exit(1);
		}
}

/******************************************************************************
 INETConnect

	Connects to the specified INET port.

 ******************************************************************************/

ACE_SOCK_Stream*
INETConnect
	(
	const JCharacter* inetAddr
	)
{
	// This specifies the address to which we will connect.

	ACE_INET_Addr addr(inetAddr);

	// The connector will hook up a stream to the given address.

	ACE_SOCK_Connector connector;

	// This is the stream from which we will read and to which we will write.

	ACE_SOCK_Stream* socket = new ACE_SOCK_Stream;
	assert( socket != NULL );

	// Try to get a connection to the server.  If we pass maxWait = {0,0},
	// then we have to expect a return value of -1 and jerrno() == EWOULDBLOCK.
	// In this case, we could do other things and call complete() later.

	ACE_Time_Value maxWait(kMaxWaitSeconds);
	if (connector.connect(*socket, addr, &maxWait) == -1)
		{
		ReportConnectError();
		}

	// This seems unnecessary in our case?
/*
	if (socket->disable(ACE_NONBLOCK) == -1) 
		{
		cerr << "error trying to enable blocking mode: " << jerrno() << endl;
		socket->close();
		exit(1);
		}
*/
	return socket;
}

/******************************************************************************
 UNIXConnect

	Connects to the specified named UNIX socket.

 ******************************************************************************/

ACE_SOCK_Stream*
UNIXConnect
	(
	const JCharacter* socketName
	)
{
	// This specifies the address to which we will connect.

	ACE_UNIX_Addr addr(socketName);

	// The connector will hook up a stream to the given address.

	ACE_LSOCK_Connector connector;

	// This is the stream from which we will read and to which we will write.

	ACE_LSOCK_Stream* socket = new ACE_LSOCK_Stream;
	assert( socket != NULL );

	// Try to get a connection to the server.  If we pass maxWait = {0,0},
	// then we have to expect a return value of -1 and jerrno() == EWOULDBLOCK.
	// In this case, we could do other things and call complete() later.

	// We can't put this higher up because ACE_LSOCK_Connector wants
	// an ACE_LSOCK_Stream, not an ACE_SOCK_Stream.

	ACE_Time_Value maxWait(kMaxWaitSeconds);
	if (connector.connect(*socket, addr, &maxWait) == -1)
		{
		ReportConnectError();
		}

	return socket;
}

/******************************************************************************
 ReportConnectError

 ******************************************************************************/

void
ReportConnectError()
{
	const int err = jerrno();
	if (err == ETIME)
		{
		cerr << "server not responding after ";
		cerr << kMaxWaitSeconds << " sec" << endl;
		}
	else
		{
		cerr << "error trying to connect: " << jerrno() << endl;
		}
	exit(1);
}
