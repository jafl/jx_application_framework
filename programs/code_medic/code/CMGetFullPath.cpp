/******************************************************************************
 CMGetFullPath.cpp

	BASE CLASS = CMCommand, virtual JBroadcaster

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CMGetFullPath.h"
#include <jAssert.h>

// JBroadcaster message types

const JUtf8Byte* CMGetFullPath::kFileFound    = "FileFound::CMGetFullPath";
const JUtf8Byte* CMGetFullPath::kFileNotFound = "FileNotFound::CMGetFullPath";
const JUtf8Byte* CMGetFullPath::kNewCommand   = "NewCommand::CMGetFullPath";

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetFullPath::CMGetFullPath
	(
	const JString&	cmd,
	const JString&	fileName,
	const JIndex	lineIndex	// for convenience
	)
	:
	CMCommand(cmd, true, false),
	itsFileName(fileName),
	itsLineIndex(lineIndex)
{
	CMCommand::Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetFullPath::~CMGetFullPath()
{
}
