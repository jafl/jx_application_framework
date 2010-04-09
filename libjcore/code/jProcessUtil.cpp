/******************************************************************************
 jProcessUtil.cpp

	Routines for creating child processes.

	Copyright © 1997 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jProcessUtil.h>
#include <JThisProcess.h>
#include <JPtrArray-JString.h>
#include <JProcessError.h>
#include <JStdError.h>
#include <jStreamUtil.h>
#include <jGlobals.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <jSignal.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jSysUtil.h>
#include <jErrno.h>
#include <jMissingProto.h>
#include <jAssert.h>

static JBoolean theIncludeCWDOnPathFlag = kJFalse;

// Private functions

void		JCleanArg(JString* arg);
JBoolean	JProgramAvailable(const JCharacter* programName, JString* fixedName);

// string ID's

static const JCharacter* kStatusSuccessID    = "StatusSuccess::jProcessUtil";
static const JCharacter* kStatusKillID       = "StatusKill::jProcessUtil";
static const JCharacter* kStatusErrorCodeID  = "StatusErrorCode::jProcessUtil";
static const JCharacter* kStatusTerminatedID = "StatusTerminated::jProcessUtil";
static const JCharacter* kStatusStoppedID    = "StatusStopped::jProcessUtil";
static const JCharacter* kStatusUnknownID    = "StatusUnknown::jProcessUtil";

/******************************************************************************
 JPrepArgForExec

	Inserts backslashes in front of double quotes and backslashes and then
	puts double quotes around the entire string.

 ******************************************************************************/

JString
JPrepArgForExec
	(
	const JCharacter* arg
	)
{
	JString str    = arg;
	JBoolean quote = kJFalse;

	const JSize length = str.GetLength();
	for (JIndex i=length; i>=1; i--)
		{
		const JCharacter c = str.GetCharacter(i);
		if (c == '"' || c == '\'' || c == '\\' || c == ';')
			{
			str.InsertSubstring("\\", i);
			}
		else if (isspace(c))
			{
			quote = kJTrue;
			}
		}

	if (quote)
		{
		str.PrependCharacter('"');
		str.AppendCharacter('"');
		}
	return str;
}

/******************************************************************************
 JParseArgsForExec

	Splits up the given string by whitespace (except that which is quoted).
	Semi-colons not enclosed by quotes are converted to separate arguments.

 ******************************************************************************/

void
JParseArgsForExec
	(
	const JCharacter*	cmd,
	JPtrArray<JString>*	argList
	)
{
	assert( !JStringEmpty(cmd) );

	argList->CleanOut();

	const JSize length = strlen(cmd);

	JIndex i = 0, j = i;
	while (i < length)
		{
		if (isspace(cmd[i]) || cmd[i] == ';')
			{
			if (j < i)
				{
				JString* s = new JString(cmd+j, i-j);
				assert( s != NULL );
				JCleanArg(s);	// clean out backslashes and quotes
				argList->Append(s);
				}
			if (cmd[i] == ';')
				{
				argList->Append(";");
				}
			i++;
			j = i;
			}
		else if (cmd[i] == '\\')
			{
			i += 2;
			}
		else if (cmd[i] == '"' || cmd[i] == '\'')
			{
			const JCharacter c = cmd[i];
			i++;
			while (i < length)
				{
				if (cmd[i] == '\\')
					{
					i++;
					}
				else if (cmd[i] == c)
					{
					i++;
					break;
					}
				i++;
				}
			}
		else
			{
			i++;
			}
		}
	i = JMin(i, length);

	// catch last argument

	if (j < i)
		{
		JString* s = new JString(cmd+j, i-j);
		assert( s != NULL );
		JCleanArg(s);	// clean out backslashes and quotes
		argList->Append(s);
		}
/*
	JSize length = str.GetLength();
	while (length > 0)
		{
		JString* arg = NULL;

		JIndex i;
		if (str.GetFirstCharacter() == '"')
			{
			i = 2;
			while (i <= length && str.GetCharacter(i) != '"')
				{
				if (str.GetCharacter(i) == '\\')
					{
					i++;
					}
				i++;
				}

			if (i <= length)
				{
				// use JIndexRange to allow empty string: ""
				arg = new JString(str.GetSubstring(JIndexRange(2, i-1)));
				assert( arg != NULL );
				str.RemoveSubstring(1,i);
				}
			else if (length > 1)
				{
				arg = new JString(str.GetSubstring(2, length));
				assert( arg != NULL );
				str.Clear();
				}
			else
				{
				str.Clear();
				}
			}
		else if (str.LocateSubstring(" ", &i))
			{
			arg = new JString(str.GetSubstring(1, i-1));
			assert( arg != NULL );
			str.RemoveSubstring(1,i);
			}
		else
			{
			arg = new JString(str);
			assert( arg != NULL );
			str.Clear();
			}

		if (arg != NULL)
			{
			JCleanArg(arg);
			argList->Append(arg);
			}

		str.TrimWhitespace();
		length = str.GetLength();
		}
*/
}

