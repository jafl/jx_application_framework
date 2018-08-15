/******************************************************************************
 SyGCopyProcess.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2000 by John Lindal.

 *****************************************************************************/

#include "SyGCopyProcess.h"
#include "SyGFileTreeTable.h"
#include "SyGGlobals.h"
#include <JXDeleteObjectTask.h>
#include <JTableSelection.h>
#include <JSimpleProcess.h>
#include <JThisProcess.h>
#include <JFSFileTree.h>
#include <JDirInfo.h>
#include <jFileUtil.h>
#include <jMountUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

	srcTable can be nullptr.  We take ownership of srcNameList.

 ******************************************************************************/

void
SyGCopyProcess::Copy
	(
	SyGFileTreeTable*	srcTable,
	JPtrArray<JString>*	srcNameList,
	SyGFileTreeTable*	destTable,
	SyGFileTreeNode*	destNode
	)
{
	if (CleanSrcList(srcNameList, destNode))
		{
		SyGCopyProcess* p =
			jnew SyGCopyProcess(srcTable, srcNameList, destTable, destNode, kJTrue);
		assert( p != nullptr );
		}
}

void
SyGCopyProcess::Move
	(
	SyGFileTreeTable*	srcTable,
	JPtrArray<JString>*	srcNameList,
	SyGFileTreeTable*	destTable,
	SyGFileTreeNode*	destNode
	)
{
	if (CleanSrcList(srcNameList, destNode))
		{
		SyGCopyProcess* p =
			jnew SyGCopyProcess(srcTable, srcNameList, destTable, destNode, kJFalse);
		assert( p != nullptr );
		}
}

/******************************************************************************
 CleanSrcList (static private)

	Removes items that would go nowhere or which user doesn't want to
	overwrite.  If nothing is left, deletes the list.

 ******************************************************************************/

