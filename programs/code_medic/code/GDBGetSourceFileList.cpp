/******************************************************************************
 GDBGetSourceFileList.cpp

	BASE CLASS = CMGetSourceFileList

	We do not use -file-list-exec-source-files because it generates much
	more data and the output takes too long to parse.

	Copyright � 2001-10 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include "GDBGetSourceFileList.h"
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

GDBGetSourceFileList::GDBGetSourceFileList
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

GDBGetSourceFileList::~GDBGetSourceFileList()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
GDBGetSourceFileList::HandleSuccess
	(
	const JString& origData
	)
{
	if (origData.BeginsWith("Source files for which symbols have been read in:"))
		{
		(JXGetApplication())->DisplayBusyCursor();

		JXFileListTable* table = GetFileList()->GetTable();
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
		log << "GDBGetSourceFileList failed to match";
		(CMGetLink())->Log(log);
		}
}