/******************************************************************************
 JCleanArg (private)

	Removes single backslashes and unbackslashed quotes.

 ******************************************************************************/

void
JCleanArg
	(
	JString* arg
	)
{
	JSize quote = 0;

	JSize length = arg->GetLength();
	for (JIndex i=1; i<=length; i++)
		{
		const JCharacter c = arg->GetCharacter(i);
		if (c == '\\')
			{
			arg->RemoveSubstring(i,i);
			length--;
			}
		else if (quote == 0 && (c == '"' || c == '\''))
			{
			arg->RemoveSubstring(i,i);
			i--;
			length--;
			quote = (c == '"' ? 2 : 1);
			}
		else if ((quote == 1 && c == '\'') ||
				 (quote == 2 && c == '"'))
			{
			arg->RemoveSubstring(i,i);
			i--;
			length--;
			quote = 0;
			}
		}
}

/******************************************************************************
 JExecute

	Convenience version that specifies a working directory.

 ******************************************************************************/

JError
JExecute
	(
	const JCharacter*		workingDirectory,
	const JCharacter*		cmd,
	pid_t*					childPID,
	const JExecuteAction	toAction,
	int*					toFD,
	const JExecuteAction	fromAction,
	int*					fromFD,
	const JExecuteAction	errAction,
	int*					errFD
	)
{
	const JString origPath = JGetCurrentDirectory();
	JError err             = JChangeDirectory(workingDirectory);
	if (err.OK())
		{
		err = JExecute(cmd, childPID, toAction, toFD, fromAction, fromFD, errAction, errFD);
		JChangeDirectory(origPath);
		}

	return err;
}

/******************************************************************************
 JExecute

	Convenience version that specifies a working directory.

 ******************************************************************************/

JError
JExecute
	(
	const JCharacter*			workingDirectory,
	const JPtrArray<JString>&	argList,
	pid_t*						childPID,
	const JExecuteAction		toAction,
	int*						toFD,
	const JExecuteAction		fromAction,
	int*						fromFD,
	const JExecuteAction		errAction,
	int*						errFD
	)
{
	const JString origPath = JGetCurrentDirectory();
	JError err             = JChangeDirectory(workingDirectory);
	if (err.OK())
		{
		err = JExecute(argList, childPID, toAction, toFD, fromAction, fromFD, errAction, errFD);
		JChangeDirectory(origPath);
		}

	return err;
}

/******************************************************************************
 JExecute

	Convenience version that specifies a working directory.

 ******************************************************************************/

JError
JExecute
	(
	const JCharacter*		workingDirectory,
	const JCharacter*		argv[],
	const JSize				size,
	pid_t*					childPID,
	const JExecuteAction	toAction,
	int*					toFD,
	const JExecuteAction	fromAction,
	int*					fromFD,
	const JExecuteAction	errAction,
	int*					errFD
	)
{
	const JString origPath = JGetCurrentDirectory();
	JError err             = JChangeDirectory(workingDirectory);
	if (err.OK())
		{
		err = JExecute(argv, size, childPID, toAction, toFD, fromAction, fromFD, errAction, errFD);
		JChangeDirectory(origPath);
		}

	return err;
}

/******************************************************************************
 JExecute

	Splits up the given string by whitespace (except that which is quoted)
	and passes the resulting array of strings to the main version of JExecute().

 ******************************************************************************/

JError
JExecute
	(
	const JCharacter*		cmd,
	pid_t*					childPID,
	const JExecuteAction	toAction,
	int*					toFD,
	const JExecuteAction	fromAction,
	int*					fromFD,
	const JExecuteAction	errAction,
	int*					errFD
	)
{
	// parse the string into a list of arguments

	JPtrArray<JString> argList(JPtrArrayT::kDeleteAll);
	JParseArgsForExec(cmd, &argList);

	// pass the arguments to JExecute

	return JExecute(argList, childPID,
					toAction, toFD, fromAction, fromFD,
					errAction, errFD);
}

/******************************************************************************
 JExecute

	Convenience version that calls JExecute(char**).

 ******************************************************************************/

