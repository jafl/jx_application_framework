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
	CMGetSourceFileList("info sources", fileList)
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
	if (origData.BeginsWith("Source files for which symbols have been read in:"))
		{
		(JXGetApplication())->DisplayBusyCursor();

		JXFileListTable* table = (GetFileList())->GetTable();
		table->RemoveAllFiles();

		JString data = origData;
		JIndex i,j;
		while (data.LocateSubstring("Source files for which symbols", &i))
			{
			j = i;
			if (!data.LocateNextSubstring(":", &j))
				{
				j = data.GetLength();
				}
			data.ReplaceSubstring(i, j, ",");
			}
		data.TrimWhitespace();		// no comma after last file

		std::istrstream input(data.GetCString(), data.GetLength());
		JString fullName, path, name, s;
		JBoolean foundDelimiter;
		do
			{
			input >> ws;
			fullName = JReadUntil(input, ',', &foundDelimiter);
			fullName.TrimWhitespace();
			if (!fullName.IsEmpty())
				{
				JSplitPathAndName(fullName, &path, &name);
				table->AddFile(name);
				}
			}
			while (foundDelimiter);
		}
	else
		{
		std::ostringstream log;
		log << "LLDBGetSourceFileList failed to match";
		(CMGetLink())->Log(log);
		}
}
