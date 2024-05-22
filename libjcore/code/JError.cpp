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
	needs to check if the return value is OK().  If it isn't, then pass
	the object's long message to JStringManager::ReportError() or
	JUserNotification::ReportError().

	Since the type must be unique for every JError, the descriptive message
	can be stored in JStringManager.  This is automatic when the message
	passed to the constructor is nullptr.

	Refer to the documentation for JRTTIBase for the recommended way to
	define type strings.

	BASE CLASS = JBroadcaster::Message

	Copyright (C) 1997-2024 by John Lindal.

 *****************************************************************************/

#include "JError.h"
#include "jGlobals.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JError::JError
	(
	const JUtf8Byte* type
	)
	:
	JBroadcaster::Message(type),
	itsDynamicMessage(nullptr)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JError::~JError()
{
	jdelete itsDynamicMessage;
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
	itsDynamicMessage(source.itsDynamicMessage != nullptr ?
					  jnew JString(*source.itsDynamicMessage) : nullptr)
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

	if (itsDynamicMessage != nullptr && source.itsDynamicMessage != nullptr)
	{
		itsDynamicMessage->Set(*source.itsDynamicMessage);
	}
	else if (source.itsDynamicMessage != nullptr)
	{
		itsDynamicMessage = jnew JString(*source.itsDynamicMessage);
	}
	else if (itsDynamicMessage != nullptr)
	{
		jdelete itsDynamicMessage;
		itsDynamicMessage = nullptr;
	}

	return *this;
}

/******************************************************************************
 GetMessage

 ******************************************************************************/

const JString&
JError::GetMessage()
	const
{
	return (itsDynamicMessage != nullptr ? *itsDynamicMessage : JGetString(GetType()));
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
	jdelete itsDynamicMessage;
	itsDynamicMessage = jnew JString(msg);
}

void
JError::SetMessage
	(
	const JUtf8Byte*	map[],
	const JSize			size
	)
{
	jdelete itsDynamicMessage;
	itsDynamicMessage = jnew JString(JGetString(GetType(), map, size));
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
