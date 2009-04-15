/******************************************************************************
 JError.h

	Base class for all errors

	Copyright © 1997-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JError
#define _H_JError

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBroadcaster.h>

#if defined WIN32 && !defined GetMessage
#define GetMessage	GetMessageA
#endif

class JString;

extern const JCharacter* kJNoError;
extern const JCharacter* kJUnknownError;
extern const JCharacter* kJUnexpectedError;

class JError : public JBroadcaster::Message
{
public:

	JError(const JError& source);

	virtual	~JError();

	const JError& operator=(const JError& source);

	const JCharacter*	GetMessage() const;
	void				ReportIfError() const;

	JBoolean
	OK() const
	{
		return Is(kJNoError);
	}

protected:

	JError(const JCharacter* type, const JCharacter* msg = NULL,
		   const JBoolean copyMsg = kJFalse);

	void	SetMessage(const JCharacter* msg, const JBoolean copyMsg);

	// JStringManager -- id is object's type

	void	SetMessage(const JCharacter* map[], const JSize size);

private:

	// exactly one of the following is not NULL

	const JCharacter*	itsSMessage;
	JString*			itsDMessage;
};

/******************************************************************************
 Derived classes

 ******************************************************************************/

class JNoError : public JError
{
public:

	JNoError()
		:
		JError(kJNoError)
		{ };
};

class JUnknownError : public JError
{
public:

	JUnknownError(const int err);
};

class JUnexpectedError : public JError
{
public:

	JUnexpectedError(const int err);
};

#endif