JError
JExecute
	(
	const JPtrArray<JString>&	argList,
	pid_t*						childPID,
	const JExecuteAction		toAction,
	int*						toFD,
	const JExecuteAction		fromAction,
	int*						fromFD,
	const JExecuteAction		errAction,
	int*						errFD
	)
{
	const JSize argc = argList.GetElementCount();

	const JCharacter** argv = new const JCharacter* [ argc+1 ];
	assert( argv != NULL );

	for (JIndex i=1; i<=argc; i++)
		{
		argv[i-1] = *(argList.NthElement(i));
		}
	argv[argc] = NULL;

	const JError err = JExecute(argv, (argc+1) * sizeof(JCharacter*), childPID,
								toAction, toFD, fromAction, fromFD,
								errAction, errFD);

	delete [] argv;
	return err;
}

/******************************************************************************
 JExecute

	argv[0] can either be a full path or just a name.  If it is just a
	name, we search for it just like the shell would.

	argv[] must be terminated with a NULL entry.

	size must be sizeof(argv[]).  JExecute automatically divides by
	sizeof(char*).  We required this so we can check that argv is NULL
	terminated.  It is way too easy to forget to do this otherwise.

	If childPID != NULL, it is set to the pid of the child process.
	Otherwise, JExecute() blocks until the child exits.

	Actions:

		kJIgnoreConnection  don't create a pipe, use default connection

		kJCreatePipe        create a pipe and return the end that the
		                    parent should use

		kJAttachToFD        connect the child to the descriptor passed in
		                    via the corresponding int* (don't create pipe)

		kJTossOutput        connects output to /dev/null
		                    (only works for fromAction and errAction)

		kJAttachToFromFD    connect stderr to stdout
		                    (only works for errAction)

		kJForceNonblockingPipe	same as kJCreatePipe, but monitors and turns
								off non-blocking mode
								(only works for fromAction)

	For kJAttachToFD, toFD has to be something that can be read from, and
	fromFD and errFD have to be something that can be written to.

	Can return JProgramNotAvailable, JNoProcessMemory, JNoKernelMemory.

	*** Security Note:
		This function calls execvp(), which uses the current search path to
		find the program to run.  In most cases, this is desirable because
		each UNIX system has its own ideas about where to put "standard"
		programs.  For suid root programs, however, this is dangerous.
		Therefore, when writing suid root programs, always call this function
		with a full path so you know exactly which binary is being run.

 ******************************************************************************/

