/******************************************************************************
 evt_client.cpp

	Event driven ACE client that can use either an INET or a UNIX socket.

	Check OpenConnection() for cmd line arguments.

	Written by John Lindal.

 ******************************************************************************/

#include "evt_handlers.h"
#include "util.h"
#include <ace/Connector.h>
#include <ace/SOCK_Connector.h>
#include <ace/LSOCK_Connector.h>
#include <stdlib.h>
#include <jErrno.h>
#include <jAssert.h>

// jAssert.h includes jNew.h, which redefines these as macros.
// These macros conflict with ACE::operator new().

// notation

typedef ClientHandler<ACE_SOCK_STREAM>	INETHandler;
typedef ClientHandler<ACE_LSOCK_STREAM>	UNIXHandler;

typedef ACE_Connector<INETHandler, ACE_SOCK_CONNECTOR>	INETConnector;
typedef ACE_Connector<UNIXHandler, ACE_LSOCK_CONNECTOR>	UNIXConnector;

// Prototypes

ACE_Event_Handler*	OpenConnection(int argc, char* argv[]);
ACE_Event_Handler*	INETConnect(const JCharacter* inetAddr);
ACE_Event_Handler*	UNIXConnect(const JCharacter* socketName);

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
	// Initiate the requested connection.

	ACE_Event_Handler* connector = OpenConnection(argc, argv);

	// Start the event loop.

	ACE_Reactor::run_event_loop();

	// clean up

	delete connector;
	return 0;
}

/******************************************************************************
 OpenConnection

	-i [host_name:]port_number	=> INET socket
	-u socket_name				=> UNIX socket

 ******************************************************************************/

ACE_Event_Handler*
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

	// keep the compiler happy
	return NULL;
}

/******************************************************************************
 INETConnect

	Connects to the specified INET port.

 ******************************************************************************/

ACE_Event_Handler*
INETConnect
	(
	const JCharacter* inetAddr
	)
{
	// This specifies the address to which we will connect.

	ACE_INET_Addr addr(inetAddr);

	// This object will handle the transaction with the server.

	INETHandler* handler = new INETHandler;
	assert( handler != NULL );

	// This object will hook the INETHandler's ACE_SOCK_Stream up when
	// a connection is established.

	INETConnector* connector = new INETConnector;
	assert( connector != NULL );

	// Begin waiting for a connection.

	ACE_Synch_Options options(ACE_Synch_Options::USE_REACTOR, ACE_Time_Value(kMaxWaitSeconds));

	if (connector->connect(handler, addr, options) == -1 &&
		jerrno() != EAGAIN)
		{
		cerr << "error trying to connect: " << jerrno() << endl;
		exit(1);
		}

	return connector;
}

/******************************************************************************
 UNIXConnect

	Connects to the specified named UNIX socket.

 ******************************************************************************/

ACE_Event_Handler*
UNIXConnect
	(
	const JCharacter* socketName
	)
{
	// This specifies the address to which we will connect.

	ACE_UNIX_Addr addr(socketName);

	// This object will handle the transaction with the server.

	UNIXHandler* handler = new UNIXHandler;
	assert( handler != NULL );

	// This object will hook the UNIXHandler's ACE_LSOCK_Stream up when
	// a connection is established.

	UNIXConnector* connector = new UNIXConnector;
	assert( connector != NULL );

	// Begin waiting for a connection.

	ACE_Synch_Options options(ACE_Synch_Options::USE_REACTOR, ACE_Time_Value(kMaxWaitSeconds));

	if (connector->connect(handler, addr, options) == -1 &&
		jerrno() != EAGAIN)
		{
		cerr << "error trying to connect: " << jerrno() << endl;
		exit(1);
		}

	return connector;
}
