/******************************************************************************
 JProcess.cpp

	Class to represent a UNIX process.

	Event loops can call CheckForFinishedChild(false) in order to
	receive JBroadcaster messages when child processes finish.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1997 by John Lindal.

 *****************************************************************************/

#include "JThisProcess.h"
#include "jAssert.h"

// JBroadcaster message types

const JUtf8Byte* JProcess::kFinished = "Finished::JProcess";

// static data

JPtrArray<JProcess>	JProcess::theProcessList(JPtrArrayT::kForgetAll);

/******************************************************************************
 Create (static)

	Refer to JExecute() for documentation.

 ******************************************************************************/

JError
JProcess::Create
	(
	JProcess**				process,
	const JString&			cmdStr,
	const JExecuteAction	toAction,
	int*					toFD,
	const JExecuteAction	fromAction,
	int*					fromFD,
	const JExecuteAction	errAction,
	int*					errFD
	)
{
	pid_t childPID;
	const JError err = JExecute(cmdStr, &childPID, toAction, toFD,
								fromAction, fromFD, errAction, errFD);
	if (err.OK())
	{
		*process = jnew JProcess(childPID);
	}
	else
	{
		*process = nullptr;
	}

	return err;
}

JError
JProcess::Create
	(
	JProcess**					process,
	const JPtrArray<JString>&	argList,
	const JExecuteAction		toAction,
	int*						toFD,
	const JExecuteAction		fromAction,
	int*						fromFD,
	const JExecuteAction		errAction,
	int*						errFD
	)
{
	pid_t childPID;
	const JError err = JExecute(argList, &childPID, toAction, toFD,
								fromAction, fromFD, errAction, errFD);
	if (err.OK())
	{
		*process = jnew JProcess(childPID);
	}
	else
	{
		*process = nullptr;
	}

	return err;
}

JError
JProcess::Create
	(
	JProcess**				process,
	const JUtf8Byte*		argv[],
	const JSize				size,
	const JExecuteAction	toAction,
	int*					toFD,
	const JExecuteAction	fromAction,
	int*					fromFD,
	const JExecuteAction	errAction,
	int*					errFD
	)
{
	pid_t childPID;
	const JError err = JExecute(argv, size, &childPID, toAction, toFD,
								fromAction, fromFD, errAction, errFD);
	if (err.OK())
	{
		*process = jnew JProcess(childPID);
	}
	else
	{
		*process = nullptr;
	}

	return err;
}

JError
JProcess::Create
	(
	JProcess**				process,
	const JString&			workingDirectory,
	const JString&			cmdStr,
	const JExecuteAction	toAction,
	int*					toFD,
	const JExecuteAction	fromAction,
	int*					fromFD,
	const JExecuteAction	errAction,
	int*					errFD
	)
{
	pid_t childPID;
	const JError err = JExecute(workingDirectory, cmdStr, &childPID, toAction, toFD,
								fromAction, fromFD, errAction, errFD);
	if (err.OK())
	{
		*process = jnew JProcess(childPID);
	}
	else
	{
		*process = nullptr;
	}

	return err;
}

JError
JProcess::Create
	(
	JProcess**					process,
	const JString&				workingDirectory,
	const JPtrArray<JString>&	argList,
	const JExecuteAction		toAction,
	int*						toFD,
	const JExecuteAction		fromAction,
	int*						fromFD,
	const JExecuteAction		errAction,
	int*						errFD
	)
{
	pid_t childPID;
	const JError err = JExecute(workingDirectory, argList, &childPID, toAction, toFD,
								fromAction, fromFD, errAction, errFD);
	if (err.OK())
	{
		*process = jnew JProcess(childPID);
	}
	else
	{
		*process = nullptr;
	}

	return err;
}

