#ifndef _H_jMissingProto
#define _H_jMissingProto

// prototypes and functions missing from AIX 4.x

#include <stdlib.h>

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
