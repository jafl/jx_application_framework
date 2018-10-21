/******************************************************************************
 TestPrefObject.cpp

	BASE CLASS = JPrefObject

	Written by John Lindal.

 ******************************************************************************/

#include "TestPrefObject.h"
#include <JTestManager.h>
#include <jAssert.h>

static const JUtf8Byte* kText = "foobar";

/******************************************************************************
 Constructor

 ******************************************************************************/

TestPrefObject::TestPrefObject
	(
	JPrefsManager*	prefsMgr,
	const JPrefID&	prefID
	)
	:
	JPrefObject(prefsMgr, prefID),
	itsNumber(prefID.GetID())
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestPrefObject::~TestPrefObject()
{
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
TestPrefObject::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	JAssertEqual(1, vers);

	JIndex number;
	input >> number;
	JAssertEqual(itsNumber, number);

	JString s;
	input >> s;
	JAssertStringsEqual(kText, s);

	JBoolean t;
	input >> t;
	JAssertTrue(t);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
TestPrefObject::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << 1;
	output << ' ' << itsNumber;
	output << ' ' << JString(kText, kJFalse);
	output << ' ' << kJTrue;
}
