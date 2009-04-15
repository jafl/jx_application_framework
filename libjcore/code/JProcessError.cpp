/******************************************************************************
 JProcessError.cpp

	Constants for jProcessUtil and JProcess.

	Copyright © 1997 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <JCoreStdInc.h>
#include <JProcessError.h>

// Types

const JCharacter* kJInvalidSignal       = "JInvalidSignal";
const JCharacter* kJInvalidProcess      = "JInvalidProcess";
const JCharacter* kJCanNotSignalProcess = "JCanNotSignalProcess";
const JCharacter* kJProcessAccessDenied = "JProcessAccessDenied";

const JCharacter* kJRunProgramError     = "JRunProgramError";

// Messages

const JCharacter* kJInvalidSignalMsg =
	"An invalid signal was sent to a process.";

const JCharacter* kJInvalidProcessMsg =
	"A signal was sent to a non-existent process.";

const JCharacter* kJCanNotSignalProcessMsg =
	"You are not allowed to send a signal to the specified process.";

const JCharacter* kJProcessAccessDeniedMsg =
	"You are not allowed access to the specified process.";