JBoolean
SyGCopyProcess::CleanSrcList
	(
	JPtrArray<JString>*	srcNameList,
	SyGFileTreeNode*	destNode
	)
{
	JString destPath = (destNode->GetDirEntry())->GetFullName();
	JAppendDirSeparator(&destPath);

	JSize count = srcNameList->GetElementCount();
	JString destName;
	JBoolean ask, first = kJTrue;
	for (JIndex i=1; i<=count; i++)
		{
		const JString* srcName = srcNameList->GetElement(i);
		if (!ActionIsUseful(*srcName, destPath, &destName) ||
			!OKToReplace(*srcName, destName, &ask, &first))
			{
			srcNameList->DeleteElement(i);
			i--;
			count--;	// don't confuse user by going in reverse order
			}
		}

	if (srcNameList->IsEmpty())
		{
		jdelete srcNameList;
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 ActionIsUseful (static private)

	Returns kJFalse if move/copy would produce no effect.  Otherwise,
	*destName is the destination path and name.

 ******************************************************************************/

JBoolean
SyGCopyProcess::ActionIsUseful
	(
	const JString&	srcName,
	const JString&	destPath,
	JString*		destName
	)
{
	if (!JNameUsed(srcName) || JSameDirEntry(srcName, destPath))
		{
		return kJFalse;
		}

	JString fullName(srcName), srcPath, name;
	JStripTrailingDirSeparator(&fullName);
	JSplitPathAndName(fullName, &srcPath, &name);

	if (JSameDirEntry(srcPath, destPath))
		{
		return kJFalse;
		}
	else
		{
		*destName = JCombinePathAndName(destPath, name);
		return kJTrue;
		}
}

/******************************************************************************
 OKToReplace (static private)

 ******************************************************************************/

JBoolean
SyGCopyProcess::OKToReplace
	(
	const JString&	srcName,
	const JString&	destName,
	JBoolean*		ask,
	JBoolean*		first
	)
{
	JString destPath, name;
	JSplitPathAndName(destName, &destPath, &name);	// src has same name

	if (!JNameUsed(destName))
		{
		if (JDirectoryExists(srcName) &&
			JDirectoryExists(destPath))		// can't move into self
			{
			JString trueSrc, trueDest;
			JBoolean ok = JGetTrueName(srcName, &trueSrc);
			assert( ok );
			ok = JGetTrueName(destPath, &trueDest);
			assert( ok );

			if (trueDest.BeginsWith(trueSrc))
				{
				const JUtf8Byte* map[] =
					{
					"src", name.GetBytes()
					};
				const JString msg = JGetString("NoMoveIntoSelf::SyGCopyProcess", map, sizeof(map));
				(JGetUserNotification())->ReportError(msg);
				return kJFalse;
				}
			}

		return kJTrue;
		}

	const JBoolean srcIsDir  = JDirectoryExists(srcName);
	const JBoolean destIsDir = JDirectoryExists(destName);
	if (srcIsDir != destIsDir)			// can't replace one type with the other
		{
		const JUtf8Byte* map[] =
			{
			"src_type",  (srcIsDir  ? "directory" : "file"),
			"dest_type", (destIsDir ? "directory" : "file"),
			"dest",      name.GetBytes()
			};
		const JString msg = JGetString("OnlyReplaceSameType::SyGCopyProcess", map, sizeof(map));
		(JGetUserNotification())->ReportError(msg);
		return kJFalse;
		}
	else if (srcIsDir && destIsDir)		// can't be replaced by contents or v.v.
		{
		JString trueSrc, trueDest;
		JBoolean ok = JGetTrueName(srcName, &trueSrc);
		assert( ok );
		ok = JGetTrueName(destName, &trueDest);
		assert( ok );

		if (trueSrc.BeginsWith(trueDest))
			{
			const JUtf8Byte* map[] =
				{
				"dest", name.GetBytes()
				};
			const JString msg = JGetString("NoReplaceWithContents::SyGCopyProcess", map, sizeof(map));
			(JGetUserNotification())->ReportError(msg);
			return kJFalse;
			}
		else if (trueDest.BeginsWith(trueSrc))
			{
			const JUtf8Byte* map[] =
				{
				"src", name.GetBytes()
				};
			const JString msg = JGetString("NoMoveIntoSelf::SyGCopyProcess", map, sizeof(map));
			(JGetUserNotification())->ReportError(msg);
			return kJFalse;
			}
		}

	if (*first)
		{
		*ask   = !(JGetUserNotification())->AskUserYes(JGetString("AskAutoReplace::SyGCopyProcess"));
		*first = kJFalse;
		}

	if (*ask)
		{
		const JUtf8Byte* map[] =
			{
			"name", name.GetBytes()
			};
		const JString msg = JGetString("AskReplace::SyGCopyProcess", map, sizeof(map));
		if (!(JGetUserNotification())->AskUserYes(msg))
			{
			return kJFalse;
			}
		}

	return SyGDeleteDirEntry(destName);
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

SyGCopyProcess::SyGCopyProcess
	(
	SyGFileTreeTable*	srcTable,
	JPtrArray<JString>*	srcNameList,
	SyGFileTreeTable*	destTable,
	SyGFileTreeNode*	destNode,
	const JBoolean		isCopy
	)
	:
	itsSrcTable(srcTable),
	itsSrcNameList(srcNameList),
	itsDestTable(destTable),
	itsDestNode(destNode),
	itsIsMoveFlag(!isCopy),
	itsProcess(nullptr)
{
	assert( !itsSrcNameList->IsEmpty() );
	itsSrcNameList->SetCleanUpAction(JPtrArrayT::kDeleteAll);

	JString destPath = (itsDestNode->GetDirEntry())->GetFullName();
	JAppendDirSeparator(&destPath);

	itsVCSType = kJUnknownVCSType;
	{
	JVCSType type1, type2, type3;
	JBoolean anyVCS, allVCS, sameVCS;

	const JSize srcCount = srcNameList->GetElementCount();
	JString path, name;
	for (JIndex i=1; i<=srcCount; i++)
		{
		const JString* src   = srcNameList->GetElement(i);
		const JBoolean isDir = JDirectoryExists(*src);
		JBoolean isVCS3      = kJFalse;
		if (isDir)
			{
			isVCS3 = JIsManagedByVCS(*src, &type3);
			JSplitPathAndName(*src, &path, &name);
			}
		else
			{
			path = *src;
			}

		JBoolean isVCS;
		if (isDir && !isVCS3)
			{
			isVCS = kJFalse;
			type2 = kJUnknownVCSType;
			}
		else
			{
			isVCS = JIsManagedByVCS(path, &type2);
			}

		if (i == 1)
			{
			type1 = type2;
			anyVCS = allVCS = sameVCS = isVCS;
			}
		else if (isVCS)
			{
			if (type2 != type1)
				{
				sameVCS = kJFalse;
				}
			anyVCS = kJTrue;
			}
		else
			{
			sameVCS = allVCS = kJFalse;
			}
		}

	if (allVCS && sameVCS)
		{
		type2 = JGetVCSType(destPath);
		if (isCopy && type1 == type2 && type1 != kJGitType)
			{
			if ((JGetUserNotification())->AskUserYes(JGetString("UseVCSCopy::SyGCopyProcess")))
				{
				itsVCSType = type1;
				}
			}
		else if (!isCopy && type1 == type2)
			{
			itsVCSType = type1;
			}
		else if (!isCopy &&
				 ((type2 == kJUnknownVCSType && !(JGetUserNotification())->AskUserNo(JGetString("NoTargetVCSMove::SyGCopyProcess"))) ||
				  (type2 != kJUnknownVCSType && !(JGetUserNotification())->AskUserNo(JGetString("MixedVCSMove::SyGCopyProcess")))))
			{
			JXDeleteObjectTask<JBroadcaster>::Delete(this);
			return;
			}
		}
	else if (!isCopy &&
			 ((allVCS && !(JGetUserNotification())->AskUserNo(JGetString("MixedVCSMove::SyGCopyProcess"))) ||
			  (anyVCS && !(JGetUserNotification())->AskUserNo(JGetString("SomeVCSMove::SyGCopyProcess")))))
		{
		JXDeleteObjectTask<JBroadcaster>::Delete(this);
		return;
		}
	}

	if (itsSrcTable != nullptr)
		{
		(itsSrcTable->GetTableSelection()).ClearSelection();
		ClearWhenGoingAway(itsSrcTable, &itsSrcTable);
		}

	(itsDestTable->GetTableSelection()).ClearSelection();
	ClearWhenGoingAway(itsDestTable, &itsDestTable);
	ClearWhenGoingAway(itsDestTable, &itsDestNode);
	ClearWhenGoingAway(itsDestNode, &itsDestNode);

	JSize prefixCount = 0;
	if (isCopy && itsVCSType == kJSVNType)
		{
		prefixCount = 2;
		itsSrcNameList->InsertAtIndex(1, JString("svn", kJFalse));
		itsSrcNameList->InsertAtIndex(2, JString("cp", kJFalse));
		}
	else if (isCopy)
		{
		prefixCount = 2;
		itsSrcNameList->InsertAtIndex(1, JString("cp", kJFalse));
		itsSrcNameList->InsertAtIndex(2, JString("-Rdf", kJFalse));
		}
	else if (itsVCSType == kJSVNType)
		{
		prefixCount = 3;
		itsSrcNameList->InsertAtIndex(1, JString("svn", kJFalse));
		itsSrcNameList->InsertAtIndex(2, JString("mv", kJFalse));
		itsSrcNameList->InsertAtIndex(3, JString("--force", kJFalse));
		}
	else if (itsVCSType == kJGitType)
		{
		prefixCount = 3;
		itsSrcNameList->InsertAtIndex(1, JString("git", kJFalse));
		itsSrcNameList->InsertAtIndex(2, JString("mv", kJFalse));
		itsSrcNameList->InsertAtIndex(3, JString("-f", kJFalse));
		}
	else
		{
		prefixCount = 2;
		itsSrcNameList->InsertAtIndex(1, JString("mv", kJFalse));
		itsSrcNameList->InsertAtIndex(2, JString("-f", kJFalse));
		}

	itsSrcNameList->Append(destPath);

	Start(prefixCount);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGCopyProcess::~SyGCopyProcess()
{
	assert( itsProcess == nullptr );
	jdelete itsSrcNameList;
}

/******************************************************************************
 Start (private)

 ******************************************************************************/

void
SyGCopyProcess::Start
	(
	const JSize prefixCount
	)
{
	const JString* destPath = itsSrcNameList->GetLastElement();

	const JError err = JSimpleProcess::Create(&itsProcess, *destPath, *itsSrcNameList);

	// clean up to make Receive simpler

	for (JIndex i=1; i<=prefixCount; i++)
		{
		itsSrcNameList->DeleteElement(1);
		}

	err.ReportIfError();
	if (err.OK())
		{
		ListenTo(itsProcess);
		JThisProcess::Ignore(itsProcess);	// detach so it always finishes
		}
	else
		{
		JXDeleteObjectTask<JBroadcaster>::Delete(this);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SyGCopyProcess::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsProcess && message.Is(JProcess::kFinished))
		{
		JString* destPath = itsSrcNameList->GetLastElement();
		itsSrcNameList->RemoveElement(itsSrcNameList->GetElementCount());

		JBoolean done = kJTrue;

		JSimpleProcess* process = itsProcess;
		JXDeleteObjectTask<JBroadcaster>::Delete(itsProcess);
		itsProcess = nullptr;

		const JProcess::Finished* info =
			dynamic_cast<const JProcess::Finished*>(&message);
		if (info->Successful())
			{
			if (itsSrcTable != nullptr)
				{
				itsSrcTable->UpdateDisplay(kJTrue);
				}

			JBoolean selectName = JI2B(itsDestNode != nullptr);
			assert( selectName == JI2B(itsDestTable != nullptr) );

			if (selectName && itsDestTable->IsEditing())
				{
				itsDestTable->UpdateDisplay(kJTrue);
				selectName = kJFalse;
				}

			const JSize count = itsSrcNameList->GetElementCount();
			JString path, name, oldName, newName, device;
			JFileSystemType fsType;
			for (JIndex i=1; i<=count; i++)
				{
				JString* fullName = itsSrcNameList->GetElement(i);
				JStripTrailingDirSeparator(fullName);
				JSplitPathAndName(*fullName, &path, &name);

				JBoolean writable, isTop;
				if (JIsMounted(*fullName, &writable, &isTop, &device, &fsType) &&
					!JMountSupportsExecFlag(fsType))
					{
					RemoveExecutePermissions(*fullName, *destPath);
					}

				if (selectName)
					{
					JPoint cell;
					itsDestTable->SelectName(name, itsDestNode, &cell);	// updates table
					}

				newName = JCombinePathAndName(*destPath, name);
				if (itsIsMoveFlag && JDirectoryExists(newName))
					{
					oldName = *(itsSrcNameList->GetElement(i));
					JAppendDirSeparator(&oldName);
					JAppendDirSeparator(&newName);
					JFSFileTree::DirectoryRenamed msg(oldName, newName);
					(SyGGetApplication())->DirectoryRenamed(msg);
					}
				}
			}
		else if (itsVCSType != kJUnknownVCSType && itsIsMoveFlag)
			{
			process->ReportError(kJFalse);

			if ((JGetUserNotification())->AskUserYes(JGetString("AskPlainVCSMove::SyGCopyProcess")))
				{
				done = kJFalse;

				itsSrcNameList->InsertAtIndex(1, JString("mv", kJFalse));
				itsSrcNameList->InsertAtIndex(2, JString("-f", kJFalse));
				itsSrcNameList->Append(destPath);
				Start(2);
				}
			}

		if (done)
			{
			JXDeleteObjectTask<JBroadcaster>::Delete(this);
			jdelete destPath;
			}
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 RemoveExecutePermissions (private)

 ******************************************************************************/

void
SyGCopyProcess::RemoveExecutePermissions
	(
	const JString& srcPath,
	const JString& destPath
	)
{
	JString path, name;
	JSplitPathAndName(srcPath, &path, &name);
	path = JCombinePathAndName(destPath, name);

	if (JFileExists(srcPath))
		{
		mode_t p;
		if ((JGetPermissions(path, &p)).OK())
			{
			p &= ~(S_IXUSR | S_IXGRP | S_IXOTH);
			JSetPermissions(path, p);
			}
		return;
		}

	JDirInfo* info;
	if (JDirInfo::Create(srcPath, &info))
		{
		const JSize count = info->GetEntryCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JDirEntry& entry = info->GetEntry(i);
			RemoveExecutePermissions(entry.GetFullName(), path);
			}
		jdelete info;
		}
}
