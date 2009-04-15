#ifndef _H_jMissingProto
#define _H_jMissingProto

// prototypes and functions missing from SunOS

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define SSIZE_MAX       INT_MAX
#define UNIX_PATH_MAX   108

typedef int ssize_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef int sig_atomic_t;

int ftruncate(int fd, off_t length);
int readlink(const char *path, char *buf, size_t len);

void usleep(unsigned long usec);

int getitimer(int which, struct itimerval *value);
int setitimer(int which, const struct itimerval *value, struct itimerval *ovalue);

int select (int width, fd_set* readfds, fd_set* writefds,
            fd_set* exceptfds, struct timeval* timeout);
void bzero(char* b, int length);

int killpg(int pgrp, int sig);

int socket(int domain, int type, int protocol);
int connect(int s, const struct sockaddr* name, int namelen);
int listen(int s, int backlog);
int accept(int s, const struct sockaddr* addr, int* addrlen);
int bind(int s, const struct sockaddr* name, int namelen);

#ifdef __cplusplus
}
#endif

inline pid_t
getpgid
	(
	pid_t pid
	)
{
	return getpgrp(pid);
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
