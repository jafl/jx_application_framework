/******************************************************************************
 CMLocation.cpp

	BASE CLASS = none

	Copyright © 2001 John Lindal. All rights reserved.

 *****************************************************************************/

#include <CMLocation.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CMLocation::CMLocation()
	:
	itsLineNumber(0)
{
}

CMLocation::CMLocation
	(
	const JCharacter*	fileName,
	const JIndex		lineNumber
	)
	:
	itsFileName(fileName),
	itsFileID(fileName),
	itsLineNumber(lineNumber)
{
	if (!itsFileName.IsEmpty() && JIsRelativePath(itsFileName))
		{
		JString p, n;
		JSplitPathAndName(itsFileName, &p, &n);
		itsFileName = n;
		}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMLocation::~CMLocation()
{
}

/******************************************************************************
 Copy constructor

 *****************************************************************************/

CMLocation::CMLocation
	(
	const CMLocation& source
	)
{
	*this = source;
}

/******************************************************************************
 Assignment

 *****************************************************************************/

const CMLocation&
CMLocation::operator=
	(
	const CMLocation& source
	)
{
	if (this != &source)
		{
		itsFileName      = source.itsFileName;
		itsFileID        = source.itsFileID;
		itsLineNumber    = source.itsLineNumber;
		itsFunctionName  = source.itsFunctionName;
		itsMemoryAddress = source.itsMemoryAddress;
		}

	return *this;
}

/******************************************************************************
 Comparison

 *****************************************************************************/

JBoolean
operator==
	(
	const CMLocation& lhs,
	const CMLocation& rhs
	)
{
	return JI2B(lhs.itsFileID     == rhs.itsFileID &&
				lhs.itsLineNumber == rhs.itsLineNumber);
}
