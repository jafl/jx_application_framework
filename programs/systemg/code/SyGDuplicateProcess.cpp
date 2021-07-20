/******************************************************************************
 SyGDuplicateProcess.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2000 by John Lindal.

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
	auto* p = jnew SyGDuplicateProcess(table, nodeList);
	assert( p != nullptr );
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
	itsProcess(nullptr),
	itsShouldEditFlag(nodeList.GetElementCount() == 1)
{
	itsTable->GetTableSelection().ClearSelection();
	ClearWhenGoingAway(itsTable, &itsTable);

	const JSize count = nodeList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		auto* node = const_cast<SyGFileTreeNode*>(nodeList.GetElement(i));
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
	assert( itsProcess == nullptr );
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
		const auto* info =
			dynamic_cast<const JProcess::Finished*>(&message);
		if (info->Successful() && itsTable != nullptr && !itsTable->IsEditing())
			{
			SyGFileTreeNode* node   = itsNodeList.GetFirstElement();
			SyGFileTreeNode* parent = nullptr;
			if (node != nullptr)
				{
				parent = node->GetSyGParent();
				}

			JPoint cell;
			const bool found = itsTable->SelectName(itsCurrentName, parent, &cell);		// updates table
			if (itsShouldEditFlag && found)
				{
				itsTable->BeginEditing(cell);
				}
			}

		JXDeleteObjectTask<JBroadcaster>::Delete(itsProcess);
		itsProcess = nullptr;
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
		if (sender == itsNodeList.GetElement(i))
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

	const JString* origName = itsFullNameList.GetFirstElement();
	JString path, name, root, suffix;
	JSplitPathAndName(*origName, &path, &name);
	if (JSplitRootAndSuffix(name, &root, &suffix))
		{
		suffix.Prepend(".");
		}

	root += "_copy";
	name  = JGetUniqueDirEntryName(path, root, suffix.GetBytes());

	JSplitPathAndName(name, &path, &itsCurrentName);

	const JUtf8Byte* argv[] = { "cp", "-Rdf", origName->GetBytes(), name.GetBytes(), nullptr };

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
		itsProcess = nullptr;
		itsNodeList.RemoveElement(1);
		ProcessNextFile();
		}
}
