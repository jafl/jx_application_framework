/******************************************************************************
 LLDBGetSourceFileList.cpp

	BASE CLASS = CMGetSourceFileList

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetSourceFileList.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBModule.h"
#include "lldb/API/SBCompileUnit.h"
#include "CMFileListDir.h"
#include "LLDBLink.h"
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
	CMGetSourceFileList("", fileList)
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
	LLDBLink* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == nullptr)
		{
		return;
		}

	lldb::SBTarget t = link->GetDebugger()->GetSelectedTarget();
	if (t.IsValid())
		{
		(JXGetApplication())->DisplayBusyCursor();

		JXFileListTable* table = GetFileList()->GetTable();
		table->RemoveAllFiles();

		JString fullName;

		const JSize mCount = t.GetNumModules();
		for (JIndex i=0; i<mCount; i++)
			{
			lldb::SBModule m   = t.GetModuleAtIndex(i);
			const JSize uCount = m.GetNumCompileUnits();
			for (JIndex j=0; j<uCount; j++)
				{
				lldb::SBCompileUnit u = m.GetCompileUnitAtIndex(j);
				lldb::SBFileSpec f    = u.GetFileSpec();

				if (f.GetDirectory() != nullptr && f.GetFilename() != nullptr)
					{
					fullName = JCombinePathAndName(f.GetDirectory(), f.GetFilename());
					table->AddFile(fullName);
					}
				}
			}
		}
}
