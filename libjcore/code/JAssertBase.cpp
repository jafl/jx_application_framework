/******************************************************************************
 JAssertBase.cpp

	Base class for global object that handles assert() macro.

	To use this system, create a derived class and pass it to JInitCore().

	Derived classes must override Assert().  This returns zero if the
	program is supposed to continue.  Otherwise, it should call abort().

	DefaultAssert() uses the environment variable J_ASSERT_ACTION to
	determine how the assert() macro behaves:

		"ask_user"			prints message and asks "abort or continue?"
		any other value		prints message and calls abort()
		undefined			prints message and calls abort()
		"silent"			ignores the failure (unsafe if assert() is used correctly)

	This action is settable at run-time via SetAction().

	BASE CLASS = none

	Copyright © 1997 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <JCoreStdInc.h>
#include <JAssertBase.h>
#include <jGlobals.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jMissingProto.h>
#include <jAssert.h>

static const JCharacter* kAssertActionEnvName = "J_ASSERT_ACTION";
static const JCharacter* kAskUserAction       = "ask_user";
static const JCharacter* kIgnoreFailureAction = "silent";

/******************************************************************************
 Constructor

 ******************************************************************************/

JAssertBase::JAssertBase()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JAssertBase::~JAssertBase()
{
}

/******************************************************************************
 Abort (virtual)

	Derived classes can override this to perform clean-up.

 ******************************************************************************/

void
JAssertBase::Abort()
{
	abort();
}

/******************************************************************************
 DefaultAssert (static)

 ******************************************************************************/

int
JAssertBase::DefaultAssert
	(
	const JCharacter*	expr,
	const JCharacter*	file,
	const int			line
	)
{
	const Action action = GetAction();
	if (action != kIgnoreFailure)
		{
		fprintf(stderr, "\a\nAssertion failed: %s\n    %s:%d\n",
				expr, file, line);

		if (action == kAskUser)
			{
			fprintf(stderr, "Press 'c' to continue, \"return\" to exit: ");

			fflush(stderr);
			int ch = getchar();

			if (ch == 'c')
				{
				getchar();	// remove return key
				return 0;
				}
			}

		JAssertBase* ah;
		if (JGetAssertHandler(&ah))
			{
			ah->Abort();
			}
		else
			{
			abort();
			}
		}

	return 0;
}

/******************************************************************************
 GetAction (static)

 ******************************************************************************/

JAssertBase::Action
JAssertBase::GetAction()
{
	const char* action = getenv(kAssertActionEnvName);
	if (action == NULL)
		{
		return kAbort;
		}
	else if (strcmp(action, kIgnoreFailureAction) == 0)
		{
		return kIgnoreFailure;
		}
	else if (strcmp(action, kAskUserAction) == 0)
		{
		return kAskUser;
		}
	else
		{
		return kAbort;
		}
}

/******************************************************************************
 SetAction (static)

	We can't call unsetenv() because SunOS 4.1.4 doesn't have it.

 ******************************************************************************/

void
JAssertBase::SetAction
	(
	const Action action
	)
{
	if (action == kIgnoreFailure)
		{
		setenv(kAssertActionEnvName, kIgnoreFailureAction, 1);
		}
	else if (action == kAskUser)
		{
		setenv(kAssertActionEnvName, kAskUserAction, 1);
		}
	else
		{
		assert( action == kAbort );
		setenv(kAssertActionEnvName, "", 1);
		}
}
