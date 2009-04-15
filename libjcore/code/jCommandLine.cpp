/******************************************************************************
 jCommandLine.cpp

	Routines useful in a command line interface.

	Copyright © 1994-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jCommandLine.h>
#include <stdlib.h>
#include <string.h>
#include <jAssert.h>

/******************************************************************************
 JWaitForReturn

 ******************************************************************************/

void
JWaitForReturn()
{
	cout << endl << "Press return to continue...";

	cin.clear();
	while (cin.get() != '\n') { };

	cout << endl;
}

/******************************************************************************
 JInputFinished

	When you are done getting information from the user, call this to
	remove the last return typed by the user.

	This insures that JWaitForReturn will work the first time.

 ******************************************************************************/

void
JInputFinished()
{
	cin.clear();
	cin.get();
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
		cerr << argv[0] << ": missing " << valueCount - remaining;
		cerr << " argument(s) for " << argv[*index] << endl;
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
	const JCharacter* arg
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
	const JCharacter* arg
	)
{
	return JI2B(strcmp(arg, "-h")     == 0 ||
				strcmp(arg, "-?")     == 0 ||
				strcmp(arg, "--help") == 0);
}
