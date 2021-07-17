/******************************************************************************
 GDBGetSourceFileList.cpp

	BASE CLASS = CMGetSourceFileList

	We do not use -file-list-exec-source-files because it generates much
	more data and the output takes too long to parse.

	Copyright (C) 2001-10 by John Lindal.

 ******************************************************************************/

#include "GDBGetSourceFileList.h"
#include "CMFileListDir.h"
#include "cmGlobals.h"
#include <JXFileListTable.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetSourceFileList::GDBGetSourceFileList
	(
	CMFileListDir* fileList
	)
	:
	CMGetSourceFileList(JString("info sources", JString::kNoCopy), fileList)
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

static const JRegex textPattern = "Source files for which symbols[^:]+(:|$)";

void
GDBGetSourceFileList::HandleSuccess
	(
	const JString& origData
	)
{
	if (origData.BeginsWith("Source files for which symbols have been read in:"))
		{
		JXGetApplication()->DisplayBusyCursor();

		JXFileListTable* table = GetFileList()->GetTable();
		table->RemoveAllFiles();

		JString data = origData;

		JStringIterator iter(&data);
		while (iter.Next(textPattern))
			{
			iter.ReplaceLastMatch(",");
			}
		data.TrimWhitespace();		// no comma after last file

		icharbufstream input(data.GetRawBytes(), data.GetByteCount());
		JString fullName, path, name, s;
		bool foundDelimiter;
		do
			{
			input >> std::ws;
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
		CMGetLink()->Log(log);
		}
}
