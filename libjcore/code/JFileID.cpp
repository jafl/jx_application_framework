/******************************************************************************
 JFileID.cpp

	Stores system dependent information that can be used to quickly check
	if two files are the same item in a file system.

	BASE CLASS = none

	Copyright © 2001 John Lindal. All rights reserved.

 *****************************************************************************/

#include <JCoreStdInc.h>
#include <JFileID.h>
#include <sys/stat.h>
#include <ace/OS.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JFileID::JFileID()
	:
	itsValidFlag(kJFalse)
{
}

JFileID::JFileID
	(
	const JCharacter* fullName
	)
{
	SetFileName(fullName);
}

/******************************************************************************
 SetFileName

 *****************************************************************************/

void
JFileID::SetFileName
	(
	const JCharacter* fullName
	)
{
	ACE_stat stbuf;
	itsValidFlag = JI2B( ACE_OS::stat(fullName, &stbuf) == 0 );
	itsDevice    = stbuf.st_dev;
	itsINode     = stbuf.st_ino;
}

/******************************************************************************
 Comparison

 *****************************************************************************/

JBoolean
operator==
	(
	const JFileID& lhs,
	const JFileID& rhs
	)
{
	return JI2B(lhs.itsValidFlag == rhs.itsValidFlag &&
				lhs.itsDevice    == rhs.itsDevice    &&
				lhs.itsINode     == rhs.itsINode);
}

/******************************************************************************
 Compare (static)

 ******************************************************************************/

JOrderedSetT::CompareResult
JFileID::Compare
	(
	const JFileID& id1,
	const JFileID& id2
	)
{
	if (id1.itsValidFlag && !id2.itsValidFlag)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (!id1.itsValidFlag && id2.itsValidFlag)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (id1.itsDevice < id2.itsDevice)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (id1.itsDevice > id2.itsDevice)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (id1.itsINode < id2.itsINode)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (id1.itsINode > id2.itsINode)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}