JError
JExecute
	(
	const JCharacter*		argv[],
	const JSize				size,
	pid_t*					childPID,
	const JExecuteAction	toAction,
	int*					toFD,
	const JExecuteAction	origFromAction,
	int*					fromFD,
	const JExecuteAction	errAction,
	int*					errFD
	)
{
	assert( size > sizeof(JCharacter*) );
	assert( argv[ (size/sizeof(JCharacter*)) - 1 ] == NULL );

	const JExecuteAction fromAction =
		(origFromAction == kJForceNonblockingPipe ? kJCreatePipe : origFromAction);

	assert( toAction != kJTossOutput && toAction != kJAttachToFromFD &&
			toAction != kJForceNonblockingPipe );
	assert( fromAction != kJAttachToFromFD );
	assert( errAction != kJForceNonblockingPipe );

	assert( (toAction != kJCreatePipe && toAction != kJAttachToFD) ||
			toFD != NULL );
	assert( (fromAction != kJCreatePipe && fromAction != kJAttachToFD) ||
			fromFD != NULL );
	assert( (errAction != kJCreatePipe && errAction != kJAttachToFD) ||
			errFD != NULL );

	JString progName;
	if (!JProgramAvailable(argv[0], &progName))
		{
		return JProgramNotAvailable(argv[0]);
		}
	argv[0] = progName.GetCString();

	int fd[3][2];

	if (toAction == kJCreatePipe)
		{
		const JError err = JCreatePipe(fd[0]);
		if (!err.OK())
			{
			return err;
			}
		}

	if (fromAction == kJCreatePipe)
		{
		const JError err = JCreatePipe(fd[1]);
		if (!err.OK())
			{
			if (toAction == kJCreatePipe)
				{
				close(fd[0][0]);
				close(fd[0][1]);
				}
			return err;
			}
		}

	if (errAction == kJCreatePipe)
		{
		const JError err = JCreatePipe(fd[2]);
		if (!err.OK())
			{
			if (toAction == kJCreatePipe)
				{
				close(fd[0][0]);
				close(fd[0][1]);
				}
			if (fromAction == kJCreatePipe)
				{
				close(fd[1][0]);
				close(fd[1][1]);
				}
			return err;
			}
		}

	pid_t pid;
	const JError err = JThisProcess::Fork(&pid);
	if (!err.OK())
		{
		if (toAction == kJCreatePipe)
			{
			close(fd[0][0]);
			close(fd[0][1]);
			}
		if (fromAction == kJCreatePipe)
			{
			close(fd[1][0]);
			close(fd[1][1]);
			}
		if (errAction == kJCreatePipe)
			{
			close(fd[2][0]);
			close(fd[2][1]);
			}
		return err;
		}

	// child

	else if (pid == 0)
		{
		const int stdinFD = fileno(stdin);
		if (toAction == kJCreatePipe)
			{
			dup2(fd[0][0], stdinFD);
			close(fd[0][0]);
			close(fd[0][1]);
			}
		else if (toAction == kJAttachToFD)
			{
			dup2(*toFD, stdinFD);
			close(*toFD);
			}

		const int stdoutFD = fileno(stdout);
		if (fromAction == kJCreatePipe)
			{
			dup2(fd[1][1], stdoutFD);
			close(fd[1][0]);
			close(fd[1][1]);
			}
		else if (fromAction == kJAttachToFD)
			{
			dup2(*fromFD, stdoutFD);
			close(*fromFD);
			}
		else if (fromAction == kJTossOutput)
			{
			FILE* nullFile = fopen("/dev/null", "a");
			int nullfd     = fileno(nullFile);
			dup2(nullfd, stdoutFD);
			fclose(nullFile);
			}

		const int stderrFD = fileno(stderr);
		if (errAction == kJCreatePipe)
			{
			dup2(fd[2][1], stderrFD);
			close(fd[2][0]);
			close(fd[2][1]);
			}
		else if (errAction == kJAttachToFD)
			{
			dup2(*errFD, stderrFD);
			close(*errFD);
			}
		else if (errAction == kJTossOutput)
			{
			FILE* nullFile = fopen("/dev/null", "a");
			int nullfd     = fileno(nullFile);
			dup2(nullfd, stderrFD);
			fclose(nullFile);
			}
		else if (errAction == kJAttachToFromFD && fromAction != kJIgnoreConnection)
			{
			dup2(stdoutFD, stderrFD);
			}

		ACE_OS::execvp(argv[0], const_cast<char* const*>(argv));

		cerr << "Unable to run program \"" << argv[0] << '"' << endl;
		cerr << endl;
		cerr << "JExecute()::execvp() failed" << endl;
		cerr << "Errno value: " << jerrno() << endl;

		JThisProcess::Exit(1);
		return JNoError();
		}

	// parent

	else
		{
		if (origFromAction == kJForceNonblockingPipe)
			{
			pid_t pid2;
			const JError err2 = JThisProcess::Fork(&pid2);
			if (err2.OK() && pid2 == 0)
				{
				for (int i=0; i<150; i++)
					{
					JWait(0.1);

					int value = fcntl(fd[1][1], F_GETFL, 0);
					if (value & O_NONBLOCK)
						{
						cerr << "turning off nonblocking for cout: " << value << endl;
						fcntl(fd[1][1], F_SETFL, value & (~ O_NONBLOCK));
						}
					}

				JThisProcess::Exit(0);
				return JNoError();
				}

			JProcess* p = new JProcess(pid2);
			p->KillAtExit(kJTrue);
			}

		if (toAction == kJCreatePipe)
			{
			close(fd[0][0]);
			*toFD = fd[0][1];
			}
		if (fromAction == kJCreatePipe)
			{
			close(fd[1][1]);
			*fromFD = fd[1][0];
			}
		if (errAction == kJCreatePipe)
			{
			close(fd[2][1]);
			*errFD = fd[2][0];
			}

		if (childPID == NULL)
			{
			return JWaitForChild(pid);
			}
		else
			{
			*childPID = pid;
			return JNoError();
			}
		}
}

/******************************************************************************
 JWaitForChild

	Wait until a child process finishes.  If !block and no child has
	finished, *pid=0.

	status can be NULL.

 ******************************************************************************/

JError
JWaitForChild
	(
	const JBoolean	block,
	pid_t*			pid,
	ACE_exitcode*	status
	)
{
	int err;
	do
		{
		jclear_errno();
		*pid = ACE_OS::waitpid(-1, status, (block ? 0 : WNOHANG));
		err = jerrno();
		}
		while (err == EINTR);

	return JNoError();
}

JError
JWaitForChild
	(
	const pid_t		pid,
	ACE_exitcode*	status
	)
{
	int err;
	do
		{
		jclear_errno();
		ACE_OS::waitpid(pid, status, 0);
		err = jerrno();
		}
		while (err == EINTR);

	return JNoError();
}

