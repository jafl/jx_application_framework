/******************************************************************************
 JSimpleProcess.cpp

	Class to run a program and report to the user if an error occurs.
	We use a JMessageProtocol to accumulate the text from the process
	so the pipe doesn't fill up.

	If the process runs for more than 5 seconds, we ignore errors because
	the user will surely have forgotten about it by then.  If errors are
	that important, it ought to have been run in an xterm!

	BASE CLASS = JProcess

	Copyright (C) 1999 by John Lindal.

 *****************************************************************************/

#include "JSimpleProcess.h"
#include "JThisProcess.h"
#include "jGlobals.h"
#include "jAssert.h"

static const time_t kMaxReportInverval = 5;		// seconds

/******************************************************************************
 Create (static)

	The versions that don't return a pointer to the object imply
	deleteWhenFinished and automatically report any error from JExecute().

 ******************************************************************************/

JError
JSimpleProcess::Create
	(
	const JString&	cmdStr,
	const JBoolean	detach
	)
{
	JSimpleProcess* p;
	const JError err = Create(&p, cmdStr, kJTrue);
	err.ReportIfError();
	if (err.OK() && detach)
		{
		JThisProcess::Ignore(p);
		}
	return err;
}

JError
JSimpleProcess::Create
	(
	JSimpleProcess**	process,
	const JString&		cmdStr,
	const JBoolean		deleteWhenFinished
	)
{
	pid_t childPID;
	int errFD;
	const JError err = JExecute(cmdStr, &childPID,
								kJIgnoreConnection, nullptr,
								kJTossOutput, nullptr,
								kJCreatePipe, &errFD);
	if (err.OK())
		{
		*process = jnew JSimpleProcess(childPID, errFD, deleteWhenFinished);
		assert( *process != nullptr );
		}
	else
		{
		*process = nullptr;
		}

	return err;
}

JError
JSimpleProcess::Create
	(
	const JString&	workingDirectory,
	const JString&	cmdStr,
	const JBoolean	detach
	)
{
	JSimpleProcess* p;
	const JError err = Create(&p, workingDirectory, cmdStr, kJTrue);
	err.ReportIfError();
	if (err.OK() && detach)
		{
		JThisProcess::Ignore(p);
		}
	return err;
}

JError
JSimpleProcess::Create
	(
	JSimpleProcess**	process,
	const JString&		workingDirectory,
	const JString&		cmdStr,
	const JBoolean		deleteWhenFinished
	)
{
	pid_t childPID;
	int errFD;
	const JError err = JExecute(workingDirectory, cmdStr, &childPID,
								kJIgnoreConnection, nullptr,
								kJTossOutput, nullptr,
								kJCreatePipe, &errFD);
	if (err.OK())
		{
		*process = jnew JSimpleProcess(childPID, errFD, deleteWhenFinished);
		assert( *process != nullptr );
		}
	else
		{
		*process = nullptr;
		}

	return err;
}

JError
JSimpleProcess::Create
	(
	const JPtrArray<JString>&	argList,
	const JBoolean				detach
	)
{
	JSimpleProcess* p;
	const JError err = Create(&p, argList, kJTrue);
	err.ReportIfError();
	if (err.OK() && detach)
		{
		JThisProcess::Ignore(p);
		}
	return err;
}

JError
JSimpleProcess::Create
	(
	JSimpleProcess**			process,
	const JPtrArray<JString>&	argList,
	const JBoolean				deleteWhenFinished
	)
{
	pid_t childPID;
	int errFD;
	const JError err = JExecute(argList, &childPID,
								kJIgnoreConnection, nullptr,
								kJTossOutput, nullptr,
								kJCreatePipe, &errFD);
	if (err.OK())
		{
		*process = jnew JSimpleProcess(childPID, errFD, deleteWhenFinished);
		assert( *process != nullptr );
		}
	else
		{
		*process = nullptr;
		}

	return err;
}

JError
JSimpleProcess::Create
	(
	const JString&				workingDirectory,
	const JPtrArray<JString>&	argList,
	const JBoolean				detach
	)
{
	JSimpleProcess* p;
	const JError err = Create(&p, workingDirectory, argList, kJTrue);
	err.ReportIfError();
	if (err.OK() && detach)
		{
		JThisProcess::Ignore(p);
		}
	return err;
}

