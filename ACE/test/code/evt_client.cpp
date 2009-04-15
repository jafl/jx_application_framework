/******************************************************************************
 evt_client.cpp

	Event driven ACE client that can use either an INET or a UNIX socket.

	Check OpenConnection() for cmd line arguments.

	Written by John Lindal.

 ******************************************************************************/

#include <JCoreStdInc.h>
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

#undef new
#undef delete

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

	ACE_Synch_Options options(ACE_Synch_Options::USE_REACTOR, kMaxWaitSeconds);

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

	ACE_Synch_Options options(ACE_Synch_Options::USE_REACTOR, kMaxWaitSeconds);

	if (connector->connect(handler, addr, options) == -1 &&
		jerrno() != EAGAIN)
		{
		cerr << "error trying to connect: " << jerrno() << endl;
		exit(1);
		}

	return connector;
}

/******************************************************************************
 Required templates

	These are defined because the linker asked for them.

 ******************************************************************************/

#include <ace/Version.h>

// INET

#define JTemplateName ACE_Connector
#define JTemplateType INETHandler, ACE_SOCK_CONNECTOR
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#if ACE_MAJOR_VERSION > 5 || \
	(ACE_MAJOR_VERSION == 5 && ACE_MINOR_VERSION >= 4) || \
	(ACE_MAJOR_VERSION == 5 && ACE_MINOR_VERSION == 4 && ACE_BETA_VERSION >= 4)

#define JTemplateName ACE_NonBlocking_Connect_Handler
#define JTemplateType INETHandler
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Connector_Base
#define JTemplateType INETHandler
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#else

#define JTemplateName ACE_Map_Manager
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<INETHandler>*, ACE_SYNCH_RW_MUTEX
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Map_Iterator
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<INETHandler>*, ACE_SYNCH_RW_MUTEX
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Map_Reverse_Iterator
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<INETHandler>*, ACE_SYNCH_RW_MUTEX
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Map_Iterator_Base
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<INETHandler>*, ACE_SYNCH_RW_MUTEX
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Map_Entry
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<INETHandler>*
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Svc_Tuple
#define JTemplateType INETHandler
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#endif

// UNIX

#define JTemplateName ACE_Connector
#define JTemplateType UNIXHandler, ACE_LSOCK_CONNECTOR
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#if ACE_MAJOR_VERSION > 5 || \
	(ACE_MAJOR_VERSION == 5 && ACE_MINOR_VERSION >= 4) || \
	(ACE_MAJOR_VERSION == 5 && ACE_MINOR_VERSION == 4 && ACE_BETA_VERSION >= 4)

#define JTemplateName ACE_NonBlocking_Connect_Handler
#define JTemplateType UNIXHandler
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Connector_Base
#define JTemplateType UNIXHandler
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#else

#define JTemplateName ACE_Map_Manager
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<UNIXHandler>*, ACE_SYNCH_RW_MUTEX
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Map_Iterator
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<UNIXHandler>*, ACE_SYNCH_RW_MUTEX
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Map_Reverse_Iterator
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<UNIXHandler>*, ACE_SYNCH_RW_MUTEX
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Map_Iterator_Base
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<UNIXHandler>*, ACE_SYNCH_RW_MUTEX
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Map_Entry
#define JTemplateType ACE_HANDLE, ACE_Svc_Tuple<UNIXHandler>*
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Svc_Tuple
#define JTemplateType UNIXHandler
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

// generic

#define JTemplateName ACE_Read_Guard
#define JTemplateType ACE_Mutex
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Write_Guard
#define JTemplateType ACE_Mutex
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Guard
#define JTemplateType ACE_Mutex
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#endif
