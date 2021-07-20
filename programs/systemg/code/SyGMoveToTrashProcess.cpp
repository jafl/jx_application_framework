/******************************************************************************
 SyGMoveToTrashProcess.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2000 by John Lindal.

 *****************************************************************************/

#include "SyGMoveToTrashProcess.h"
#include "SyGFileTreeTable.h"
#include "SyGGlobals.h"
#include <JXDeleteObjectTask.h>
#include <JSimpleProcess.h>
#include <JThisProcess.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

	table can be nullptr.  We take ownership of fullNameList.

 ******************************************************************************/

void
SyGMoveToTrashProcess::Move
	(
	SyGFileTreeTable*	table,
	JPtrArray<JString>*	fullNameList
	)
{
	JString trashDir;
	if (SyGGetTrashDirectory(&trashDir, false))
		{
		auto* p = jnew SyGMoveToTrashProcess(table, fullNameList, trashDir);
		assert( p != nullptr );
		}
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

SyGMoveToTrashProcess::SyGMoveToTrashProcess
	(
	SyGFileTreeTable*	table,
	JPtrArray<JString>*	fullNameList,
	const JString&		trashDir
	)
	:
	itsTable(table),
	itsFullNameList(fullNameList),
	itsTrashDir(trashDir),
	itsProcess(nullptr)
{
	if (itsTable != nullptr)
		{
		ClearWhenGoingAway(itsTable, &itsTable);
		}

	itsFullNameList->SetCleanUpAction(JPtrArrayT::kDeleteAll);
	ProcessNextFile();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGMoveToTrashProcess::~SyGMoveToTrashProcess()
{
	if (itsTable != nullptr)
		{
		itsTable->UpdateDisplay(true);
		}

	SyGUpdateTrash();

	assert( itsProcess == nullptr );
	jdelete itsFullNameList;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SyGMoveToTrashProcess::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsProcess && message.Is(JProcess::kFinished))
		{
		JXDeleteObjectTask<JBroadcaster>::Delete(itsProcess);
		itsProcess = nullptr;
		ProcessNextFile();
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 ProcessNextFile (private)

 ******************************************************************************/

void
SyGMoveToTrashProcess::ProcessNextFile()
{
	if (itsFullNameList->IsEmpty())
		{
		JXDeleteObjectTask<JBroadcaster>::Delete(this);
		return;
		}

	JString* origName = itsFullNameList->GetFirstElement();
	JStripTrailingDirSeparator(origName);
	JString path, name;
	JSplitPathAndName(*origName, &path, &name);

	const JString newName = JCombinePathAndName(itsTrashDir, name);
	if (JNameUsed(newName))
		{
		JString root, suffix;
		if (JSplitRootAndSuffix(name, &root, &suffix))
			{
			suffix.Prepend(".");
			}
		const JString newName2 = JGetUniqueDirEntryName(itsTrashDir, root, suffix.GetBytes());
		JRenameDirEntry(newName, newName2);
		}

	const JUtf8Byte* argv[] = { "mv", "-f", origName->GetBytes(), newName.GetBytes(), nullptr };

	const JError err = JSimpleProcess::Create(&itsProcess, argv, sizeof(argv));
	err.ReportIfError();
	itsFullNameList->DeleteElement(1);		// before ProcessNextFile()

	if (err.OK())
		{
		ListenTo(itsProcess);
		JThisProcess::Ignore(itsProcess);	// detach so it always finishes
		}
	else
		{
		itsProcess = nullptr;
		ProcessNextFile();
		}
}
