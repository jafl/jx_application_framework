/******************************************************************************
 JProcessError.h

	Defines errors for jProcessUtil and JProcess.

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JProcessError
#define _H_JProcessError

#include <JError.h>

// Types

extern const JCharacter* kJInvalidSignal;
extern const JCharacter* kJInvalidProcess;
extern const JCharacter* kJCanNotSignalProcess;
extern const JCharacter* kJProcessAccessDenied;

extern const JCharacter* kJRunProgramError;

// Messages

extern const JCharacter* kJInvalidSignalMsg;
extern const JCharacter* kJInvalidProcessMsg;
extern const JCharacter* kJCanNotSignalProcessMsg;
extern const JCharacter* kJProcessAccessDeniedMsg;

// Classes

class JInvalidSignal : public JError
{
public:

	JInvalidSignal()
		:
		JError(kJInvalidSignal, kJInvalidSignalMsg)
		{ };
};

class JInvalidProcess : public JError
{
public:

	JInvalidProcess()
		:
		JError(kJInvalidProcess, kJInvalidProcessMsg)
		{ };
};

class JCanNotSignalProcess : public JError
{
public:

	JCanNotSignalProcess()
		:
		JError(kJCanNotSignalProcess, kJCanNotSignalProcessMsg)
		{ };
};

class JProcessAccessDenied : public JError
{
public:

	JProcessAccessDenied()
		:
		JError(kJProcessAccessDenied, kJProcessAccessDeniedMsg)
		{ };
};

class JRunProgramError : public JError
{
public:

	JRunProgramError(const JCharacter* message)
		:
		JError(kJRunProgramError, message, kJTrue)
		{ };
};

#endif
