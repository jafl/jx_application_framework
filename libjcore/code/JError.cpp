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
	passed to the constructor is NULL.

	Refer to the documentation for JRTTIBase for the recommended way to
	define type strings.

	BASE CLASS = JBroadcaster::Message

	Copyright © 1997-2000 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <JCoreStdInc.h>
#include <JError.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	If msg == NULL, it is retrieved from JStringManager.

	If copyMsg == kJTrue, we make a copy of the data in msg.
	Otherwise, we just store the pointer that is passed in.

 ******************************************************************************/

JError::JError
	(
	const JCharacter*	type,
	const JCharacter*	msg,
	const JBoolean		copyMsg
	)
	:
	JBroadcaster::Message(type),
	itsSMessage(NULL),
	itsDMessage(NULL)
{
	if (msg == NULL)
		{
		msg = JGetString(type);
		}

	if (copyMsg)
		{
		itsDMessage = new JString(msg);
		assert( itsDMessage != NULL );
		}
	else
		{
		itsSMessage = msg;
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
	itsSMessage(NULL),
	itsDMessage(NULL)
{
	if (source.itsDMessage != NULL)
		{
		itsDMessage = new JString(*(source.itsDMessage));
		assert( itsDMessage != NULL );
		}
	else
		{
		itsSMessage = source.itsSMessage;
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JError::~JError()
{
	delete itsDMessage;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const JError&
JError::operator=
	(
	const JError& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	#ifdef _MSC_VER
	Message::operator=(source);
	#else
	JBroadcaster::Message::operator=(source);
	#endif

	if (source.itsDMessage != NULL && itsDMessage != NULL)
		{
		*itsDMessage = *(source.itsDMessage);
		}
	else if (source.itsDMessage != NULL)
		{
		itsDMessage = new JString(*(source.itsDMessage));
		assert( itsDMessage != NULL );

		itsSMessage = NULL;
		}
	else
		{
		itsSMessage = source.itsSMessage;

		delete itsDMessage;
		itsDMessage = NULL;
		}

	return *this;
}

/******************************************************************************
 GetMessage

 ******************************************************************************/

const JCharacter*
JError::GetMessage()
	const
{
	return (itsSMessage != NULL ? itsSMessage : itsDMessage->GetCString());
}

/******************************************************************************
 SetMessage (protected)

	This is provided so derived classes can construct their message
	dynamically, something that cannot be done safely in the initializer list.

 ******************************************************************************/

void
JError::SetMessage
	(
	const JCharacter*	msg,
	const JBoolean		copyMsg
	)
{
	if (copyMsg && itsDMessage != NULL)
		{
		*itsDMessage = msg;
		}
	else if (copyMsg)
		{
		itsSMessage = NULL;

		itsDMessage = new JString(msg);
		assert( itsDMessage != NULL );
		}
	else
		{
		delete itsDMessage;
		itsDMessage = NULL;

		itsSMessage = msg;
		}
}

void
JError::SetMessage
	(
	const JCharacter*	map[],
	const JSize			size
	)
{
	const JString msg = JGetString(GetType(), map, size);
	SetMessage(msg, kJTrue);
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
		(JGetUserNotification())->ReportError(GetMessage());
		}
}

/******************************************************************************
 Types

 ******************************************************************************/

const JCharacter* kJNoError         = "JNoError";
const JCharacter* kJUnknownError    = "JUnknownError";
const JCharacter* kJUnexpectedError = "JUnexpectedError";

/******************************************************************************
 Derived classes

 ******************************************************************************/

static const JCharacter* kJErrorMsgMap[] =
	{
	"err", NULL
	};

JUnknownError::JUnknownError
	(
	const int err
	)
	:
	JError(kJUnknownError, "")
{
	const JString errStr(err, 0);
	kJErrorMsgMap[1] = errStr;
	SetMessage(kJErrorMsgMap, sizeof(kJErrorMsgMap));
}

JUnexpectedError::JUnexpectedError
	(
	const int err
	)
	:
	JError(kJUnexpectedError, "")
{
	const JString errStr(err, 0);
	kJErrorMsgMap[1] = errStr;
	SetMessage(kJErrorMsgMap, sizeof(kJErrorMsgMap));
}
