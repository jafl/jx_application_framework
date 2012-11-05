/******************************************************************************
 GDBGetFullPath.cpp

	Each file has to be checked separately.  The compilation directory does
	not necessarily have anything to do with the file's path.  Only
	"Located in file_name" printed by "info source" is reliable.

	BASE CLASS = CMGetFullPath

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "GDBGetFullPath.h"
#include "cmGlobals.h"
#include <JRegex.h>
#include <jFileUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetFullPath::GDBGetFullPath
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex	// for convenience
	)
	:
	CMGetFullPath(BuildCommand(fileName), fileName, lineIndex)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetFullPath::~GDBGetFullPath()
{
}

/******************************************************************************
 BuildCommand (static private)

 ******************************************************************************/

JString
GDBGetFullPath::BuildCommand
	(
	const JCharacter* fileName
	)
{
	JString cmd = "list ";
	cmd        += fileName;
	cmd        += ":1,1\n-file-list-exec-source-file";
	return cmd;
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex pathPattern = ",fullname=\"([^\"]+)\"";

void
GDBGetFullPath::HandleSuccess
	(
	const JString& cmdData
	)
{
	const JString& data = GetLastResult();
	JBoolean fileOK     = kJFalse;

	const JPtrArray<JString>& resultList = GetResults();
	JString data1;
	if (resultList.GetElementCount() == 2)
		{
		data1 = *(resultList.FirstElement());
		}

	JArray<JIndexRange> matchList;
	if (!data1.Contains("No source file") &&
		pathPattern.Match(data, &matchList))
		{
		const JString fullName = data.GetSubstring(matchList.GetElement(2));
		if (JFileReadable(fullName))
			{
			fileOK = kJTrue;
			(CMGetLink())->RememberFile(GetFileName(), fullName);
			Broadcast(FileFound(fullName, GetLineIndex()));
			}
		}
	else
		{
		(CMGetLink())->Log("GDBGetFullPath failed to match");
		}

	if (!fileOK)
		{
		Broadcast(FileNotFound(GetFileName()));
		(CMGetLink())->RememberFile(GetFileName(), NULL);
		}
}
