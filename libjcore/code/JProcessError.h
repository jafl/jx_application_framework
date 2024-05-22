/******************************************************************************
 JProcessError.h

	Defines errors for jProcessUtil and JProcess.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JProcessError
#define _H_JProcessError

#include "JError.h"
#include "JString.h"

// Types

extern const JUtf8Byte* kJInvalidSignal;
extern const JUtf8Byte* kJInvalidProcess;
extern const JUtf8Byte* kJCanNotSignalProcess;
extern const JUtf8Byte* kJProcessAccessDenied;

extern const JUtf8Byte* kJRunProgramError;

// Classes

class JInvalidSignal : public JError
{
public:

	JInvalidSignal()
		:
		JError(kJInvalidSignal)
	{ };
};

class JInvalidProcess : public JError
{
public:

	JInvalidProcess()
		:
		JError(kJInvalidProcess)
	{ };
};

class JCanNotSignalProcess : public JError
{
public:

	JCanNotSignalProcess()
		:
		JError(kJCanNotSignalProcess)
	{ };
};

class JProcessAccessDenied : public JError
{
public:

	JProcessAccessDenied()
		:
		JError(kJProcessAccessDenied)
	{ };
};

class JRunProgramError : public JError
{
public:

	JRunProgramError(const JString& message)
		:
		JError(kJRunProgramError)
	{
		SetMessage(message);
	};
};

#endif
