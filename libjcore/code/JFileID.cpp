/******************************************************************************
 JFileID.cpp

	Stores system dependent information that can be used to quickly check
	if two files are the same item in a file system.

	BASE CLASS = none

	Copyright (C) 2001 John Lindal.

 *****************************************************************************/

#include "JFileID.h"
#include "JString.h"
#include <sys/stat.h>
#include <ace/OS_NS_sys_stat.h>
#include "jAssert.h"

/******************************************************************************
 Constructor

 *****************************************************************************/

JFileID::JFileID()
	:
	itsValidFlag(false)
{
}

JFileID::JFileID
	(
	const JString& fullName
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
	const JString& fullName
	)
{
	ACE_stat stbuf;
	itsValidFlag = ACE_OS::stat(fullName.GetBytes(), &stbuf) == 0;
	itsDevice    = stbuf.st_dev;
	itsINode     = stbuf.st_ino;
}

/******************************************************************************
 Comparison

 *****************************************************************************/

bool
operator==
	(
	const JFileID& lhs,
	const JFileID& rhs
	)
{
	return lhs.itsValidFlag == rhs.itsValidFlag &&
				lhs.itsDevice    == rhs.itsDevice    &&
				lhs.itsINode     == rhs.itsINode;
}

/******************************************************************************
 Compare (static)

 ******************************************************************************/

JListT::CompareResult
JFileID::Compare
	(
	const JFileID& id1,
	const JFileID& id2
	)
{
	if (id1.itsValidFlag && !id2.itsValidFlag)
		{
		return JListT::kFirstLessSecond;
		}
	else if (!id1.itsValidFlag && id2.itsValidFlag)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (id1.itsDevice < id2.itsDevice)
		{
		return JListT::kFirstLessSecond;
		}
	else if (id1.itsDevice > id2.itsDevice)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (id1.itsINode < id2.itsINode)
		{
		return JListT::kFirstLessSecond;
		}
	else if (id1.itsINode > id2.itsINode)
		{
		return JListT::kFirstGreaterSecond;
		}
	else
		{
		return JListT::kFirstEqualSecond;
		}
}
