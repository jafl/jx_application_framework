/******************************************************************************
 XDGetFullPath.cpp

	BASE CLASS = CMGetFullPath

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetFullPath.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetFullPath::XDGetFullPath
	(
	const JString&	fileName,
	const JIndex	lineIndex	// for convenience
	)
	:
	CMGetFullPath(JString("status", JString::kNoCopy), fileName, lineIndex)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetFullPath::~XDGetFullPath()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetFullPath::HandleSuccess
	(
	const JString& data
	)
{
}
