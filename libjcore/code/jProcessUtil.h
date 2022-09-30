/******************************************************************************
 jProcessUtil.h

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_jProcessUtil
#define _H_jProcessUtil

#include "jx-af/jcore/JPtrArray.h"
#include "jx-af/jcore/JProcessError.h"
#include <unistd.h>
#include <sys/types.h>
#include <ace/Basic_Types.h>

class JString;

// Constants for JExecute()

enum JExecuteAction
{
	kJIgnoreConnection,
	kJCreatePipe,
	kJAttachToFD,
	kJTossOutput,			// fromAction/errAction: output to /dev/null
	kJAttachToFromFD,		// errAction: connect stderr to stdout
	kJForceNonblockingPipe	// fromAction: monitor and turn off non-blocking mode (hack for svn log)
};

// Constants for JWaitForChild()

enum JChildExitReason
{
	kJChildFinished,	// you get return value from main()
	kJChildSignalled,	// you get signal that was sent
	kJChildStopped		// you get signal that was sent
};

// Prototypes

bool	JWillIncludeCWDOnPath();
void	JShouldIncludeCWDOnPath(const bool includeCWD);

bool	JProgramAvailable(const JString& programName);

JError	JExecute(const JString& cmd, pid_t* childPID,
				 const JExecuteAction toAction = kJIgnoreConnection,
				 int* toFD = nullptr,
				 const JExecuteAction fromAction = kJIgnoreConnection,
				 int* fromFD = nullptr,
				 const JExecuteAction errAction = kJIgnoreConnection,
				 int* errFD = nullptr);

JError	JExecute(const JPtrArray<JString>& argList, pid_t* childPID,
				 const JExecuteAction toAction = kJIgnoreConnection,
				 int* toFD = nullptr,
				 const JExecuteAction fromAction = kJIgnoreConnection,
				 int* fromFD = nullptr,
				 const JExecuteAction errAction = kJIgnoreConnection,
				 int* errFD = nullptr);

JError	JExecute(const JUtf8Byte* argv[], const JSize size, pid_t* childPID,
				 const JExecuteAction toAction = kJIgnoreConnection,
				 int* toFD = nullptr,
				 const JExecuteAction fromAction = kJIgnoreConnection,
				 int* fromFD = nullptr,
				 const JExecuteAction errAction = kJIgnoreConnection,
				 int* errFD = nullptr);

JError	JExecute(const JString& workingDirectory,
				 const JString& cmd, pid_t* childPID,
				 const JExecuteAction toAction = kJIgnoreConnection,
				 int* toFD = nullptr,
				 const JExecuteAction fromAction = kJIgnoreConnection,
				 int* fromFD = nullptr,
				 const JExecuteAction errAction = kJIgnoreConnection,
				 int* errFD = nullptr);

JError	JExecute(const JString& workingDirectory,
				 const JPtrArray<JString>& argList, pid_t* childPID,
				 const JExecuteAction toAction = kJIgnoreConnection,
				 int* toFD = nullptr,
				 const JExecuteAction fromAction = kJIgnoreConnection,
				 int* fromFD = nullptr,
				 const JExecuteAction errAction = kJIgnoreConnection,
				 int* errFD = nullptr);

JError	JExecute(const JString& workingDirectory,
				 const JUtf8Byte* argv[], const JSize size, pid_t* childPID,
				 const JExecuteAction toAction = kJIgnoreConnection,
				 int* toFD = nullptr,
				 const JExecuteAction fromAction = kJIgnoreConnection,
				 int* fromFD = nullptr,
				 const JExecuteAction errAction = kJIgnoreConnection,
				 int* errFD = nullptr);

	// utility functions

JString JPrepArgForExec(const JString& arg);
void	JParseArgsForExec(const JString& cmd, JPtrArray<JString>* argList);

	// wait for any child -- returns child that finished

JError JWaitForChild(const bool block, pid_t* pid, ACE_exitcode* status = nullptr);

	// wait for specified child

JError JWaitForChild(const pid_t pid, ACE_exitcode* status = nullptr);

	// evaluate "status" from JWaitForChild()

JChildExitReason	JDecodeChildExitReason(const ACE_exitcode status, int* result);
JString				JPrintChildExitReason(const JChildExitReason reason, const int result);

	// send signal

JError JSendSignalToProcess(const pid_t pid,  const int signal);
JError JSendSignalToGroup(const pid_t pgid, const int signal);

	// set priority

JError	JSetProcessPriority(const pid_t pid, const int priority);

	// process group

JError	JGetPGID(const pid_t pid, pid_t* pgid);

	// blocking convenience function to run a program

JError	JRunProgram(const JString& cmd, JString* errOutput);

#endif
