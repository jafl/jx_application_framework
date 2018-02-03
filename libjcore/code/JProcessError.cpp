/******************************************************************************
 JProcessError.cpp

	Constants for jProcessUtil and JProcess.

	Copyright (C) 1997 by John Lindal.

 *****************************************************************************/

#include <JProcessError.h>

// Types

const JUtf8Byte* kJInvalidSignal       = "JInvalidSignal";
const JUtf8Byte* kJInvalidProcess      = "JInvalidProcess";
const JUtf8Byte* kJCanNotSignalProcess = "JCanNotSignalProcess";
const JUtf8Byte* kJProcessAccessDenied = "JProcessAccessDenied";

const JUtf8Byte* kJRunProgramError     = "JRunProgramError";

// Messages

const JUtf8Byte* kJInvalidSignalMsg =
	"An invalid signal was sent to a process.";

const JUtf8Byte* kJInvalidProcessMsg =
	"A signal was sent to a non-existent process.";

const JUtf8Byte* kJCanNotSignalProcessMsg =
	"You are not allowed to send a signal to the specified process.";

const JUtf8Byte* kJProcessAccessDeniedMsg =
	"You are not allowed access to the specified process.";