JError
JProcess::Create
	(
	JProcess**				process,
	const JString&			workingDirectory,
	const JUtf8Byte*		argv[],
	const JSize				size,
	const JExecuteAction	toAction,
	int*					toFD,
	const JExecuteAction	fromAction,
	int*					fromFD,
	const JExecuteAction	errAction,
	int*					errFD
	)
{
	pid_t childPID;
	const JError err = JExecute(workingDirectory, argv, size, &childPID, toAction, toFD,
								fromAction, fromFD, errAction, errFD);
	if (err.OK())
	{
		*process = jnew JProcess(childPID);
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

JProcess::JProcess
	(
	const pid_t pid
	)
	:
	itsPID(pid),
	itsIsFinishedFlag(false),
	itsFinishedStatus(0),
	itsAutoDeleteFlag(false)
{
	theProcessList.SetCompareFunction(ComparePID);
	theProcessList.InsertSorted(this, true);

	JThisProcess::QuitAtExit(this, true);
}

// private -- used only by CheckForFinishedChild()
//			  to avoid adding temporary object to theProcessList

JProcess::JProcess
	(
	const pid_t	pid,
	const int	x
	)
	:
	itsPID(pid)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JProcess::~JProcess()
{
	theProcessList.Remove(this);
	JThisProcess::QuitAtExit(this, false);
	JThisProcess::KillAtExit(this, false);
}

/******************************************************************************
 Quit/Kill at exit

 ******************************************************************************/

bool
JProcess::WillQuitAtExit()
	const
{
	return JThisProcess::WillQuitAtExit(this);
}

void
JProcess::QuitAtExit
	(
	const bool quit
	)
{
	JThisProcess::QuitAtExit(this, quit);
}

bool
JProcess::WillKillAtExit()
	const
{
	return JThisProcess::WillKillAtExit(this);
}

void
JProcess::KillAtExit
	(
	const bool kill
	)
{
	JThisProcess::KillAtExit(this, kill);
}

/******************************************************************************
 SendSignalToGroup

 ******************************************************************************/

JError
JProcess::SendSignalToGroup
	(
	const int signal
	)
{
	pid_t pgid;
	const JError err = GetPGID(&pgid);
	if (err.OK())
	{
		return JSendSignalToGroup(pgid, signal);
	}
	else
	{
		return err;
	}
}

/******************************************************************************
 WaitUntilFinished

	Block until the child finishes.

 ******************************************************************************/

void
JProcess::WaitUntilFinished()
{
	ACE_exitcode status;
	const JError err = JWaitForChild(itsPID, &status);
	if (err.OK())
	{
		itsIsFinishedFlag = true;
		itsFinishedStatus = status;

		const bool autoDelete = itsAutoDeleteFlag;	// save since Broadcast() might delete it -- in which case, the flag must be false!
		Broadcast(Finished(status));
		if (autoDelete)
		{
			jdelete this;
		}
	}
}

/******************************************************************************
 CheckForFinishedChild (static)

	Checks if a child has finished.  If one has, an a JProcess object
	exists for it, then tell it to Broadcast().

 ******************************************************************************/

void
JProcess::CheckForFinishedChild
	(
	const bool block
	)
{
	pid_t pid;
	ACE_exitcode status;
	const JError err = JWaitForChild(block, &pid, &status);
	if (err.OK() && pid > 0)
	{
		theProcessList.SetCompareFunction(ComparePID);

		JProcess target(pid, 0);
		JIndex i;
		if (theProcessList.SearchSorted(&target, JListT::kFirstMatch, &i))
		{
			// Broadcast message from each JProcess object with the
			// given pid.  There could be more than one!

			// Since theProcessList may be changed by code in some Receive(),
			// we first collect all the objects that need to broadcast,
			// and then we tell each one to broadcast.

			JPtrArray<JProcess> list(JPtrArrayT::kForgetAll);
			const JSize processCount = theProcessList.GetItemCount();
			while (i <= processCount)
			{
				JProcess* p = theProcessList.GetItem(i);
				if (p->GetPID() != pid)
				{
					break;
				}

				list.Append(p);
				i++;
			}

			for (auto* p : list)
			{
				const bool autoDelete = p->itsAutoDeleteFlag;	// save since Broadcast() might delete it -- in which case, the flag must be false!
				p->itsIsFinishedFlag  = true;
				p->itsFinishedStatus  = status;
				p->Broadcast(Finished(status));
				if (autoDelete)
				{
					jdelete p;
				}
			}
		}
	}
}

/******************************************************************************
 ComparePID (static private)

 ******************************************************************************/

std::weak_ordering
JProcess::ComparePID
	(
	JProcess* const & p1,
	JProcess* const & p2
	)
{
	return p1->GetPID() <=> p2->GetPID();
}
