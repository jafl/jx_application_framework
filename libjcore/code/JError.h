/******************************************************************************
 JError.h

	Base class for all errors

	Copyright (C) 1997-2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JError
#define _H_JError

#include "JBroadcaster.h"
#include "JString.h"

#if defined WIN32 && !defined GetMessage
#define GetMessage	GetMessageA
#endif

class JString;

extern const JUtf8Byte* kJNoError;
extern const JUtf8Byte* kJUnknownError;
extern const JUtf8Byte* kJUnexpectedError;

class JError : public JBroadcaster::Message
{
public:

	JError(const JError& source);

	JError& operator=(const JError& source);

	const JString&	GetMessage() const;
	void			ReportIfError() const;

	bool
	OK() const
	{
		return Is(kJNoError);
	}

protected:

	JError(const JUtf8Byte* type, const JUtf8Byte* msg = nullptr);

	void	SetMessage(const JString& msg);
	void	SetMessage(const JUtf8Byte* map[], const JSize size);	// JStringManager -- id is object's type

private:

	JString	itsMessage;
};


/******************************************************************************
 GetMessage

 ******************************************************************************/

inline const JString&
JError::GetMessage()
	const
{
	return itsMessage;
}

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
