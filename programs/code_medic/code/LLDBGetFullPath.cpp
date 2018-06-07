/******************************************************************************
 LLDBGetFullPath.cpp

	BASE CLASS = CMGetFullPath

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetFullPath.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetFullPath::LLDBGetFullPath
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex	// for convenience
	)
	:
	CMGetFullPath("NOP", fileName, lineIndex)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetFullPath::~LLDBGetFullPath()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetFullPath::HandleSuccess
	(
	const JString& cmdData
	)
{
	Broadcast(FileNotFound(GetFileName()));
	(CMGetLink())->RememberFile(GetFileName(), nullptr);
}
