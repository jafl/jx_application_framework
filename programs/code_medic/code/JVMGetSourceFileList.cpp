/******************************************************************************
 JVMGetSourceFileList.cpp

	BASE CLASS = CMGetSourceFileList

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetSourceFileList.h"
#include "CMFileListDir.h"
#include "JVMLink.h"
#include "cmGlobals.h"
#include <JXFileListTable.h>
#include <JDirInfo.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetSourceFileList::JVMGetSourceFileList
	(
	CMFileListDir* fileList
	)
	:
	CMGetSourceFileList(JString("NOP", JString::kNoCopy), fileList)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetSourceFileList::~JVMGetSourceFileList()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
JVMGetSourceFileList::Starting()
{
	CMGetSourceFileList::Starting();

	dynamic_cast<JVMLink*>(CMGetLink())->FlushClassList();

	JXFileListTable* table = GetFileList()->GetTable();
	table->RemoveAllFiles();

	const JPtrArray<JString>& list = dynamic_cast<JVMLink*>(CMGetLink())->GetSourcePathList();
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
JVMGetSourceFileList::ScanDirectory
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
			if (fileType == kCBJavaSourceFT)
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
JVMGetSourceFileList::HandleSuccess
	(
	const JString& origData
	)
{
}
