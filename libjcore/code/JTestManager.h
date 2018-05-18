/******************************************************************************
 JTestManager.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUnitTestManager
#define _H_JUnitTestManager

#include <JString.h>
#include <sstream>
#include <math.h>

class JTestManager;
class JError;

typedef void	(*JUnitTest)();

#define MAX_TEST_COUNT 1000

// must assign to unused int in order to call at load time
#define JTEST(f)	void f(); static int unused_##f = JTestManager::Instance()->RegisterTest(f, #f); void f()

#define JAssertNull(ptr)	JTestManager::Instance()->IsNull(ptr, __FILE__, __LINE__)
#define JAssertNotNull(ptr)	JTestManager::Instance()->IsNotNull(ptr, __FILE__, __LINE__)
#define JAssertTrue(value)	JTestManager::Instance()->IsTrue(value, __FILE__, __LINE__)
#define JAssertFalse(value)	JTestManager::Instance()->IsFalse(value, __FILE__, __LINE__)

#define JAssertNullWithMessage(ptr, msg)	JTestManager::Instance()->IsNull(ptr, __FILE__, __LINE__, msg)
#define JAssertNotNullWithMessage(ptr, msg)	JTestManager::Instance()->IsNotNull(ptr, __FILE__, __LINE__, msg)
#define JAssertTrueWithMessage(value, msg)	JTestManager::Instance()->IsTrue(value, __FILE__, __LINE__, msg)
#define JAssertFalseWithMessage(value, msg)	JTestManager::Instance()->IsFalse(value, __FILE__, __LINE__, msg)

#define JAssertEqual(expected, actual) \
	JAreEqual(expected, actual, __FILE__, __LINE__)

#define JAssertEqualWithMessage(expected, actual, msg) \
	JAreEqual(expected, actual, __FILE__, __LINE__, msg)

#define JAssertStringsEqual(expected, actual) \
	JTestManager::Instance()->StringsAreEqual(expected, actual, __FILE__, __LINE__)

#define JAssertStringsEqualWithMessage(expected, actual, msg) \
	JTestManager::Instance()->StringsAreEqual(expected, actual, __FILE__, __LINE__, msg)

#define JAssertWithin(epsilon, expected, actual) \
	JAreWithin(epsilon, expected, actual, __FILE__, __LINE__)

#define JAssertWithinWithMessage(epsilon, expected, actual, msg) \
	JAreWithin(epsilon, expected, actual, __FILE__, __LINE__, msg)

#define JAssertOK(err) \
	JTestManager::Instance()->IsOK(err, __FILE__, __LINE__)

class JTestManager
{
public:

	static JTestManager* Instance();

	static int	Execute();
	static void	ReportFailure(JUtf8Byte const* message, JUtf8Byte const* file, const JIndex line);
	static void	ReportFatal(JUtf8Byte const* message, JUtf8Byte const* file, const JIndex line);

	int	RegisterTest(JUnitTest test, const JUtf8Byte* name);

	JBoolean	IsNull(const void* ptr, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsNotNull(const void* ptr, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsTrue(const JBoolean value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsTrue(const int value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsFalse(const JBoolean value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsFalse(const int value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsOK(const JError& err, JUtf8Byte const* file, const JIndex line);

	JBoolean	StringsAreEqual(const JString& expectedValue, const JString& actualValue,
								JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	StringsAreEqual(const JString& expectedValue, const JUtf8Byte* actualValue,
								JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	StringsAreEqual(const JUtf8Byte* expectedValue, const JString& actualValue,
								JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	StringsAreEqual(const JUtf8Byte* expectedValue, const JUtf8Byte* actualValue,
								JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);

private:

	JUnitTest			itsTests[ MAX_TEST_COUNT ];
	const JUtf8Byte*	itsNames[ MAX_TEST_COUNT ];
	const JUtf8Byte*	itsCurrentTestName;
	JSize				itsTestCount;
	JSize				itsFailureCount;

private:

	JTestManager();

	~JTestManager();

	void	ExecuteTests();

	// not allowed

	JTestManager(const JTestManager& source);
	const JTestManager& operator=(const JTestManager& source);
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
		JTestManager::Instance()->ReportFailure(s.str().c_str(), file, line);
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
		JTestManager::Instance()->ReportFailure(s.str().c_str(), file, line);
		return kJFalse;
		}
}

inline JBoolean
JTestManager::IsTrue
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
JTestManager::IsFalse
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
JTestManager::StringsAreEqual
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
JTestManager::StringsAreEqual
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
JTestManager::StringsAreEqual
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
