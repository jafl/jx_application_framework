/******************************************************************************
 JVMGetFullPath.cpp

	BASE CLASS = CMGetFullPath

	Copyright (C) 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "JVMGetFullPath.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetFullPath::JVMGetFullPath
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

JVMGetFullPath::~JVMGetFullPath()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetFullPath::HandleSuccess
	(
	const JString& data
	)
{
}
