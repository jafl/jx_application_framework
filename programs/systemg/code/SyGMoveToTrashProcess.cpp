/******************************************************************************
 SyGMoveToTrashProcess.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright © 2000 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <SyGStdInc.h>
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

	table can be NULL.  We take ownership of fullNameList.

 ******************************************************************************/

void
SyGMoveToTrashProcess::Move
	(
	SyGFileTreeTable*	table,
	JPtrArray<JString>*	fullNameList
	)
{
	JString trashDir;
	if (SyGGetTrashDirectory(&trashDir, kJFalse))
		{
		SyGMoveToTrashProcess* p = new SyGMoveToTrashProcess(table, fullNameList, trashDir);
		assert( p != NULL );
		}
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

SyGMoveToTrashProcess::SyGMoveToTrashProcess
	(
	SyGFileTreeTable*	table,
	JPtrArray<JString>*	fullNameList,
	const JCharacter*	trashDir
	)
	:
	itsTable(table),
	itsFullNameList(fullNameList),
	itsTrashDir(trashDir),
	itsProcess(NULL)
{
	if (itsTable != NULL)
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
	if (itsTable != NULL)
		{
		itsTable->UpdateDisplay(kJTrue);
		}

	SyGUpdateTrash();

	assert( itsProcess == NULL );
	delete itsFullNameList;
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
		itsProcess = NULL;
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

	JString* origName = itsFullNameList->FirstElement();
	JStripTrailingDirSeparator(origName);
	JString path, name;
	JSplitPathAndName(*origName, &path, &name);

	const JString newName = JCombinePathAndName(itsTrashDir, name);
	if (JNameUsed(newName))
		{
		JString root, suffix;
		if (JSplitRootAndSuffix(name, &root, &suffix))
			{
			suffix.PrependCharacter('.');
			}
		const JString newName2 = JGetUniqueDirEntryName(itsTrashDir, root, suffix);
		JRenameDirEntry(newName, newName2);
		}

	const JCharacter* argv[] = { "mv", "-f", *origName, newName, NULL };

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
		itsProcess = NULL;
		ProcessNextFile();
		}
}
