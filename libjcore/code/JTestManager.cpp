/******************************************************************************
 JTestManager.cpp

	Executes functional tests.

	BASE CLASS = NONE

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "JError.h"
#include <stdio.h>
#include <unistd.h>
#include "jAssert.h"

static JTestManager* theManager = nullptr;

static JBoolean theFormatOutputFlag = kJFalse;

/******************************************************************************
 Instance

 ******************************************************************************/

JTestManager*
JTestManager::Instance()
{
	if (theManager == nullptr)
		{
		theManager = new JTestManager();
		assert( theManager != nullptr );

		theFormatOutputFlag = JI2B( isatty(fileno(stdin)) );
		}

	return theManager;
}

/******************************************************************************
 Constructor (private)

 ******************************************************************************/

JTestManager::JTestManager()
	:
	itsCurrentTestName(nullptr),
	itsTestCount(0),
	itsFailureCount(0)
{
}

/******************************************************************************
 Destructor (private)

 ******************************************************************************/

JTestManager::~JTestManager()
{
}

/******************************************************************************
 RegisterTest

 ******************************************************************************/

int
JTestManager::RegisterTest
	(
	JUnitTest			test,
	const JUtf8Byte*	name
	)
{
	assert( itsTestCount < MAX_TEST_COUNT-1 );
	itsTests[ itsTestCount ] = test;
	itsNames[ itsTestCount ] = name;
	itsTestCount++;
	return 0;
}

/******************************************************************************
 Execute (static)

	Returns value suitable for returning from main().

 ******************************************************************************/

int
JTestManager::Execute()
{
	Instance()->ExecuteTests();
	return (Instance()->itsFailureCount > 0 ? 1 : 0);
}

/******************************************************************************
 ExecuteTests (private)

 ******************************************************************************/

void
JTestManager::ExecuteTests()
{
	for (JUnsignedOffset i=0; i<itsTestCount; i++)
		{
		itsCurrentTestName = itsNames[i];
		itsTests[i]();
		}
}

/******************************************************************************
 ReportFailure (static)

 ******************************************************************************/

void
JTestManager::ReportFailure
	(
	JUtf8Byte const*	message,
	JUtf8Byte const*	file,
	const JIndex		line
	)
{
	Instance()->itsFailureCount++;

	if (theFormatOutputFlag)
		{
		std::cerr << "\033[1m";
		}

	std::cerr << file << ':' << line << ": error: ";

	if (theFormatOutputFlag)
		{
		std::cerr << "\033[22m";
		}

	std::cerr << Instance()->itsCurrentTestName << ": " << message << std::endl;
}

/******************************************************************************
 ReportFatal (static)

 ******************************************************************************/

void
JTestManager::ReportFatal
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
JTestManager::IsNull
	(
	const void*			ptr,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg
	)
{
	if (ptr == nullptr)
		{
		return kJTrue;
		}
	else
		{
		std::ostringstream s;
		if (msg != nullptr)
			{
			s << msg << ": ";
			}
		s << "Value is not null";

		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}

/******************************************************************************
 IsNotNull

 ******************************************************************************/

JBoolean
JTestManager::IsNotNull
	(
	const void*			ptr,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg
	)
{
	if (ptr != nullptr)
		{
		return kJTrue;
		}
	else
		{
		std::ostringstream s;
		if (msg != nullptr)
			{
			s << msg << ": ";
			}
		s << "Value is null";

		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}

/******************************************************************************
 IsTrue

 ******************************************************************************/

JBoolean
JTestManager::IsTrue
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
		if (msg != nullptr)
			{
			s << msg << ": ";
			}
		s << "Value is not true";

		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}

/******************************************************************************
 IsFalse

 ******************************************************************************/

JBoolean
JTestManager::IsFalse
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
		if (msg != nullptr)
			{
			s << msg << ": ";
			}
		s << "Value is not false";

		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}

/******************************************************************************
 IsOK

 ******************************************************************************/

JBoolean
JTestManager::IsOK
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
		s << "Error: " << err.GetType() << ": " << err.GetMessage();

		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}

/******************************************************************************
 StringsAreEqual

	Specialization of JAreEqual for char*

 ******************************************************************************/

JBoolean
JTestManager::StringsAreEqual
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
		if (msg != nullptr)
			{
			s << msg << ": ";
			}
		s << "Strings are not equal:" 
		  << "  Expected <<" << expectedValue
		  << ">> but got <<" << actualValue << ">>" << std::endl;
		s << '\t'; JString(expectedValue, strlen(expectedValue), kJFalse).PrintHex(s); s << std::endl;
		s << '\t'; JString(actualValue, strlen(actualValue), kJFalse).PrintHex(s); s << std::endl;

		ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}
