/******************************************************************************
 SyGCopyProcess.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright © 2000 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <SyGStdInc.h>
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
#include <jVCSUtil.h>
#include <jMountUtil.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kAskAutoReplaceID        = "AskAutoReplace::SyGCopyProcess";
static const JCharacter* kAskReplaceID            = "AskReplace::SyGCopyProcess";
static const JCharacter* kOnlyReplaceSameTypeID   = "OnlyReplaceSameType::SyGCopyProcess";
static const JCharacter* kNoMoveIntoSelfID        = "NoMoveIntoSelf::SyGCopyProcess";
static const JCharacter* kNoReplaceWithContentsID = "NoReplaceWithContents::SyGCopyProcess";
static const JCharacter* kMixedVCSMoveID          = "MixedVCSMove::SyGCopyProcess";
static const JCharacter* kSomeVCSMoveID           = "SomeVCSMove::SyGCopyProcess";
static const JCharacter* kNoTargetVCSMoveID       = "NoTargetVCSMove::SyGCopyProcess";

/******************************************************************************
 Constructor function (static)

	srcTable can be NULL.  We take ownership of srcNameList.

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
			new SyGCopyProcess(srcTable, srcNameList, destTable, destNode, kJTrue);
		assert( p != NULL );
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
			new SyGCopyProcess(srcTable, srcNameList, destTable, destNode, kJFalse);
		assert( p != NULL );
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
		const JString* srcName = srcNameList->NthElement(i);
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
		delete srcNameList;
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
	const JCharacter*	srcName,
	const JCharacter*	destPath,
	JString*			destName
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
	const JCharacter*	srcName,
	const JCharacter*	destName,
	JBoolean*			ask,
	JBoolean*			first
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
				const JCharacter* map[] =
					{
					"src", name.GetCString()
					};
				const JString msg = JGetString(kNoMoveIntoSelfID, map, sizeof(map));
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
		const JCharacter* map[] =
			{
			"src_type",  (srcIsDir  ? "directory" : "file"),
			"dest_type", (destIsDir ? "directory" : "file"),
			"dest",      name.GetCString()
			};
		const JString msg = JGetString(kOnlyReplaceSameTypeID, map, sizeof(map));
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
			const JCharacter* map[] =
				{
				"dest", name.GetCString()
				};
			const JString msg = JGetString(kNoReplaceWithContentsID, map, sizeof(map));
			(JGetUserNotification())->ReportError(msg);
			return kJFalse;
			}
		else if (trueDest.BeginsWith(trueSrc))
			{
			const JCharacter* map[] =
				{
				"src", name.GetCString()
				};
			const JString msg = JGetString(kNoMoveIntoSelfID, map, sizeof(map));
			(JGetUserNotification())->ReportError(msg);
			return kJFalse;
			}
		}

	if (*first)
		{
		*ask   = !(JGetUserNotification())->AskUserYes(JGetString(kAskAutoReplaceID));
		*first = kJFalse;
		}

	if (*ask)
		{
		const JCharacter* map[] =
			{
			"name", name.GetCString()
			};
		const JString msg = JGetString(kAskReplaceID, map, sizeof(map));
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
	itsProcess(NULL)
{
	assert( !itsSrcNameList->IsEmpty() );
	itsSrcNameList->SetCleanUpAction(JPtrArrayT::kDeleteAll);

	JString destPath = (itsDestNode->GetDirEntry())->GetFullName();
	JAppendDirSeparator(&destPath);

	JVCSType vcsType = kJUnknownVCSType;
	if (!isCopy)
		{
		JVCSType type1, type2;
		JBoolean anyVCS  = JIsManagedByVCS(*(srcNameList->FirstElement()), &type1);
		JBoolean allVCS  = anyVCS;
		JBoolean sameVCS = anyVCS;

		const JSize srcCount = srcNameList->GetElementCount();
		for (JIndex i=2; i<=srcCount; i++)
			{
			if (JIsManagedByVCS(*(srcNameList->NthElement(i)), &type2))
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
			if (type1 == type2)
				{
				vcsType = type1;
				}
			else if ((type2 == kJUnknownVCSType && !(JGetUserNotification())->AskUserNo(JGetString(kNoTargetVCSMoveID))) ||
					 (type2 != kJUnknownVCSType && !(JGetUserNotification())->AskUserNo(JGetString(kMixedVCSMoveID))))
				{
				JXDeleteObjectTask<JBroadcaster>::Delete(this);
				return;
				}
			}
		else if ((allVCS && !(JGetUserNotification())->AskUserNo(JGetString(kMixedVCSMoveID))) ||
				 (anyVCS && !(JGetUserNotification())->AskUserNo(JGetString(kSomeVCSMoveID))))
			{
			JXDeleteObjectTask<JBroadcaster>::Delete(this);
			return;
			}
		}

	if (itsSrcTable != NULL)
		{
		(itsSrcTable->GetTableSelection()).ClearSelection();
		ListenTo(itsSrcTable);
		}

	(itsDestTable->GetTableSelection()).ClearSelection();
	ListenTo(itsDestTable);
	ListenTo(itsDestNode);

	JSize prefixCount = 0;
	if (isCopy)
		{
		prefixCount = 2;
		itsSrcNameList->InsertAtIndex(1, "cp");
		itsSrcNameList->InsertAtIndex(2, "-Rdf");
		}
	else if (vcsType == kJSVNType)
		{
		prefixCount = 3;
		itsSrcNameList->InsertAtIndex(1, "svn");
		itsSrcNameList->InsertAtIndex(2, "mv");
		itsSrcNameList->InsertAtIndex(3, "--force");
		}
	else if (vcsType == kJGitType)
		{
		prefixCount = 3;
		itsSrcNameList->InsertAtIndex(1, "git");
		itsSrcNameList->InsertAtIndex(2, "mv");
		itsSrcNameList->InsertAtIndex(3, "-f");
		}
	else
		{
		prefixCount = 2;
		itsSrcNameList->InsertAtIndex(1, "mv");
		itsSrcNameList->InsertAtIndex(2, "-f");
		}

	itsSrcNameList->Append(destPath);

	const JError err = JSimpleProcess::Create(&itsProcess, *itsSrcNameList);

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
 Destructor

 ******************************************************************************/