/******************************************************************************
 JDecodeChildExitReason

	Returns one of:

		kJChildFinished  -- *result contains return value from main()
		kJChildSignalled -- *result contains signal that was sent
		kJChildStopped   -- *result contains signal that was sent

 ******************************************************************************/

JChildExitReason
JDecodeChildExitReason
	(
	const ACE_exitcode	status,
	int*				result
	)
{
	if (WIFEXITED(status))
		{
		*result = WEXITSTATUS(status);
		return kJChildFinished;
		}
	else if (WIFSIGNALED(status))
		{
		*result = WTERMSIG(status);
		return kJChildSignalled;
		}
	else if (WIFSTOPPED(status))
		{
		*result = WSTOPSIG(status);
		return kJChildStopped;
		}

	*result = 0;
	return kJChildFinished;
}

/******************************************************************************
 JPrintChildExitReason

 ******************************************************************************/

JString
JPrintChildExitReason
	(
	const JChildExitReason	reason,
	const int				result
	)
{
	if (reason == kJChildFinished && result == 0)
		{
		return JGetString(kStatusSuccessID);
		}
	else if (reason == kJChildSignalled && result == SIGKILL)
		{
		return JGetString(kStatusKillID);
		}
	else if (reason == kJChildFinished)
		{
		const JString errValue(result, JString::kBase10);
		const JCharacter* map[] =
			{
			"code", errValue
			};
		return JGetString(kStatusErrorCodeID, map, sizeof(map));
		}
	else if (reason == kJChildSignalled)
		{
		const JString sigName = JGetSignalName(result);
		const JCharacter* map[] =
			{
			"signal", sigName
			};
		return JGetString(kStatusTerminatedID, map, sizeof(map));
		}
	else if (reason == kJChildStopped)
		{
		const JString sigName = JGetSignalName(result);
		const JCharacter* map[] =
			{
			"signal", sigName
			};
		return JGetString(kStatusStoppedID, map, sizeof(map));
		}
	else
		{
		return JGetString(kStatusUnknownID);
		}
}

/******************************************************************************
 JSendSignalToProcess

 ******************************************************************************/

JError
JSendSignalToProcess
	(
	const pid_t	pid,
	const int	signal
	)
{
	jclear_errno();
	if (ACE_OS::kill(pid, signal) == 0)
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
 JProgramAvailable

	Returns kJTrue if the given program can be run by JExecute().

 ******************************************************************************/

JBoolean
JProgramAvailable
	(
	const JCharacter* programName
	)
{
	JString s;
	return JProgramAvailable(programName, &s);
}

JBoolean
JProgramAvailable
	(
	const JCharacter*	programName,
	JString*			fixedName
	)
{
	if (JStringEmpty(programName) ||
		!JExpandHomeDirShortcut(programName, fixedName))
		{
		return kJFalse;
		}

	if (fixedName->GetFirstCharacter() == '/')
		{
		return JFileExecutable(*fixedName);
		}

	JString fullName = programName;
	if (fullName.Contains("/"))
		{
		const JString dir = JGetCurrentDirectory();
		fullName          = JCombinePathAndName(dir, fullName);
		return JFileExecutable(fullName);
		}

	// check each directory in the exec path list

	const JCharacter* cpath = getenv("PATH");

	JString path(cpath == NULL ? "" : cpath);
	if (theIncludeCWDOnPathFlag)
		{
		path.Prepend(".:");
		}

	if (path.IsEmpty())
		{
		return kJFalse;
		}

	JIndex colonIndex;
	while (path.LocateSubstring(":", &colonIndex))
		{
		if (colonIndex > 1)
			{
			const JString dir = path.GetSubstring(1, colonIndex-1);
			fullName          = JCombinePathAndName(dir, programName);
			if (JFileExists(fullName) && JFileExecutable(fullName))
				{
				if (dir == ".")
					{
					fixedName->Prepend("./");	// in case we added this to PATH
					}
				return kJTrue;
				}
			}

		path.RemoveSubstring(1, colonIndex);
		}

	if (path.IsEmpty())
		{
		return kJFalse;
		}

	fullName = JCombinePathAndName(path, programName);
	return JFileExecutable(fullName);
}

/******************************************************************************
 Include CWD on Path

	Turn this option on to automatically include ./ on the execution path.

 ******************************************************************************/

JBoolean
JWillIncludeCWDOnPath()
{
	return theIncludeCWDOnPathFlag;
}

void
JShouldIncludeCWDOnPath
	(
	const JBoolean includeCWD
	)
{
	theIncludeCWDOnPathFlag = includeCWD;
}
