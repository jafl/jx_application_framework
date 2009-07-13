/******************************************************************************
 jProcessUtil.h

	Interface for jProcessUtil.cc

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jProcessUtil
#define _H_jProcessUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray.h>
#include <JProcessError.h>
#include <unistd.h>
#include <sys/types.h>
#include <ace/OS.h>

class JString;

// Constants for JExecute()

enum JExecuteAction
{
	kJIgnoreConnection,
	kJCreatePipe,
	kJAttachToFD,
	kJTossOutput,		// output to /dev/null, fromAction and errAction only
	kJAttachToFromFD	// connect stderr to stdout, errAction only
};

// Constants for JWaitForChild()

enum JChildExitReason
{
	kJChildFinished,	// you get return value from main()
	kJChildSignalled,	// you get signal that was sent
	kJChildStopped		// you get signal that was sent
};

// Prototypes

JBoolean	JWillIncludeCWDOnPath();
void		JShouldIncludeCWDOnPath(const JBoolean includeCWD);

JBoolean	JProgramAvailable(const JCharacter* programName);

JError	JExecute(const JCharacter* cmd, pid_t* childPID,
				 const JExecuteAction toAction = kJIgnoreConnection,
				 int* toFD = NULL,
				 const JExecuteAction fromAction = kJIgnoreConnection,
				 int* fromFD = NULL,
				 const JExecuteAction errAction = kJIgnoreConnection,
				 int* errFD = NULL);

JError	JExecute(const JPtrArray<JString>& argList, pid_t* childPID,
				 const JExecuteAction toAction = kJIgnoreConnection,
				 int* toFD = NULL,
				 const JExecuteAction fromAction = kJIgnoreConnection,
				 int* fromFD = NULL,
				 const JExecuteAction errAction = kJIgnoreConnection,
				 int* errFD = NULL);

JError	JExecute(const JCharacter* argv[], const JSize size, pid_t* childPID,
				 const JExecuteAction toAction = kJIgnoreConnection,
				 int* toFD = NULL,
				 const JExecuteAction fromAction = kJIgnoreConnection,
				 int* fromFD = NULL,
				 const JExecuteAction errAction = kJIgnoreConnection,
				 int* errFD = NULL);

JError	JExecute(const JCharacter* workingDirectory,
				 const JCharacter* cmd, pid_t* childPID,
				 const JExecuteAction toAction = kJIgnoreConnection,
				 int* toFD = NULL,
				 const JExecuteAction fromAction = kJIgnoreConnection,
				 int* fromFD = NULL,
				 const JExecuteAction errAction = kJIgnoreConnection,
				 int* errFD = NULL);

JError	JExecute(const JCharacter* workingDirectory,
				 const JPtrArray<JString>& argList, pid_t* childPID,
				 const JExecuteAction toAction = kJIgnoreConnection,
				 int* toFD = NULL,
				 const JExecuteAction fromAction = kJIgnoreConnection,
				 int* fromFD = NULL,
				 const JExecuteAction errAction = kJIgnoreConnection,
				 int* errFD = NULL);

JError	JExecute(const JCharacter* workingDirectory,
				 const JCharacter* argv[], const JSize size, pid_t* childPID,
				 const JExecuteAction toAction = kJIgnoreConnection,
				 int* toFD = NULL,
				 const JExecuteAction fromAction = kJIgnoreConnection,
				 int* fromFD = NULL,
				 const JExecuteAction errAction = kJIgnoreConnection,
				 int* errFD = NULL);

	// utility functions

JString JPrepArgForExec(const JCharacter* arg);
void	JParseArgsForExec(const JCharacter* cmd, JPtrArray<JString>* argList);

	// wait for any child -- returns child that finished

JError JWaitForChild(const JBoolean block, pid_t* pid, ACE_exitcode* status = NULL);

	// wait for specified child

JError JWaitForChild(const pid_t pid, ACE_exitcode* status = NULL);

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

JError	JRunProgram(const JCharacter* cmd, JString* errOutput);

#endif
