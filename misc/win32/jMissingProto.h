#pragma once

#include <jFileUtil.h>
#include <JString.h>
#include <unistd.h>
#include <io.h>
#include <fcntl.h>
#include <sys/utime.h>

#define access		_access
#define utime		_utime
#define utimbuf		_utimbuf
#define	S_ISREG(x)	((x) & _S_IFREG)
#define S_ISDIR(x)	((x) & _S_IFDIR)
#define O_RDONLY	_O_RDONLY
#define ETXTBSY		-1

typedef int	ssize_t;

inline int
setenv
	(
	const char*	name,
	const char*	value,
	const int	overwrite
	)
{
	JString s = name;
	s        += "=";
	s        += value;
	return putenv(s);
}
