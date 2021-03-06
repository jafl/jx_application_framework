/******************************************************************************
 SyGDuplicateProcess.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2000 by John Lindal. All rights reserved.

 *****************************************************************************/

#include "SyGDuplicateProcess.h"
#include "SyGFileTreeTable.h"
#include <JXDeleteObjectTask.h>
#include <JTableSelection.h>
#include <JSimpleProcess.h>
#include <JThisProcess.h>
#include <JDirEntry.h>
#include <jFileUtil.h>
#include <jVCSUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

void
SyGDuplicateProcess::Duplicate
	(
	SyGFileTreeTable*					table,
	const JPtrArray<SyGFileTreeNode>&	nodeList
	)
{
	SyGDuplicateProcess* p = jnew SyGDuplicateProcess(table, nodeList);
	assert( p != NULL );
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

SyGDuplicateProcess::SyGDuplicateProcess
	(
	SyGFileTreeTable*					table,
	const JPtrArray<SyGFileTreeNode>&	nodeList
	)
	:
	itsTable(table),
	itsNodeList(JPtrArrayT::kForgetAll),
	itsFullNameList(JPtrArrayT::kDeleteAll),
	itsProcess(NULL),
	itsShouldEditFlag(JI2B(nodeList.GetElementCount() == 1))
{
	(itsTable->GetTableSelection()).ClearSelection();
	ClearWhenGoingAway(itsTable, &itsTable);

	const JSize count = nodeList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		SyGFileTreeNode* node = const_cast<SyGFileTreeNode*>(nodeList.NthElement(i));
		itsNodeList.Append(node);
		itsFullNameList.Append((node->GetDirEntry())->GetFullName());
		ListenTo(node);
		}

	ProcessNextFile();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGDuplicateProcess::~SyGDuplicateProcess()
{
	assert( itsProcess == NULL );
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SyGDuplicateProcess::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsProcess && message.Is(JProcess::kFinished))
		{
		const JProcess::Finished* info =
			dynamic_cast<const JProcess::Finished*>(&message);
		if (info->Successful() && itsTable != NULL && !itsTable->IsEditing())
			{
			SyGFileTreeNode* node   = itsNodeList.FirstElement();
			SyGFileTreeNode* parent = NULL;
			if (node != NULL)
				{
				parent = node->GetSyGParent();
				}

			JPoint cell;
			const JBoolean found = itsTable->SelectName(itsCurrentName, parent, &cell);		// updates table
			if (itsShouldEditFlag && found)
				{
				itsTable->BeginEditing(cell);
				}
			}

		JXDeleteObjectTask<JBroadcaster>::Delete(itsProcess);
		itsProcess = NULL;
		itsNodeList.RemoveElement(1);
		ProcessNextFile();
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
SyGDuplicateProcess::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	JIndex nodeIndex  = 0;
	const JSize count = itsNodeList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (sender == itsNodeList.NthElement(i))
			{
			nodeIndex = i;
			break;
			}
		}

	if (nodeIndex > 0)
		{
		itsNodeList.SetToNull(nodeIndex, JPtrArrayT::kForget);
		}
	else
		{
		JBroadcaster::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 ProcessNextFile (private)

 ******************************************************************************/

void
SyGDuplicateProcess::ProcessNextFile()
{
	if (itsFullNameList.IsEmpty())
		{
		JXDeleteObjectTask<JBroadcaster>::Delete(this);
		return;
		}

	const JString* origName = itsFullNameList.FirstElement();
	JString path, name, root, suffix;
	JSplitPathAndName(*origName, &path, &name);
	if (JSplitRootAndSuffix(name, &root, &suffix))
		{
		suffix.PrependCharacter('.');
		}

	root += "_copy";
	name  = JGetUniqueDirEntryName(path, root, suffix);

	JSplitPathAndName(name, &path, &itsCurrentName);

	const JCharacter* argv[] = { "cp", "-Rdf", *origName, name, NULL };

	JVCSType type;
	if (JIsManagedByVCS(*origName, &type) && type == kJSVNType)
		{
		argv[0] = "svn";
		argv[1] = "cp";
		}

	const JError err = JSimpleProcess::Create(&itsProcess, argv, sizeof(argv));
	err.ReportIfError();
	itsFullNameList.DeleteElement(1);		// before ProcessNextFile()

	if (err.OK())
		{
		ListenTo(itsProcess);
		JThisProcess::Ignore(itsProcess);	// detach so it always finishes
		}
	else
		{
		itsProcess = NULL;
		itsNodeList.RemoveElement(1);
		ProcessNextFile();
		}
}
