/******************************************************************************
 MDRecord.cc

	BASE CLASS = JBroadcaster

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "MDRecord.h"
#include <JMemoryManager.h>
#include <JPtrArray-JString.h>
#include <jFileUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	Dual function is JMMRecord::StreamForDebug().

 *****************************************************************************/

MDRecord::MDRecord()
{
}

MDRecord::MDRecord
	(
	std::istream& input
	)
{
	input >> itsIsValidFlag >> itsIsArrayNewFlag;
	input >> itsNewFile >> itsNewLine;
	input >> itsSize >> itsData;
}

// search target

MDRecord::MDRecord
	(
	const JCharacter* fileName
	)
	:
	itsNewFile(fileName)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MDRecord::~MDRecord()
{
}

/******************************************************************************
 Comparisons (static private)

 ******************************************************************************/

JListT::CompareResult
MDRecord::CompareState
	(
	MDRecord * const & r1,
	MDRecord * const & r2
	)
{
	if (!r1->itsIsValidFlag && r2->itsIsValidFlag)
		{
		return JListT::kFirstLessSecond;
		}
	else if (r1->itsIsValidFlag && !r2->itsIsValidFlag)
		{
		return JListT::kFirstGreaterSecond;
		}
	else
		{
		return CompareFileName(r1, r2);
		}
}

JListT::CompareResult
MDRecord::CompareFileName
	(
	MDRecord * const & r1,
	MDRecord * const & r2
	)
{
	const JCharacter* s1 = strrchr(r1->itsNewFile, ACE_DIRECTORY_SEPARATOR_CHAR);
	if (s1 == NULL)
		{
		s1 = r1->GetNewFile();
		}
	else
		{
		s1++;
		}

	const JCharacter* s2 = strrchr(r2->itsNewFile, ACE_DIRECTORY_SEPARATOR_CHAR);
	if (s2 == NULL)
		{
		s2 = r2->GetNewFile();
		}
	else
		{
		s2++;
		}

	int result = JString::Compare(s1, s2, kJFalse);

	if (result == 0)
		{
		result = r1->itsNewLine - r2->itsNewLine;
		}

	if (result < 0)
		{
		return JListT::kFirstLessSecond;
		}
	else if (result > 0)
		{
		return JListT::kFirstGreaterSecond;
		}
	else
		{
		return JListT::kFirstEqualSecond;
		}
}

JListT::CompareResult
MDRecord::CompareSize
	(
	MDRecord * const & r1,
	MDRecord * const & r2
	)
{
	const int result = r1->itsSize - r2->itsSize;
	if (result < 0)
		{
		return JListT::kFirstLessSecond;
		}
	else if (result > 0)
		{
		return JListT::kFirstGreaterSecond;
		}
	else
		{
		return CompareFileName(r1, r2);
		}
}

JListT::CompareResult
MDRecord::CompareData
	(
	MDRecord * const & r1,
	MDRecord * const & r2
	)
{
	JListT::CompareResult	result =
		JCompareStringsCaseInsensitive(const_cast<JString*>(&r1->itsData),
									   const_cast<JString*>(&r2->itsData));

	if (result == JListT::kFirstEqualSecond)
		{
		return CompareFileName(r1, r2);
		}
	else
		{
		return result;
		}
}
