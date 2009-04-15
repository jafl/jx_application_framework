/******************************************************************************
 JSimpleProcess.cpp

	Class to run a program and report to the user if an error occurs.
	We use a JMessageProtocol to accumulate the text from the process
	so the pipe doesn't fill up.

	If the process runs for more than 5 seconds, we ignore errors because
	the user will surely have forgotten about it by then.  If errors are
	that important, it ought to have been run in an xterm!

	BASE CLASS = JProcess

	Copyright © 1999 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <JCoreStdInc.h>
#include <JSimpleProcess.h>
#include <JThisProcess.h>
#include <jGlobals.h>
#include <jAssert.h>

static const time_t kMaxReportInverval = 5;		// seconds

/******************************************************************************
 Create (static)

	The versions that don't return a pointer to the object imply
	deleteWhenFinished and automatically report any error from JExecute().

 ******************************************************************************/

JError
JSimpleProcess::Create
	(
	const JCharacter*	cmdStr,
	const JBoolean		detach
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
	const JCharacter*	cmdStr,
	const JBoolean		deleteWhenFinished
	)
{
	pid_t childPID;
	int errFD;
	const JError err = JExecute(cmdStr, &childPID,
								kJIgnoreConnection, NULL,
								kJTossOutput, NULL,
								kJCreatePipe, &errFD);
	if (err.OK())
		{
		*process = new JSimpleProcess(childPID, errFD, deleteWhenFinished);
		assert( *process != NULL );
		}
	else
		{
		*process = NULL;
		}

	return err;
}

JError
JSimpleProcess::Create
	(
	const JCharacter*	workingDirectory,
	const JCharacter*	cmdStr,
	const JBoolean		detach
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
	const JCharacter*	workingDirectory,
	const JCharacter*	cmdStr,
	const JBoolean		deleteWhenFinished
	)
{
	pid_t childPID;
	int errFD;
	const JError err = JExecute(workingDirectory, cmdStr, &childPID,
								kJIgnoreConnection, NULL,
								kJTossOutput, NULL,
								kJCreatePipe, &errFD);
	if (err.OK())
		{
		*process = new JSimpleProcess(childPID, errFD, deleteWhenFinished);
		assert( *process != NULL );
		}
	else
		{
		*process = NULL;
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
								kJIgnoreConnection, NULL,
								kJTossOutput, NULL,
								kJCreatePipe, &errFD);
	if (err.OK())
		{
		*process = new JSimpleProcess(childPID, errFD, deleteWhenFinished);
		assert( *process != NULL );
		}
	else
		{
		*process = NULL;
		}

	return err;
}

JError
JSimpleProcess::Create
	(
	const JCharacter*			workingDirectory,
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
	const JCharacter*			workingDirectory,
	const JPtrArray<JString>&	argList,
	const JBoolean				deleteWhenFinished
	)
{
	pid_t childPID;
	int errFD;
	const JError err = JExecute(workingDirectory, argList, &childPID,
								kJIgnoreConnection, NULL,
								kJTossOutput, NULL,
								kJCreatePipe, &errFD);
	if (err.OK())
		{
		*process = new JSimpleProcess(childPID, errFD, deleteWhenFinished);
		assert( *process != NULL );
		}
	else
		{
		*process = NULL;
		}

	return err;
}

JError
JSimpleProcess::Create
	(
	const JCharacter*	argv[],
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
	const JCharacter*	argv[],
	const JSize			size,
	const JBoolean		deleteWhenFinished
	)
{
	pid_t childPID;
	int errFD;
	const JError err = JExecute(argv, size, &childPID,
								kJIgnoreConnection, NULL,
								kJTossOutput, NULL,
								kJCreatePipe, &errFD);
	if (err.OK())
		{
		*process = new JSimpleProcess(childPID, errFD, deleteWhenFinished);
		assert( *process != NULL );
		}
	else
		{
		*process = NULL;
		}

	return err;
}

JError
JSimpleProcess::Create
	(
	const JCharacter*	workingDirectory,
	const JCharacter*	argv[],
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
	const JCharacter*	workingDirectory,
	const JCharacter*	argv[],
	const JSize			size,
	const JBoolean		deleteWhenFinished
	)
{
	pid_t childPID;
	int errFD;
	const JError err = JExecute(workingDirectory, argv, size, &childPID,
								kJIgnoreConnection, NULL,
								kJTossOutput, NULL,
								kJCreatePipe, &errFD);
	if (err.OK())
		{
		*process = new JSimpleProcess(childPID, errFD, deleteWhenFinished);
		assert( *process != NULL );
		}
	else
		{
		*process = NULL;
		}

	return err;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSimpleProcess::~JSimpleProcess()
{
	DeleteLink();
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
			dynamic_cast(const JProcess::Finished*, &message);
		assert( info != NULL );
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
	JThisProcess::CheckACEReactor();

	JString lastLine;
	const JBoolean hasPartialLine = itsLink->PeekPartialMessage(&lastLine);

	if ((itsLink->HasMessages() || hasPartialLine) &&
		time(NULL) < itsStartTime + kMaxReportInverval)
		{
		JString text, line;
		while (itsLink->GetNextMessage(&line))
			{
			if (!text.IsEmpty())
				{
				text.AppendCharacter('\n');
				}
			text += line;
			}

		if (hasPartialLine)
			{
			if (!text.IsEmpty())
				{
				text.AppendCharacter('\n');
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

/******************************************************************************
 Constructor

 ******************************************************************************/

// This function has to be last so JCore::new works for everything else.

#undef new

JSimpleProcess::JSimpleProcess
	(
	const pid_t		pid,
	const int		fd,
	const JBoolean	deleteWhenFinished
	)
	:
	JProcess(pid),
	itsStartTime(time(NULL))
{
	itsLink = new ProcessLink(fd);
	assert( itsLink != NULL );

	ListenTo(this);

	ShouldDeleteWhenFinished(deleteWhenFinished);
}

/******************************************************************************
 DeleteLink (private)

 ******************************************************************************/

// This function has to be last so JCore::delete works for everything else.

#undef delete

void
JSimpleProcess::DeleteLink()
{
	delete itsLink;
	itsLink = NULL;
}
