/******************************************************************************
 evt_server.cpp

	Bare bones ACE server that can use either an INET or a UNIX socket.

	Check CreateServer() for cmd line arguments.

	*** Unresolved:
		Why does the program exit after the first connection?
		Inside ACE, this is triggered by handle_events() returning -1.

	Written by John Lindal.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include "evt_handlers.h"
#include "util.h"
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/LSOCK_Acceptor.h>
#include <JString.h>
#include <stdlib.h>
#include <jErrno.h>
#include <jAssert.h>

// Notation

typedef ServerHandler<ACE_SOCK_STREAM>	INETHandler;
typedef ServerHandler<ACE_LSOCK_STREAM>	UNIXHandler;

typedef ACE_Acceptor<INETHandler, ACE_SOCK_ACCEPTOR>	INETAcceptor;

class UNIXAcceptor :
	public ACE_Acceptor<UNIXHandler, ACE_LSOCK_ACCEPTOR>
{
public:

	virtual ~UNIXAcceptor()
		{
		(acceptor()).remove();
		}
};

// Prototypes

ACE_Event_Handler*	CreateServer(int argc, char* argv[]);
ACE_Event_Handler*	CreateINETServer(const long portNumber);
ACE_Event_Handler*	CreateUNIXServer(const JCharacter* socketName);

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
	// Create the server.

	ACE_Event_Handler* acceptor = CreateServer(argc, argv);

	// Start the event loop.

	ACE_Reactor::run_event_loop();

	// clean up

	delete acceptor;
	return 0;
}

/******************************************************************************
 CreateServer

	-i port_number	=> INET socket
	-u socket_name	=> UNIX socket

 ******************************************************************************/

ACE_Event_Handler*
CreateServer
	(
	int		argc,
	char*	argv[]
	)
{
	if (argc == 3 && strcmp(argv[1], "-i") == 0)
		{
		JUInt portNumber;
		if (JString::ConvertToUInt(argv[2], &portNumber))
			{
			return CreateINETServer(portNumber);
			}
		else
			{
			cerr << "invalid port number" << endl;
			exit(1);
			}
		}

	else if (argc == 3 && strcmp(argv[1], "-u") == 0)
		{
		return CreateUNIXServer(argv[2]);
		}

	else
		{
		cerr << "usage: " << argv[0];
		cerr << " (-i port_number)|(-u socket_name)" << endl;
		exit(1);
		}

	// keep the compiler happy
	return NULL;
}

/******************************************************************************
 CreateINETServer

 ******************************************************************************/

ACE_Event_Handler*
CreateINETServer
	(
	const long portNumber
	)
{
	// This specifies the address to which we will connect.

	ACE_INET_Addr addr(portNumber);

	// This object will create an INETHandler when a client connects.

	INETAcceptor* acceptor = new INETAcceptor;
	assert( acceptor != NULL );

	// Begin waiting for a connection.

	if (acceptor->open(addr) == -1)
		{
		cerr << "error trying to open port: " << jerrno() << endl;
		exit(1);
		}

	return acceptor;
}

/******************************************************************************
 CreateUNIXServer

 ******************************************************************************/

ACE_Event_Handler*
CreateUNIXServer
	(
	const JCharacter* socketName
	)
{
	// unlink() insures that any existing socket will keep its current
	// connections, but we want to get any new ones.

	ACE_OS::unlink(socketName);

	// This specifies the address to which we will connect.

	ACE_UNIX_Addr addr(socketName);

	// This object will create a UNIXHandler when a client connects.

	UNIXAcceptor* acceptor = new UNIXAcceptor;
	assert( acceptor != NULL );

	// Begin waiting for a connection.

	if (acceptor->open(addr) == -1)
		{
		cerr << "error trying to create socket: " << jerrno() << endl;
		exit(1);
		}

	return acceptor;
}

/******************************************************************************
 Required templates

	These are defined because the linker asked for them.

 ******************************************************************************/

// INET

#define JTemplateName ACE_Acceptor
#define JTemplateType INETHandler, ACE_SOCK_ACCEPTOR
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

// UNIX

#define JTemplateName ACE_Acceptor
#define JTemplateType UNIXHandler, ACE_LSOCK_ACCEPTOR
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType
