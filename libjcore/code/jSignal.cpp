/******************************************************************************
 jSignal.cpp

	Copyright (C) 1997 John Lindal.

 ******************************************************************************/

#include "jSignal.h"
#include "jGlobals.h"
#include "jAssert.h"

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
		return JGetString("SIGINT::jSignal");
	}
	else if (value == SIGQUIT)
	{
		return JGetString("SIGQUIT::jSignal");
	}
	else if (value == SIGILL)
	{
		return JGetString("SIGILL::jSignal");
	}
	else if (value == SIGABRT)
	{
		return JGetString("SIGABRT::jSignal");
	}
	else if (value == SIGFPE)
	{
		return JGetString("SIGFPE::jSignal");
	}
	else if (value == SIGKILL)
	{
		return JGetString("SIGKILL::jSignal");
	}
	else if (value == SIGSEGV)
	{
		return JGetString("SIGSEGV::jSignal");
	}
	else if (value == SIGPIPE)
	{
		return JGetString("SIGPIPE::jSignal");
	}
	else if (value == SIGALRM)
	{
		return JGetString("SIGALRM::jSignal");
	}
	else if (value == SIGTERM)
	{
		return JGetString("SIGTERM::jSignal");
	}
	else if (value == SIGSTOP)
	{
		return JGetString("SIGSTOP::jSignal");
	}
	else if (value == SIGBUS)
	{
		return JGetString("SIGBUS::jSignal");
	}
	else
	{
		const JString sigName((JUInt64) value);
		const JUtf8Byte* map[] =
		{
			"name", sigName.GetBytes()
		};
		return JGetString("unknown::jSignal", map, sizeof(map));
	}
}
