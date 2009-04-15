#ifndef _H_jMissingProto
#define _H_jMissingProto

// prototypes and functions missing from IRIX 5.x

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <bstring.h>

int killpg(int, int);

inline int
usleep
	(
	unsigned long microSeconds
	)
{
	fd_set         readfds, writefds, exceptfds;
	struct timeval timer;

	timer.tv_sec  = microSeconds / (unsigned long) 1000000;
	timer.tv_usec = microSeconds % (unsigned long) 1000000;

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);
	if (select(0, &readfds, &writefds, &exceptfds, &timer) < 0)
		{
		perror( "usleep (select) failed" );
		return -1;
		}

	return 0;
}

inline int
setenv
	(
	const char*	name,
	const char*	value,
	int			overwrite
	)
{
	if (overwrite || getenv(name) == NULL)
		{
		const int nameLen = strlen(name);
		char* str = new char [ nameLen + strlen(value) + 2 ];
		strcpy(str, name);
		str[nameLen] = '=';
		strcpy(str + nameLen + 1, value);
		const int result = putenv(str);
		delete [] str;
		return result;
		}
	else
		{
		return 0;
		}
}

#endif
