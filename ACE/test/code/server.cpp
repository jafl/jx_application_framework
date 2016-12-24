/******************************************************************************
 server.cpp

	Bare bones ACE server that can use either an INET or a UNIX socket.

	Written by John Lindal.

 ******************************************************************************/

#include "util.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/LSOCK_Acceptor.h>
#include <JString.h>
#include <jTime.h>
#include <stdlib.h>
#include <jErrno.h>
#include <jAssert.h>

// Prototypes

void	RunINETServer(const long portNumber);
void	RunUNIXServer(const JCharacter* socketName);

/******************************************************************************
 main

	-i port_number	=> INET socket
	-u socket_name	=> UNIX socket

 ******************************************************************************/

int
main
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
			RunINETServer(portNumber);
			}
		else
			{
			std::cerr << "invalid port number" << std::endl;
			return 1;
			}
		}

	else if (argc == 3 && strcmp(argv[1], "-u") == 0)
		{
		RunUNIXServer(argv[2]);
		}

	else
		{
		std::cerr << "usage: " << argv[0];
		std::cerr << " (-i port_number)|(-u socket_name)" << std::endl;
		return 1;
		}

	return 0;
}

/******************************************************************************
 RunINETServer

 ******************************************************************************/

void
RunINETServer
	(
	const long portNumber
	)
{
	// This specifies the port to which we will listen.

	ACE_INET_Addr addr(portNumber);

	// The acceptor listens for a connection on a given address.
	// When it gets one, it will hook up a stream for us.

	ACE_SOCK_Acceptor acceptor;

	// This is the stream from which we will read and to which we will write.
	// We can reuse it for every connection, since we only process one at a time.

	ACE_SOCK_Stream socket;

	// Create the socket.

	if (acceptor.open(addr) == -1)
		{
		std::cerr << "error trying to open port: " << jerrno() << std::endl;
		exit(1);
		}

	// Process 3 connections.  This lets us test the shutdown code easily.

	for (JSize i=1; i<=3; i++)
		{
		if (acceptor.accept(socket) == -1)
			{
			std::cerr << "error waiting for connection: " << jerrno() << std::endl;
			break;
			}

		// Send and receive messages.

		std::cout << "Connection established, fd=" << socket.get_handle() << std::endl;
		if (!TalkToClient(socket))
			{
			i--;
			}

		// Close the socket.

		if (socket.close() == -1) 
			{
			std::cerr << "error trying to close connection: " << jerrno() << std::endl;
			}

		// This tests that queued clients are willing to wait.

		JWait(2.0);
		}

	// Close the socket.  This does not happen automatically.

	acceptor.close();
}

/******************************************************************************
 RunUNIXServer

 ******************************************************************************/

void
RunUNIXServer
	(
	const JCharacter* socketName
	)
{
	// unlink() ensures that any existing socket will keep its current
	// connections, but we want to get any new ones.

	ACE_OS::unlink(socketName);

	// This specifies the socket that we will create.

	ACE_UNIX_Addr addr(socketName);

	// The acceptor listens for a connection on a given address.
	// When it gets one, it will hook up a stream for us.

	ACE_LSOCK_Acceptor acceptor;

	// This is the stream from which we will read and to which we will write.
	// We can reuse it for every connection, since we only process one at a time.

	ACE_LSOCK_Stream socket;

	// Create the socket.

	if (acceptor.open(addr) == -1)
		{
		std::cerr << "error trying to create socket: " << jerrno() << std::endl;
		exit(1);
		}

	// Process 3 connections.  This lets us test the shutdown code easily.

	for (JIndex i=1; i<=3; i++)
		{
		if (acceptor.accept(socket) == -1)
			{
			std::cerr << "error waiting for connection: " << jerrno() << std::endl;
			break;
			}

		// Send and receive messages.

		std::cout << "Connection established, fd=" << socket.get_handle() << std::endl;
		if (!TalkToClient(socket))
			{
			i--;
			}

		// Close the socket.

		if (socket.close() == -1) 
			{
			std::cerr << "error trying to close connection: " << jerrno() << std::endl;
			}

		// This tests that queued clients are willing to wait.

		JWait(2.0);
		}

	// Close the socket.  This does not happen automatically.

	acceptor.remove();
}
