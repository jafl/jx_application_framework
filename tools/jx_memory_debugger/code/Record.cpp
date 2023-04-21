/******************************************************************************
 Record.cpp

	BASE CLASS = JBroadcaster

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "Record.h"
#include <jx-af/jcore/JMemoryManager.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

	Dual function is JMMRecord::StreamForDebug().

 *****************************************************************************/

Record::Record()
{
}

Record::Record
	(
	std::istream& input
	)
{
	input >> JBoolFromString(itsIsValidFlag) >> JBoolFromString(itsIsArrayNewFlag);
	input >> itsNewFile >> itsNewLine;
	input >> itsSize >> itsData;
}

// search target

Record::Record
	(
	const JString& fileName
	)
	:
	itsNewFile(fileName)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

Record::~Record()
{
}

/******************************************************************************
 Comparisons (static private)

 ******************************************************************************/

std::weak_ordering
Record::CompareState
	(
	Record * const & r1,
	Record * const & r2
	)
{
	if (!r1->itsIsValidFlag && r2->itsIsValidFlag)
	{
		return std::weak_ordering::less;
	}
	else if (r1->itsIsValidFlag && !r2->itsIsValidFlag)
	{
		return std::weak_ordering::greater;
	}
	else
	{
		return CompareFileNames(r1, r2);
	}
}

std::weak_ordering
Record::CompareFileNames
	(
	Record * const & r1,
	Record * const & r2
	)
{
	const JUtf8Byte* s1 = strrchr(r1->itsNewFile.GetBytes(), ACE_DIRECTORY_SEPARATOR_CHAR);
	if (s1 == nullptr)
	{
		s1 = r1->GetNewFile().GetBytes();
	}
	else
	{
		s1++;
	}

	const JUtf8Byte* s2 = strrchr(r2->itsNewFile.GetBytes(), ACE_DIRECTORY_SEPARATOR_CHAR);
	if (s2 == nullptr)
	{
		s2 = r2->GetNewFile().GetBytes();
	}
	else
	{
		s2++;
	}

	int result = JString::Compare(s1, s2, JString::kIgnoreCase);
	if (result == 0)
	{
		result = r1->itsNewLine - r2->itsNewLine;
	}
	return JIntToWeakOrdering(result);
}

std::weak_ordering
Record::CompareSizes
	(
	Record * const & r1,
	Record * const & r2
	)
{
	std::weak_ordering result = JCompareSizes(r1->itsSize, r2->itsSize);
	if (result == std::weak_ordering::equivalent)
	{
		result = CompareFileNames(r1, r2);
	}
	return result;
}

std::weak_ordering
Record::CompareData
	(
	Record * const & r1,
	Record * const & r2
	)
{
	std::weak_ordering result =
		JCompareStringsCaseInsensitive(&r1->itsData, &r2->itsData);

	if (result == std::weak_ordering::equivalent)
	{
		result = CompareFileNames(r1, r2);
	}
	return result;
}
