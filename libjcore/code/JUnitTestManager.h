/******************************************************************************
 JUnitTestManager.h

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JUnitTestManager
#define _H_JUnitTestManager

#include <JString.h>
#include <sstream>
#include <math.h>

class JUnitTestManager;

typedef void	(*JUnitTest)();

#define MAX_TEST_COUNT 1000

#define JTEST(f)	void f(); static int unused_##f = JUnitTestManager::Instance()->RegisterTest(f); void f()

#define JAssertNull(ptr)	JUnitTestManager::Instance()->IsNull(ptr, __FILE__, __LINE__)
#define JAssertTrue(value)	JUnitTestManager::Instance()->IsTrue(value, __FILE__, __LINE__)
#define JAssertFalse(value)	JUnitTestManager::Instance()->IsFalse(value, __FILE__, __LINE__)

#define JAssertNullWithMessage(ptr, msg)	JUnitTestManager::Instance()->IsNull(ptr, __FILE__, __LINE__, msg)
#define JAssertTrueWithMessage(value, msg)	JUnitTestManager::Instance()->IsTrue(value, __FILE__, __LINE__, msg)
#define JAssertFalseWithMessage(value, msg)	JUnitTestManager::Instance()->IsFalse(value, __FILE__, __LINE__, msg)

#define JAssertEqual(expected, actual) \
	JAreEqual(expected, actual, __FILE__, __LINE__)

#define JAssertEqualWithMessage(expected, actual, msg) \
	JAreEqual(expected, actual, __FILE__, __LINE__, msg)

#define JAssertStringsEqual(expected, actual) \
	JUnitTestManager::Instance()->StringsAreEqual(expected, actual, __FILE__, __LINE__)

#define JAssertStringsEqualWithMessage(expected, actual, msg) \
	JUnitTestManager::Instance()->StringsAreEqual(expected, actual, __FILE__, __LINE__, msg)

#define JAssertWithin(epsilon, expected, actual) \
	JAreWithin(epsilon, expected, actual, __FILE__, __LINE__)

#define JAssertWithinWithMessage(epsilon, expected, actual, msg) \
	JAreWithin(epsilon, expected, actual, __FILE__, __LINE__, msg)

class JUnitTestManager
{
public:

	static JUnitTestManager* Instance();

	static int	Execute();
	static void	ReportFailure(JUtf8Byte const* message, JUtf8Byte const* file, const JIndex line);
	static void	ReportFatal(JUtf8Byte const* message, JUtf8Byte const* file, const JIndex line);

	int	RegisterTest(JUnitTest name);

	JBoolean	IsNull(const void* ptr, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsTrue(const JBoolean value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsTrue(const int value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsFalse(const JBoolean value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsFalse(const int value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);

	JBoolean	StringsAreEqual(const JString& expectedValue, const JString& actualValue,
								JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	StringsAreEqual(const JString& expectedValue, const JUtf8Byte* actualValue,
								JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	StringsAreEqual(const JUtf8Byte* expectedValue, const JString& actualValue,
								JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	StringsAreEqual(const JUtf8Byte* expectedValue, const JUtf8Byte* actualValue,
								JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);

private:

	JUnitTest	itsTests[ MAX_TEST_COUNT ];
	JSize		itsTestCount;
	JSize		itsFailureCount;

private:

	JUnitTestManager();

	~JUnitTestManager();

	void	ExecuteTests();

	// not allowed

	JUnitTestManager(const JUnitTestManager& source);
	const JUnitTestManager& operator=(const JUnitTestManager& source);
};

template <class A, class B>
JBoolean
JAreEqual
	(
	const A&			expectedValue,
	const B&			actualValue,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg = NULL
	)
{
	if (expectedValue == actualValue)
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
		s << "Values are not equal:"
		  << "  Expected " << expectedValue
		  << " but got " << actualValue;

		const std::string msg = s.str();
		JUnitTestManager::Instance()->ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}

template <class T>
JBoolean
JAreWithin
	(
	const T&			epsilon,
	const T&			expectedValue,
	const T&			actualValue,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg = NULL
	)
{
	if (fabs(expectedValue - actualValue) < epsilon)
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
		s << "Values are not within " << epsilon << ":"
		  << "  Expected " << expectedValue
		  << " but got " << actualValue;

		const std::string msg = s.str();
		JUnitTestManager::Instance()->ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}

inline JBoolean
JUnitTestManager::IsTrue
	(
	const int			value,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg
	)
{
	return IsTrue(JI2B(value), file, line, msg);
}

inline JBoolean
JUnitTestManager::IsFalse
	(
	const int			value,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg
	)
{
	return IsFalse(JI2B(value), file, line, msg);
}

inline JBoolean
JUnitTestManager::StringsAreEqual
	(
	const JString&		expectedValue,
	const JString&		actualValue,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg
	)
{
	return StringsAreEqual(expectedValue.GetBytes(), actualValue.GetBytes(), file, line, msg);
}

inline JBoolean
JUnitTestManager::StringsAreEqual
	(
	const JString&		expectedValue,
	const JUtf8Byte*	actualValue,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg
	)
{
	return StringsAreEqual(expectedValue.GetBytes(), actualValue, file, line, msg);
}

inline JBoolean
JUnitTestManager::StringsAreEqual
	(
	const JUtf8Byte*	expectedValue,
	const JString&		actualValue,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg
	)
{
	return StringsAreEqual(expectedValue, actualValue.GetBytes(), file, line, msg);
}

#endif
