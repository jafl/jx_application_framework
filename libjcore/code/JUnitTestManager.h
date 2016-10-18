/******************************************************************************
 JUnitTestManager.h

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JUnitTestManager
#define _H_JUnitTestManager

#include <jTypes.h>
#include <sstream>

class JUnitTestManager;

typedef void	(*JUnitTest)();

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

class JUnitTestManager
{
public:

	static JUnitTestManager* Instance();

	static void	Execute(const JUnitTest* tests, const JBoolean exit = kJTrue);	// NULL terminated array
	static void	ReportFailure(JUtf8Byte const* message, JUtf8Byte const* file, const JIndex line);
	static void	Exit();

	JBoolean	IsNull(const void* ptr, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsTrue(const JBoolean value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsTrue(const int value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsFalse(const JBoolean value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);
	JBoolean	IsFalse(const int value, JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);

	JBoolean	StringsAreEqual(const JUtf8Byte* expectedValue, const JUtf8Byte* actualValue,
								JUtf8Byte const* file, const JIndex line, const JUtf8Byte* msg = NULL);

private:

	JSize	itsFailureCount;

private:

	JUnitTestManager();

	~JUnitTestManager();

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
	else if (msg != NULL)
		{
		JUnitTestManager::Instance()->ReportFailure(msg, file, line);
		return kJFalse;
		}
	else
		{
		std::ostringstream s;
		s << "Values are not equal." 
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

#endif
