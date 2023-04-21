/******************************************************************************
 JTestManager.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUnitTestManager
#define _H_JUnitTestManager

#include "JString.h"
#include <sstream>
#include <compare>
#include "jMath.h"

class JTestManager;
class JError;

using JUnitTest = void (*)();

#define MAX_TEST_COUNT 1000

// must assign to unused int in order to call at load time
#define JTEST(f)	void f(); static int unused_##f = JTestManager::Instance()->RegisterTest(f, #f); void f()

#define JAssertNull(ptr)	JTestManager::IsNull(ptr, __FILE__, __LINE__)
#define JAssertNotNull(ptr)	JTestManager::IsNotNull(ptr, __FILE__, __LINE__)
#define JAssertTrue(value)	JTestManager::IsTrue(value, __FILE__, __LINE__)
#define JAssertFalse(value)	JTestManager::IsFalse(value, __FILE__, __LINE__)

#define JAssertNullWithMessage(ptr, msg)	JTestManager::IsNull(ptr, __FILE__, __LINE__, msg)
#define JAssertNotNullWithMessage(ptr, msg)	JTestManager::IsNotNull(ptr, __FILE__, __LINE__, msg)
#define JAssertTrueWithMessage(value, msg)	JTestManager::IsTrue(value, __FILE__, __LINE__, msg)
#define JAssertFalseWithMessage(value, msg)	JTestManager::IsFalse(value, __FILE__, __LINE__, msg)

#define JAssertEqual(expected, actual) \
	JAreEqual(expected, actual, __FILE__, __LINE__)

#define JAssertEqualWithMessage(expected, actual, msg) \
	JAreEqual(expected, actual, __FILE__, __LINE__, msg)

#define JAssertStringsEqual(expected, actual) \
	JTestManager::StringsAreEqual(expected, actual, __FILE__, __LINE__)

#define JAssertStringsEqualWithMessage(expected, actual, msg) \
	JTestManager::StringsAreEqual(expected, actual, __FILE__, __LINE__, msg)

#define JAssertWithin(epsilon, expected, actual) \
	JAreWithin(epsilon, expected, actual, __FILE__, __LINE__)

#define JAssertWithinWithMessage(epsilon, expected, actual, msg) \
	JAreWithin(epsilon, expected, actual, __FILE__, __LINE__, msg)

#define JAssertOK(err) \
	JTestManager::IsOK(err, __FILE__, __LINE__)

class JTestManager
{
public:

	static JTestManager* Instance();

	static int	Execute();
	static void	ReportFailure(JUtf8Byte const* message, JUtf8Byte const* file, const JIndex line);

	[[noreturn]] static void	ReportFatal(JUtf8Byte const* message, JUtf8Byte const* file, const JIndex line);

	int	RegisterTest(JUnitTest test, const JUtf8Byte* name);

	static bool	IsNull(const void* ptr, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = nullptr);
	static bool	IsNotNull(const void* ptr, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = nullptr);
	static bool	IsTrue(const bool value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = nullptr);
	static bool	IsFalse(const bool value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = nullptr);
	static bool	IsOK(const JError& err, JUtf8Byte const* file, const JIndex line);

	static bool	StringsAreEqual(const JString& expectedValue, const JString& actualValue,
								JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = nullptr);
	static bool	StringsAreEqual(const JString& expectedValue, const JUtf8Byte* actualValue,
								JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = nullptr);
	static bool	StringsAreEqual(const JUtf8Byte* expectedValue, const JString& actualValue,
								JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = nullptr);
	static bool	StringsAreEqual(const JUtf8Byte* expectedValue, const JUtf8Byte* actualValue,
								JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = nullptr);

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

	JTestManager(const JTestManager&) = delete;
	JTestManager& operator=(const JTestManager&) = delete;
};

inline std::ostream&
operator<<
	(
	std::ostream&				output,
	const std::weak_ordering	v
	)
{
	if (v == std::weak_ordering::less)
	{
		output << "less";
	}
	else if (v == std::weak_ordering::greater)
	{
		output << "greater";
	}
	else
	{
		output << "equivalent";
	}
	return output;
}

template <class A, class B>
bool
JAreEqual
	(
	const A&			expectedValue,
	const B&			actualValue,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg = nullptr
	)
{
	if (expectedValue == actualValue)
		{
		return true;
		}
	else
		{
		std::ostringstream s;
		if (msg != nullptr)
			{
			s << msg << ": ";
			}
		s << "Values are not equal:"
		  << "  Expected " << expectedValue
		  << " but got " << actualValue;

		JTestManager::ReportFailure(s.str().c_str(), file, line);
		return false;
		}
}

template <class T>
bool
JAreWithin
	(
	const T&			epsilon,
	const T&			expectedValue,
	const T&			actualValue,
	JUtf8Byte const*	file,
	const JIndex		line,
	const JUtf8Byte*	msg = nullptr
	)
{
	if (fabs(expectedValue - actualValue) < epsilon)
		{
		return true;
		}

	if (JSign(expectedValue) == JSign(actualValue))
		{
		T logV = log10(fabs(expectedValue));

		const T expectedExponent = floor(logV);
		const T expectedMantissa = logV - expectedExponent;

		logV = log10(fabs(actualValue));

		const T actualExponent = floor(logV);
		const T actualMantissa = logV - expectedExponent;

		if (fabs(expectedExponent - actualExponent) < epsilon &&
			fabs(expectedMantissa - actualMantissa) < epsilon)
			{
			return true;
			}
		}

	std::ostringstream s;
	if (msg != nullptr)
		{
		s << msg << ": ";
		}
	s << "Values are not within " << epsilon << ":"
	  << "  Expected " << expectedValue
	  << " but got " << actualValue;

	JTestManager::ReportFailure(s.str().c_str(), file, line);
	return false;
}

inline bool
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

inline bool
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

inline bool
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