SyGCopyProcess::~SyGCopyProcess()
{
	assert( itsProcess == NULL );
	delete itsSrcNameList;
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
		JString* destPath = itsSrcNameList->LastElement();
		itsSrcNameList->RemoveElement(itsSrcNameList->GetElementCount());

		const JProcess::Finished* info =
			dynamic_cast(const JProcess::Finished*, &message);
		if (info->Successful())
			{
			if (itsSrcTable != NULL)
				{
				itsSrcTable->UpdateDisplay(kJTrue);
				}

			JBoolean selectName = JI2B(itsDestNode != NULL);
			assert( selectName == JI2B(itsDestTable != NULL) );

			if (selectName && itsDestTable->IsEditing())
				{
				itsDestTable->UpdateDisplay(kJTrue);
				selectName = kJFalse;
				}

			const JSize count = itsSrcNameList->GetElementCount();
			JString path, name, oldName, newName, device, fsType;
			for (JIndex i=1; i<=count; i++)
				{
				JString* fullName = itsSrcNameList->NthElement(i);
				JStripTrailingDirSeparator(fullName);
				JSplitPathAndName(*fullName, &path, &name);

				JBoolean writable, isTop;
				if (JIsMounted(*fullName, &writable, &isTop, &device, &fsType) &&
					JStringCompare(fsType, "vfat", kJFalse) == 0)
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
					oldName = *(itsSrcNameList->NthElement(i));
					JAppendDirSeparator(&oldName);
					JAppendDirSeparator(&newName);
					JFSFileTree::DirectoryRenamed msg(oldName, newName);
					(SyGGetApplication())->DirectoryRenamed(msg);
					}
				}
			}

		JXDeleteObjectTask<JBroadcaster>::Delete(itsProcess);
		itsProcess = NULL;

		JXDeleteObjectTask<JBroadcaster>::Delete(this);
		delete destPath;
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
	const JCharacter* srcPath,
	const JCharacter* destPath
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
		delete info;
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
SyGCopyProcess::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsSrcTable)
		{
		itsSrcTable = NULL;
		}
	else if (sender == itsDestTable)
		{
		itsDestTable = NULL;
		itsDestNode  = NULL;
		}
	else if (sender == itsDestNode)
		{
		itsDestNode = NULL;
		}
	else
		{
		JBroadcaster::ReceiveGoingAway(sender);
		}
}
