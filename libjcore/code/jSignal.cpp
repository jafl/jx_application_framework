/******************************************************************************
 jSignal.cpp

	Copyright © 1997 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jSignal.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 JGetSignalName

	Returns a string describing the specified signal.

 ******************************************************************************/

JString
JGetSignalName
	(
	const int value
	)
{
	if (value == SIGINT)
		{
		return JString("interrupt signal (SIGINT)");
		}
	else if (value == SIGQUIT)
		{
		return JString("quit signal (SIGQUIT)");
		}
	else if (value == SIGILL)
		{
		return JString("illegal instruction (SIGILL)");
		}
	else if (value == SIGABRT)
		{
		return JString("call from abort() (SIGABRT)");
		}
	else if (value == SIGFPE)
		{
		return JString("floating point exception (SIGFPE)");
		}
	else if (value == SIGKILL)
		{
		return JString("kill signal (SIGKILL)");
		}
	else if (value == SIGSEGV)
		{
		return JString("segmentation fault (SIGSEGV)");
		}
	else if (value == SIGPIPE)
		{
		return JString("broken pipe (SIGPIPE)");
		}
	else if (value == SIGALRM)
		{
		return JString("timer signal from alarm (SIGALRM)");
		}
	else if (value == SIGTERM)
		{
		return JString("termination signal (SIGTERM)");
		}
	else if (value == SIGSTOP)
		{
		return JString("stop signal (SIGSTOP)");
		}
	else if (value == SIGBUS)
		{
		return JString("bus error (SIGBUS)");
		}
	else
		{
		JString sigName(value, 0);
		sigName.Prepend("signal ");
		return sigName;
		}
}
