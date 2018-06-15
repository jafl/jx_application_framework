/******************************************************************************
 jCommandLine.cpp

	Routines useful in a command line interface.

	Copyright (C) 1994-99 by John Lindal.

 ******************************************************************************/

#include "jCommandLine.h"
#include <stdlib.h>
#include <string.h>
#include "jAssert.h"

/******************************************************************************
 JWaitForReturn

 ******************************************************************************/

void
JWaitForReturn()
{
	std::cout << std::endl << "Press return to continue...";

	std::cin.clear();
	while (std::cin.get() != '\n') { };

	std::cout << std::endl;
}

/******************************************************************************
 JInputFinished

	When you are done getting information from the user, call this to
	remove the last return typed by the user.

	This ensures that JWaitForReturn will work the first time.

 ******************************************************************************/

void
JInputFinished()
{
	std::cin.clear();
	std::cin.get();
}

/******************************************************************************
 JCheckForValues

	Useful when parsing command line options.  *index must point to the
	option marker.  If there are fewer than valueCount arguments following
	this marker, notifies user how many arguments are missing and exits.
	Otherwise, increments *index so it points to the first value.

 ******************************************************************************/

void
JCheckForValues
	(
	const JSize	valueCount,
	JIndex*		index,
	const JSize	argc,
	char*		argv[]
	)
{
	assert( *index < argc );

	const JSize remaining = argc - (*index+1);
	if (valueCount > remaining)
		{
		std::cerr << argv[0] << ": missing " << valueCount - remaining;
		std::cerr << " argument(s) for " << argv[*index] << std::endl;
		exit(1);
		}

	(*index)++;
}

/******************************************************************************
 JIsVersionRequest

 ******************************************************************************/

JBoolean
JIsVersionRequest
	(
	const JUtf8Byte* arg
	)
{
	return JI2B(strcmp(arg, "-v")        == 0 ||
				strcmp(arg, "--version") == 0);
}

/******************************************************************************
 JIsHelpRequest

 ******************************************************************************/

JBoolean
JIsHelpRequest
	(
	const JUtf8Byte* arg
	)
{
	return JI2B(strcmp(arg, "-h")     == 0 ||
				strcmp(arg, "-?")     == 0 ||
				strcmp(arg, "--help") == 0);
}