JError
JSimpleProcess::Create
	(
	JSimpleProcess**			process,
	const JString&				workingDirectory,
	const JPtrArray<JString>&	argList,
	const JBoolean				deleteWhenFinished
	)
{
	pid_t childPID;
	int errFD;
	const JError err = JExecute(workingDirectory, argList, &childPID,
								kJIgnoreConnection, nullptr,
								kJTossOutput, nullptr,
								kJCreatePipe, &errFD);
	if (err.OK())
		{
		*process = jnew JSimpleProcess(childPID, errFD, deleteWhenFinished);
		assert( *process != nullptr );
		}
	else
		{
		*process = nullptr;
		}

	return err;
}

JError
JSimpleProcess::Create
	(
	const JUtf8Byte*	argv[],
	const JSize			size,
	const JBoolean		detach
	)
{
	JSimpleProcess* p;
	const JError err = Create(&p, argv, size, kJTrue);
	err.ReportIfError();
	if (err.OK() && detach)
		{
		JThisProcess::Ignore(p);
		}
	return err;
}

JError
JSimpleProcess::Create
	(
	JSimpleProcess**	process,
	const JUtf8Byte*	argv[],
	const JSize			size,
	const JBoolean		deleteWhenFinished
	)
{
	pid_t childPID;
	int errFD;
	const JError err = JExecute(argv, size, &childPID,
								kJIgnoreConnection, nullptr,
								kJTossOutput, nullptr,
								kJCreatePipe, &errFD);
	if (err.OK())
		{
		*process = jnew JSimpleProcess(childPID, errFD, deleteWhenFinished);
		assert( *process != nullptr );
		}
	else
		{
		*process = nullptr;
		}

	return err;
}

JError
JSimpleProcess::Create
	(
	const JString&		workingDirectory,
	const JUtf8Byte*	argv[],
	const JSize			size,
	const JBoolean		detach
	)
{
	JSimpleProcess* p;
	const JError err = Create(&p, workingDirectory, argv, size, kJTrue);
	err.ReportIfError();
	if (err.OK() && detach)
		{
		JThisProcess::Ignore(p);
		}
	return err;
}

JError
JSimpleProcess::Create
	(
	JSimpleProcess**	process,
	const JString&		workingDirectory,
	const JUtf8Byte*	argv[],
	const JSize			size,
	const JBoolean		deleteWhenFinished
	)
{
	pid_t childPID;
	int errFD;
	const JError err = JExecute(workingDirectory, argv, size, &childPID,
								kJIgnoreConnection, nullptr,
								kJTossOutput, nullptr,
								kJCreatePipe, &errFD);
	if (err.OK())
		{
		*process = jnew JSimpleProcess(childPID, errFD, deleteWhenFinished);
		assert( *process != nullptr );
		}
	else
		{
		*process = nullptr;
		}

	return err;
}

/******************************************************************************
 Constructor

 ******************************************************************************/

JSimpleProcess::JSimpleProcess
	(
	const pid_t		pid,
	const int		fd,
	const JBoolean	deleteWhenFinished
	)
	:
	JProcess(pid),
	itsStartTime(time(nullptr))
{
	itsLink = new ProcessLink(fd);
	assert( itsLink != nullptr );

	ListenTo(this);

	ShouldDeleteWhenFinished(deleteWhenFinished);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSimpleProcess::~JSimpleProcess()
{
	delete itsLink;
	itsLink = nullptr;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JSimpleProcess::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JProcess::kFinished))
		{
		const JProcess::Finished* info =
			dynamic_cast<const JProcess::Finished*>(&message);
		assert( info != nullptr );
		ReportError(info->Successful());
		}
	else
		{
		JProcess::Receive(sender, message);
		}
}

/******************************************************************************
 ReportError

 ******************************************************************************/

void
JSimpleProcess::ReportError
	(
	const JBoolean success
	)
{
	StopListening(this);

	JThisProcess::CheckACEReactor();

	JString lastLine;
	const JBoolean hasPartialLine = itsLink->PeekPartialMessage(&lastLine);

	if ((itsLink->HasMessages() || hasPartialLine) &&
		time(nullptr) < itsStartTime + kMaxReportInverval)
		{
		JString text, line;
		while (itsLink->GetNextMessage(&line))
			{
			if (!text.IsEmpty())
				{
				text.Append("\n");
				}
			text += line;
			}

		if (hasPartialLine)
			{
			if (!text.IsEmpty())
				{
				text.Append("\n");
				}
			text += lastLine;
			}

		JUserNotification::SetBreakMessageCROnly();

		if (success)
			{
			text.Prepend("The program reported the following:\n\n");
			(JGetUserNotification())->DisplayMessage(text);
			}
		else
			{
			text.Prepend("An error occurred:\n\n");
			(JGetUserNotification())->ReportError(text);
			}
		}
}
