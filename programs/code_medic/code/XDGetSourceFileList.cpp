/******************************************************************************
 XDGetSourceFileList.cpp

	BASE CLASS = CMGetSourceFileList

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetSourceFileList.h"
#include "CMFileListDir.h"
#include "XDLink.h"
#include "cmGlobals.h"
#include <JXFileListTable.h>
#include <JDirInfo.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetSourceFileList::XDGetSourceFileList
	(
	CMFileListDir* fileList
	)
	:
	CMGetSourceFileList(JString("status", JString::kNoCopy), fileList)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetSourceFileList::~XDGetSourceFileList()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
XDGetSourceFileList::Starting()
{
	CMGetSourceFileList::Starting();

	JXFileListTable* table = GetFileList()->GetTable();
	table->RemoveAllFiles();

	const JPtrArray<JString>& list = dynamic_cast<XDLink*>(CMGetLink())->GetSourcePathList();
	const JSize count              = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		ScanDirectory(*(list.GetElement(i)));
		}
}

/******************************************************************************
 ScanDirectory (private)

 *****************************************************************************/

void
XDGetSourceFileList::ScanDirectory
	(
	const JString& path
	)
{
	JDirInfo* info;
	if (!JDirInfo::Create(path, &info))
		{
		return;
		}

	JXFileListTable* table = GetFileList()->GetTable();

	const JSize count = info->GetEntryCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JDirEntry& e = info->GetEntry(i);
		if (e.GetType() == JDirEntry::kFile)
			{
			const CBTextFileType fileType =
				CMGetPrefsManager()->GetFileType(e.GetName());
			if (fileType == kCBPHPFT)
				{
				table->AddFile(e.GetFullName());
				}
			}
		else if (e.GetType() == JDirEntry::kDir)
			{
			ScanDirectory(e.GetFullName());
			}
		}

	jdelete info;
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDGetSourceFileList::HandleSuccess
	(
	const JString& origData
	)
{
}
