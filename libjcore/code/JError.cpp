/******************************************************************************
 JError.cpp

	This class is the base class of all errors returned by JCore functions.
	Every class or utility function defines whatever special error classes
	it needs and then returns type JError.

	*** Note that derived classes cannot store -any- extra information.
		Consider the following code:

		JError foo()
	{
			return JUnknownError();
	}

		If this looks scary to you, you understand C++.  If you understand
		why this compiles, you grok C++.  What happens is that an object of
		type JUnknownError is created and then the JError copy constructor
		is called to construct a JError object from the JUnknownError object.
		This new JError object is returned and the JUnknownError object is
		deleted.  (Just another reason why most JCore objects don't have
		copy constructors.)

	Since every JError message stores a descriptive message, one often only
	needs to check if the return value is kJNoError.  If it isn't, then pass
	the object's long message to JStringManager::ReportError() or
	JUserNotification::ReportError().

	Since the type must be unique for every JError, the descriptive message
	can be stored in JStringManager.  This is automatic when the message
	passed to the constructor is nullptr.

	Refer to the documentation for JRTTIBase for the recommended way to
	define type strings.

	BASE CLASS = JBroadcaster::Message

	Copyright (C) 1997-2000 by John Lindal.

 *****************************************************************************/

#include "jx-af/jcore/JError.h"
#include "jx-af/jcore/jGlobals.h"
#include "jx-af/jcore/jAssert.h"

/******************************************************************************
 Constructor

	If msg == nullptr, it is retrieved from JStringManager.

	If copyMsg == true, we make a copy of the data in msg.
	Otherwise, we just store the pointer that is passed in.

 ******************************************************************************/

JError::JError
	(
	const JUtf8Byte*	type,
	const JUtf8Byte*	msg
	)
	:
	JBroadcaster::Message(type)
{
	if (JString::IsEmpty(msg))
	{
		itsMessage = JGetString(type);
	}
	else
	{
		itsMessage.Set(msg);
	}
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JError::JError
	(
	const JError& source
	)
	:
	JBroadcaster::Message(source),
	itsMessage(source.itsMessage)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JError::~JError()
{
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

JError&
JError::operator=
	(
	const JError& source
	)
{
	if (this == &source)
	{
		return *this;
	}

	JBroadcaster::Message::operator=(source);

	itsMessage = source.itsMessage;

	return *this;
}

/******************************************************************************
 SetMessage (protected)

	This is provided so derived classes can construct their message
	dynamically, something that cannot be done safely in the initializer list.

 ******************************************************************************/

void
JError::SetMessage
	(
	const JString& msg
	)
{
	itsMessage = msg;
}

void
JError::SetMessage
	(
	const JUtf8Byte*	map[],
	const JSize			size
	)
{
	itsMessage = JGetString(GetType(), map, size);
}

/******************************************************************************
 ReportIfError

 ******************************************************************************/

void
JError::ReportIfError()
	const
{
	if (!OK())
	{
		JGetUserNotification()->ReportError(GetMessage());
	}
}

/******************************************************************************
 Types

 ******************************************************************************/

const JUtf8Byte* kJNoError         = "JNoError";
const JUtf8Byte* kJUnknownError    = "JUnknownError";
const JUtf8Byte* kJUnexpectedError = "JUnexpectedError";

/******************************************************************************
 Derived classes

 ******************************************************************************/

JUnknownError::JUnknownError
	(
	const int err
	)
	:
	JError(kJUnknownError)
{
	const JString errStr(err, 0);

	const JUtf8Byte* map[] =
	{
		"err", errStr.GetBytes()
	};
	SetMessage(map, sizeof(map));
}

JUnexpectedError::JUnexpectedError
	(
	const int err
	)
	:
	JError(kJUnexpectedError)
{
	const JString errStr(err, 0);

	const JUtf8Byte* map[] =
	{
		"err", errStr.GetBytes()
	};
	SetMessage(map, sizeof(map));
}
