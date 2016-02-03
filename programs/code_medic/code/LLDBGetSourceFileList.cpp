/******************************************************************************
 LLDBGetSourceFileList.cpp

	BASE CLASS = CMGetSourceFileList

	Copyright © 2016 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "LLDBGetSourceFileList.h"
#include "CMFileListDir.h"
#include "cmGlobals.h"
#include <JXFileListTable.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <strstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetSourceFileList::LLDBGetSourceFileList
	(
	CMFileListDir* fileList
	)
	:
	CMGetSourceFileList("NOP", fileList)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetSourceFileList::~LLDBGetSourceFileList()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
LLDBGetSourceFileList::HandleSuccess
	(
	const JString& origData
	)
{
}
