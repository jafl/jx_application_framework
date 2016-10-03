/******************************************************************************
 CMGetFullPath.cpp

	BASE CLASS = CMCommand, virtual JBroadcaster

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CMGetFullPath.h"
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* CMGetFullPath::kFileFound    = "FileFound::CMGetFullPath";
const JCharacter* CMGetFullPath::kFileNotFound = "FileNotFound::CMGetFullPath";
const JCharacter* CMGetFullPath::kNewCommand   = "NewCommand::CMGetFullPath";

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetFullPath::CMGetFullPath
	(
	const JString&		cmd,
	const JCharacter*	fileName,
	const JIndex		lineIndex	// for convenience
	)
	:
	CMCommand(cmd, kJTrue, kJFalse),
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
