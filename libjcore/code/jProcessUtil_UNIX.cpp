/******************************************************************************
 jProcessUtil_UNIX.cpp

	Routines for creating child processes.

	Copyright (C) 1997-2005 John Lindal.

 ******************************************************************************/

#include <jProcessUtil.h>
#include <JThisProcess.h>
#include <JProcessError.h>
#include <JStdError.h>
#include <JString.h>
#include <jErrno.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <sys/resource.h>
#include <jAssert.h>

/******************************************************************************
 JRunProgram

	This convenience function runs the specified command and blocks until
	it finishes.  If the program prints anything to stderr, *errOutput
	contains the text.

	If you don't want to block, use JSimpleProcess.

 ******************************************************************************/

JError
JRunProgram
	(
	const JString&	cmd,
	JString*		errOutput
	)
{
	JProcess* p;
	int errFD;
	const JError err =
		JProcess::Create(&p, cmd,
						 kJIgnoreConnection, nullptr,
						 kJIgnoreConnection, nullptr,
						 kJCreatePipe, &errFD);
	if (err.OK())
		{
		// we let JReadAll() block because otherwise the pipe might get full

		JReadAll(errFD, errOutput);

		p->WaitUntilFinished();
		const JBoolean success = p->SuccessfulFinish();
		jdelete p;

		errOutput->TrimWhitespace();
		if (success)
			{
			return JNoError();
			}
		else
			{
			return JRunProgramError(*errOutput);
			}
		}
	else
		{
		errOutput->Clear();
		return err;
		}
}

/******************************************************************************
 JSetProcessPriority

 ******************************************************************************/

JError
JSetProcessPriority
	(
	const pid_t	pid,
	const int	priority
	)
{
	jclear_errno();
	if (setpriority(PRIO_PROCESS, pid, priority) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == ESRCH)
		{
		return JInvalidProcess();
		}
	else if (err == EPERM || err == EACCES)
		{
		return JProcessAccessDenied();
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/******************************************************************************
 JSendSignalToGroup

 ******************************************************************************/

JError
JSendSignalToGroup
	(
	const pid_t	pgid,
	const int	signal
	)
{
	jclear_errno();
	if (killpg(pgid, signal) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EINVAL)
		{
		return JInvalidSignal();
		}
	else if (err == ESRCH)
		{
		return JInvalidProcess();
		}
	else if (err == EPERM)
		{
		return JCanNotSignalProcess();
		}
	else
		{
		return JUnexpectedError(err);
		}
}

/******************************************************************************
 JGetPGID

	On FreeBSD, getpgid() isn't implemented because POSIX decided that
	it wasn't necessary since shells don't need it!  Morons!

	So we use the proc file system's 'status' file, which has the format

		name pid ppid pgid ...

	FreeBSD implementation written by Ivan Pascal <pascal@info.tsu.ru>

 ******************************************************************************/

#if defined __FreeBSD__

// This was never tested with PowerPC linux, so it remains undefined
// until somebody wants to test it!  I seriously doubt that the format
// of /proc files are the same as FreeBSD.
//
// || (defined __linux__ && defined __powerpc__)

JError
JGetPGID
	(
	const pid_t	pid,
	pid_t*		pgid
	)
{
	jclear_errno();

	JString fileName = "/proc/";
	fileName += JString((JUInt64) pid);
	fileName += "/status";

	FILE* statusFile = nullptr;
	const JError err = JFOpen(fileName, "r", &statusFile);
	if (!err.OK())
		{
		return err;
		}

	const int n = fscanf(statusFile, "%*s %*u %*u %u", pgid);
	if (n != 1)
		{
		return JUnexpectedError(jerrno());
		}

	fclose(statusFile);
	return JNoError();
}

#else

JError
JGetPGID
	(
	const pid_t	pid,
	pid_t*		pgid
	)
{
	jclear_errno();
	*pgid = getpgid(pid);
	if (*pgid != -1)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EINVAL || err == ESRCH)
		{
		return JInvalidProcess();
		}
	else if (err == EPERM)
		{
		return JProcessAccessDenied();
		}
	else
		{
		return JUnexpectedError(err);
		}
}

#endif
