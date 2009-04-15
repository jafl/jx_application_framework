#ifndef _H_jMissingProto
#define _H_jMissingProto

#include <jFileUtil.h>
#include <JKLRand.h>
#include <string.h>

// prototypes and functions missing from CygWin

inline char*
mkdtemp
	(
	char* tmpl
	)
{
	char* p = strstr(tmpl, "XX");
	if (p == NULL)
		{
		return NULL;
		}

	int len = 2;
	while (*(p + len) == 'X')
		{
		len++;
		}

	JKLRand r;
	while (1)
		{
		for (int i=0; i<len; i++)
			{
			*(p+i) = (char) r.UniformLong('a', 'z');
			}

		if (!JNameUsed(tmpl))
			{
			return tmpl;
			}
		}

	return NULL;
}

#endif
