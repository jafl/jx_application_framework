/******************************************************************************
 JUnitTestManager.cpp

	Executes and unit tests.

	BASE CLASS = NONE

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <jAssert.h>

static JUnitTestManager* theManager = NULL;

/******************************************************************************
 Instance

 ******************************************************************************/

JUnitTestManager*
JUnitTestManager::Instance()
{
	if (theManager == NULL)
		{
		theManager = new JUnitTestManager();
		assert( theManager != NULL );
		}

	return theManager;
}

/******************************************************************************
 Constructor (private)

 ******************************************************************************/

JUnitTestManager::JUnitTestManager()
	:
	itsTestCount(0),
	itsFailureCount(0)
{
}

/******************************************************************************
 Destructor (private)

 ******************************************************************************/

JUnitTestManager::~JUnitTestManager()
{
}

/******************************************************************************
 RegisterTest

 ******************************************************************************/

int
JUnitTestManager::RegisterTest
	(
	JUnitTest test
	)
{
	assert( itsTestCount < MAX_TEST_COUNT-1 );
	itsTests[ itsTestCount++ ] = test;
	return 0;
}

/******************************************************************************
 Execute (static)

	Returns value suitable for returning from main().

 ******************************************************************************/

int
JUnitTestManager::Execute()
{
	Instance()->ExecuteTests();
	return (Instance()->itsFailureCount > 0 ? 1 : 0);
}

/******************************************************************************
 ExecuteTests (private)

 ******************************************************************************/

void
JUnitTestManager::ExecuteTests()
{
	for (JIndex i=0; i<itsTestCount; i++)
		{
		itsTests[i]();
		}
}

/******************************************************************************
 ReportFailure (static)

 ******************************************************************************/

void
JUnitTestManager::ReportFailure
	(
	JUtf8Byte const*	message,
	JUtf8Byte const*	file,
	const JIndex		line
	)
{
	Instance()->itsFailureCount++;

	cout << file << ':' << line << ": error: " << message << endl;
}

/******************************************************************************
 ReportFatal (static)

 ******************************************************************************/

void
JUnitTestManager::ReportFatal
	(
	JUtf8Byte const*	message,
	JUtf8Byte const*	file,
	const JIndex		line
	)
{
	ReportFailure(message, file, line);
	abort();	// since we don't throw exceptions
}

/******************************************************************************
 IsNull

 ******************************************************************************/

JBoolean
JUnitTestManager::IsNull
	(
	const void*			ptr,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg
	)
{
	if (ptr == NULL)
		{
		return kJTrue;
		}
	else
		{
		std::ostringstream s;
		if (msg != NULL)
			{
			s << msg << ": ";
			}
		s << "Value is not null";

		const std::string msg = s.str();
		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}

/******************************************************************************
 IsNotNull

 ******************************************************************************/

JBoolean
JUnitTestManager::IsNotNull
	(
	const void*			ptr,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg
	)
{
	if (ptr != NULL)
		{
		return kJTrue;
		}
	else
		{
		std::ostringstream s;
		if (msg != NULL)
			{
			s << msg << ": ";
			}
		s << "Value is null";

		const std::string msg = s.str();
		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}

/******************************************************************************
 IsTrue

 ******************************************************************************/

JBoolean
JUnitTestManager::IsTrue
	(
	const JBoolean		value,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg
	)
{
	if (value)
		{
		return kJTrue;
		}
	else
		{
		std::ostringstream s;
		if (msg != NULL)
			{
			s << msg << ": ";
			}
		s << "Value is not true";

		const std::string msg = s.str();
		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}

/******************************************************************************
 IsFalse

 ******************************************************************************/

JBoolean
JUnitTestManager::IsFalse
	(
	const JBoolean		value,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg
	)
{
	if (!value)
		{
		return kJTrue;
		}
	else
		{
		std::ostringstream s;
		if (msg != NULL)
			{
			s << msg << ": ";
			}
		s << "Value is not false";

		const std::string msg = s.str();
		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}

/******************************************************************************
 IsOK

 ******************************************************************************/
/*
JBoolean
JUnitTestManager::IsOK
	(
	const JError&		err,
	JUtf8Byte const*	file,
	const JIndex		line
	)
{
	if (err.OK())
		{
		return kJTrue;
		}
	else
		{
		std::ostringstream s;
		s << "Error: " << err.GetMessage();

		const std::string msg = s.str();
		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}
*/
/******************************************************************************
 StringsAreEqual

	Specialization of JAreEqual for char*

 ******************************************************************************/

JBoolean
JUnitTestManager::StringsAreEqual
	(
	const JUtf8Byte*	expectedValue,
	const JUtf8Byte*	actualValue,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg
	)
{
	if (strcmp(expectedValue, actualValue) == 0)
		{
		return kJTrue;
		}
	else
		{
		std::ostringstream s;
		if (msg != NULL)
			{
			s << msg << ": ";
			}
		s << "Strings are not equal:" 
		  << "  Expected <<" << expectedValue
		  << ">> but got <<" << actualValue << ">>" << endl;
		s << '\t'; JString(expectedValue, strlen(expectedValue)).PrintHex(s); s << endl;
		s << '\t'; JString(actualValue, strlen(actualValue)).PrintHex(s); s << endl;

		const std::string msg = s.str();
		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}
