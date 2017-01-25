/******************************************************************************
 GMCopyProcess.cc

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2000 by John Lindal. All rights reserved.

 *****************************************************************************/

#include "GMCopyProcess.h"

#include <JXDeleteObjectTask.h>

#include <JFSFileTree.h>
#include <JSimpleProcess.h>
#include <JTableSelection.h>
#include <JThisProcess.h>
#include <JDirEntry.h>

#include "jXGlobals.h"

#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jProcessUtil.h>

#include <jAssert.h>

// string ID's

static const JCharacter* kAskAutoReplaceID        = "AskAutoReplace::GMCopyProcess";
static const JCharacter* kAskReplaceID            = "AskReplace::GMCopyProcess";
static const JCharacter* kOnlyReplaceSameTypeID   = "OnlyReplaceSameType::GMCopyProcess";
static const JCharacter* kNoMoveIntoSelfID        = "NoMoveIntoSelf::GMCopyProcess";
static const JCharacter* kNoReplaceWithContentsID = "NoReplaceWithContents::GMCopyProcess";

/******************************************************************************
 Constructor function (static)

	srcTable can be NULL.  We take ownership of srcNameList.

 ******************************************************************************/

void
GMCopyProcess::Copy
	(
	JPtrArray<JString>*	srcNameList,
	const JCharacter* dest
	)
{
	if (CleanSrcList(srcNameList, dest))
		{
		GMCopyProcess* p =
			new GMCopyProcess(srcNameList, dest, kJTrue);
		assert( p != NULL );
		}
}

void
GMCopyProcess::Move
	(
	JPtrArray<JString>*	srcNameList,
	const JCharacter* dest
	)
{
	if (CleanSrcList(srcNameList, dest))
		{
		GMCopyProcess* p =
			new GMCopyProcess(srcNameList, dest, kJFalse);
		assert( p != NULL );
		}
}

/******************************************************************************
 CleanSrcList (static private)

	Removes items that would go nowhere or which user doesn't want to
	overwrite.  If nothing is left, deletes the list.

 ******************************************************************************/

JBoolean
GMCopyProcess::CleanSrcList
	(
	JPtrArray<JString>*	srcNameList,
	const JCharacter* dest
	)
{
	JString destPath(dest);
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
GMCopyProcess::ActionIsUseful
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
GMCopyProcess::OKToReplace
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

	const JString cmd = "rm -rf " + JPrepArgForExec(destName);
	
	JString errOutput;
	const JError err = JRunProgram(cmd, &errOutput);
	if (!err.OK())
		{
		(JGetUserNotification())->ReportError(err.GetMessage());
		}

	return err.OK();
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

GMCopyProcess::GMCopyProcess
	(
	JPtrArray<JString>*	srcNameList,
	const JCharacter* 	dest,
	const JBoolean		isCopy
	)
	:
	itsSrcNameList(srcNameList),
	itsDestPath(dest)
{
	assert( !itsSrcNameList->IsEmpty() );

	itsSrcNameList->SetCleanUpAction(JPtrArrayT::kDeleteAll);

	if (isCopy)
		{
		itsSrcNameList->InsertAtIndex(1, "cp");

#ifdef _J_OSX
		itsSrcNameList->InsertAtIndex(2, "-Rf");
#else
		itsSrcNameList->InsertAtIndex(2, "-Rdf");
#endif
		}
	else
		{
		itsSrcNameList->InsertAtIndex(1, "mv");
		itsSrcNameList->InsertAtIndex(2, "-f");
		}

	JAppendDirSeparator(&itsDestPath);
	itsSrcNameList->Append(itsDestPath);

	const JError err = JSimpleProcess::Create(&itsProcess, *itsSrcNameList);

	itsSrcNameList->DeleteElement(1);		// clean up to make Receive simpler
	itsSrcNameList->DeleteElement(1);

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

GMCopyProcess::~GMCopyProcess()
{
	assert( itsProcess == NULL );
	delete itsSrcNameList;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GMCopyProcess::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsProcess && message.Is(JProcess::kFinished))
		{
		JXDeleteObjectTask<JBroadcaster>::Delete(itsProcess);
		itsProcess = NULL;

		JXDeleteObjectTask<JBroadcaster>::Delete(this);
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}
