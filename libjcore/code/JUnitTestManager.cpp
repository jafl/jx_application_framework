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
 Execute (static)

	The array of tests must be NULL terminated.

 ******************************************************************************/

void
JUnitTestManager::Execute
	(
	const JUnitTest*	tests,
	const JBoolean		exit
	)
{
	int i = 0;
	while (1)
		{
		JUnitTest t = tests[i];
		if (t == NULL)
			{
			break;
			}

		t();
		i++;
		}

	if (exit)
		{
		JUnitTestManager::Exit();
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
	theManager->itsFailureCount++;

	cout << message << endl;
	cout << '\t' << "at " << file << ':' << line << endl << endl;
}

/******************************************************************************
 Exit (static)

 ******************************************************************************/

void
JUnitTestManager::Exit()
{
	exit( theManager->itsFailureCount > 0 ? 1 : 0 );
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
	else if (msg != NULL)
		{
		ReportFailure(msg, file, line);
		return kJFalse;
		}
	else
		{
		std::ostringstream s;
		s << "Value is not null: " << ptr;

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
	else if (msg != NULL)
		{
		ReportFailure(msg, file, line);
		return kJFalse;
		}
	else
		{
		std::ostringstream s;
		s << "Value is not true: " << value;

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
	else if (msg != NULL)
		{
		ReportFailure(msg, file, line);
		return kJFalse;
		}
	else
		{
		std::ostringstream s;
		s << "Value is not false: " << value;

		const std::string msg = s.str();
		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}

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
	else if (msg != NULL)
		{
		ReportFailure(msg, file, line);
		return kJFalse;
		}
	else
		{
		std::ostringstream s;
		s << "Strings are not equal." 
		  << "  Expected <<" << expectedValue
		  << ">> but got <<" << actualValue << ">>";

		const std::string msg = s.str();
		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}
