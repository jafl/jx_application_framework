#ifndef _H_jMissingProto
#define _H_jMissingProto

// prototypes and functions missing from HP-UX 9

#include <time.h>
#include <stdio.h>

inline int
usleep
	(
	unsigned long microSeconds
	)
{
	int            readfds, writefds, exceptfds;
	struct timeval timer;

	timer.tv_sec  = microSeconds / (unsigned long) 1000000;
	timer.tv_usec = microSeconds % (unsigned long) 1000000;

	readfds = writefds = exceptfds = 0;
	if (select(0, &readfds, &writefds, &exceptfds, &timer) < 0)
		{
		perror( "usleep (select) failed" );
		return -1;
		}

	return 0;
}

#endif
