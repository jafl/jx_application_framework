/******************************************************************************
 echo.cpp

	Echoes out everything that is sent from the child process.

	Written by John Lindal.

 ******************************************************************************/

#include "Pipe.h"
#include <ace/LSOCK_Stream.h>
#include <JProcess.h>
#include <jGlobals.h>
#include <jTime.h>
#include <jErrno.h>
#include <stdio.h>
#include <jAssert.h>

typedef Pipe<ACE_LSOCK_STREAM>	ProcessLink;

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
	if (argc == 1)
		{
		char buffer[BUFSIZ];
		while (1)
			{
			ssize_t count = read(0, buffer, sizeof(buffer));
			if (count < 0 && jerrno() == EINTR)
				{
				// keep going
				}
			else if (count <= 0)
				{
				break;
				}
			else
				{
//				std::cerr << count << std::endl;
				std::cout.write(buffer, count);
				JWait(0.01);
				}
			}
		return 0;
		}

	JProcess* p;
	int fromFD;
	const JError err = JProcess::Create(&p, argv[1],
										kJIgnoreConnection, nullptr,
										kJForceNonblockingPipe, &fromFD,
										kJAttachToFromFD, nullptr);
	if (err.OK())
		{
		char buffer[BUFSIZ];
		while (1)
			{
			ssize_t count = read(fromFD, buffer, sizeof(buffer));
			if (count < 0 && jerrno() == EINTR)
				{
				// keep going
				}
			else if (count <= 0)
				{
				break;
				}
			else
				{
//				std::cerr << count << std::endl;
				std::cout.write(buffer, count);
				JWait(0.02);
				}
			}
/*
		ProcessLink* link = new ProcessLink(fromFD);
		assert( link != nullptr );

		ACE_Time_Value delta(20);
		ACE_Reactor::instance()->run_reactor_event_loop(delta);

		link->Dump();
		jdelete link;
*/
		return 0;
		}
	else
		{
		JInitCore();
		err.ReportIfError();
		return 1;
		}
}
