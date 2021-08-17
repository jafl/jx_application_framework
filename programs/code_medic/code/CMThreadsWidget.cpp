/******************************************************************************
 CMThreadsWidget.cpp

	BASE CLASS = public JXNamedTreeListWidget

	Copyright (C) 2001-11 by John Lindal.

 *****************************************************************************/

#include "CMThreadsWidget.h"
#include "CMThreadNode.h"
#include "CMCommandDirector.h"
#include "CMThreadsDir.h"
#include "CMGetThreads.h"
#include "CMGetThread.h"
#include "cmGlobals.h"
#include <JXWindow.h>
#include <JXDeleteObjectTask.h>
#include <JTree.h>
#include <JNamedTreeList.h>
#include <JTableSelection.h>
#include <JPainter.h>
#include <JFontManager.h>
#include <JListUtil.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CMThreadsWidget::CMThreadsWidget
	(
	CMCommandDirector*	commandDir,
	CMThreadsDir*		threadDir,
	JTree*				tree,
	JNamedTreeList*		treeList,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXNamedTreeListWidget(treeList, scrollbarSet,
						  enclosure, hSizing, vSizing, x, y, w, h),
	itsCommandDir(commandDir),
	itsThreadDir(threadDir),
	itsTree(tree),
	itsNeedsUpdateFlag(false),
	itsIsWaitingForReloadFlag(false),
	itsChangingThreadFlag(false),
	itsSelectingThreadFlag(false),
	itsFlushWhenRunFlag(true),
	itsOpenIDList(nullptr)
{
	itsLink = CMGetLink();
	ListenTo(itsLink);

	itsGetThreadsCmd       = itsLink->CreateGetThreads(itsTree, this);
	itsGetCurrentThreadCmd = itsLink->CreateGetThread(this);

	SetElasticColIndex(0);

	JString name;
	JSize size;
	CMGetPrefsManager()->GetDefaultFont(&name, &size);
	SetFont(name, size);

	ListenTo(&(GetTableSelection()));
	ListenTo(GetWindow());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMThreadsWidget::~CMThreadsWidget()
{
	jdelete itsTree;
	JXDeleteObjectTask<CMGetThreads>::Delete(itsGetThreadsCmd);
	jdelete itsGetCurrentThreadCmd;
	jdelete itsOpenIDList;
}

/******************************************************************************
 SelectThread

 ******************************************************************************/

void
CMThreadsWidget::SelectThread
	(
	const JIndex id
	)
{
	SelectThread1(itsTree->GetRoot(), id);
}

/******************************************************************************
 SelectThread1 (private)

 ******************************************************************************/

bool
CMThreadsWidget::SelectThread1
	(
	const JTreeNode*	root,
	const JIndex		id
	)
{
	const JTreeList* list = GetTreeList();

	const JSize count = root->GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		const auto* node =
			dynamic_cast<const CMThreadNode*>(root->GetChild(i));
		assert( node != nullptr );

		if (node->GetID() == id)
			{
			JIndex j;
			const bool found = list->FindNode(node, &j);
			if (found)
				{
				itsSelectingThreadFlag = true;
				SelectSingleCell(JPoint(GetNodeColIndex(), j));
				itsSelectingThreadFlag = false;
				}

			return true;
			}

		if (SelectThread1(node, id))
			{
			return true;
			}
		}

	return false;
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
CMThreadsWidget::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsIsWaitingForReloadFlag)
		{
		DisplayCursor(kJXBusyCursor);
		}
	else
		{
		JXNamedTreeListWidget::AdjustCursor(pt, modifiers);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CMThreadsWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ClearIncrementalSearchBuffer();

	JPoint cell;
	if (ScrollForWheel(button, modifiers) ||
		!GetCell(pt, &cell))
		{
		return;
		}

	if (JIndex(cell.x) != GetToggleOpenColIndex())
		{
		if (modifiers.meta())
			{
			const JTreeNode* node = GetTreeList()->GetNode(cell.y);
			const auto* threadNode =
				dynamic_cast<const CMThreadNode*>(node);
			assert( threadNode != nullptr );

			JString fileName;
			JIndex lineIndex;
			if (threadNode->GetFile(&fileName, &lineIndex))
				{
				itsCommandDir->OpenSourceFile(fileName, lineIndex);
				}
			else
				{
				JGetUserNotification()->ReportError(JGetString("NoSourceFile::CMThreadsWidget"));
				}
			}
		else
			{
			SelectSingleCell(JPoint(GetNodeColIndex(), cell.y));
			}
		}
	else
		{
		itsSelectingThreadFlag = true;		// ignore selection changes during open/close
		JXNamedTreeListWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		itsSelectingThreadFlag = false;
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
CMThreadsWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsSelectingThreadFlag = true;		// ignore selection changes during open/close
	JXNamedTreeListWidget::HandleMouseUp(pt, button, buttonStates, modifiers);
	itsSelectingThreadFlag = false;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
CMThreadsWidget::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&   modifiers
	)
{
	if (c == kJUpArrow)
		{
		if (!SelectNextThread(-1) && GetRowCount() > 0)
			{
			SelectSingleCell(JPoint(GetNodeColIndex(), GetRowCount()));
			}
		ClearIncrementalSearchBuffer();
		}
	else if (c == kJDownArrow)
		{
		if (!SelectNextThread(+1) && GetRowCount() > 0)
			{
			SelectSingleCell(JPoint(GetNodeColIndex(), 1));
			}
		ClearIncrementalSearchBuffer();
		}
	else
		{
		if (c == kJLeftArrow || c == kJRightArrow)
			{
			itsSelectingThreadFlag = true;		// ignore selection changes during open/close
			}
		JXNamedTreeListWidget::HandleKeyPress(c, keySym, modifiers);
		itsSelectingThreadFlag = false;
		}
}

/******************************************************************************
 SelectNextThread (private)

 ******************************************************************************/

bool
CMThreadsWidget::SelectNextThread
	(
	const JInteger delta
	)
{
	const JTableSelection& s = GetTableSelection();

	JPoint cell;
	if (s.GetFirstSelectedCell(&cell))
		{
		cell.y += delta;
		if (RowIndexValid(cell.y))
			{
			SelectSingleCell(cell);
			}

		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMThreadsWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && message.Is(CMLink::kThreadChanged))
		{
		if (ShouldRebuild())
			{
			itsIsWaitingForReloadFlag = true;
			itsGetCurrentThreadCmd->Send();
			}
		else
			{
			itsNeedsUpdateFlag = true;
			}
		}
	else if (sender == itsLink && message.Is(CMLink::kThreadListChanged))
		{
		itsFlushWhenRunFlag = false;
		if (!itsIsWaitingForReloadFlag)
			{
			SaveOpenNodes();
			Rebuild();
			}
		}

	else if (sender == itsLink &&
			 ((message.Is(CMLink::kProgramRunning) && itsFlushWhenRunFlag) ||
			  message.Is(CMLink::kProgramFinished) ||
			  message.Is(CMLink::kDetachedFromProcess)))
		{
		itsIsWaitingForReloadFlag = false;
		FlushOldData();
		}
	else if (sender == itsLink && message.Is(CMLink::kCoreCleared))
		{
		// When the user has set a breakpoint at the same location as the
		// tbreak for obtaining the PID, then we will only get ProgramStopped
		// before CoreCleared.  We must not discard our data in this case.

		if (!itsLink->IsDebugging())
			{
			FlushOldData();
			}
		}

	else if (sender == itsLink && message.Is(CMLink::kProgramStopped))
		{
		// This is triggered when gdb prints file:line info.
		// (It would be nice to avoid this if only the frame changed,
		//  but we can't merely set a flag when we get kFrameChanged
		//  because we won't always get kProgramStopped afterwards.)

		const bool wasChanging = itsChangingThreadFlag;
		itsChangingThreadFlag      = false;

		if (!wasChanging)
			{
			Rebuild();
			}
		}
	else if (sender == itsLink && message.Is(CMLink::kFrameChanged))
		{
		// We don't need to rebuild our list when we get the next
		// ProgramStopped message.

		itsChangingThreadFlag = true;
		}

	else if (sender == itsLink &&
			 (message.Is(CMLink::kCoreLoaded) ||
			  message.Is(CMLink::kAttachedToProcess)))
		{
		itsNeedsUpdateFlag = true;

//		We can't do this because gdb often reports threads when a core file
//		is loaded, even if the program doesn't use threads.
//
//		CMCheckForThreads* cmd = jnew CMCheckForThreads(itsThreadDir);
//		assert( cmd != nullptr );
//		cmd->Send();
		}

	else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
		{
		Update();
		}

	else
		{
		JTableSelection& s = GetTableSelection();

		JPoint cell;
		if (!itsSelectingThreadFlag &&
			sender == &s && message.Is(JTableData::kRectChanged) &&
			s.GetFirstSelectedCell(&cell))
			{
			const JTreeNode* node = GetTreeList()->GetNode(cell.y);
			const auto* threadNode =
				dynamic_cast<const CMThreadNode*>(node);
			assert( threadNode != nullptr );

			itsChangingThreadFlag = true;
			itsLink->SwitchToThread(threadNode->GetID());
			}

		JXNamedTreeListWidget::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
CMThreadsWidget::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !CMIsShuttingDown())
		{
		itsLink = CMGetLink();
		ListenTo(itsLink);

		itsFlushWhenRunFlag = true;
		FlushOldData();

		jdelete itsGetThreadsCmd;
		itsGetThreadsCmd = itsLink->CreateGetThreads(itsTree, this);

		jdelete itsGetCurrentThreadCmd;
		itsGetCurrentThreadCmd = itsLink->CreateGetThread(this);

		jdelete itsOpenIDList;
		itsOpenIDList = nullptr;
		}
	else
		{
		JXNamedTreeListWidget::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 Update

 ******************************************************************************/

void
CMThreadsWidget::Update()
{
	if ((itsLink->HasCore() || itsLink->ProgramIsStopped()) &&
		itsNeedsUpdateFlag)
		{
		Rebuild();
		}
}

/******************************************************************************
 ShouldRebuild (private)

 ******************************************************************************/

bool
CMThreadsWidget::ShouldRebuild()
	const
{
	return itsThreadDir->IsActive() && !GetWindow()->IsIconified();
}

/******************************************************************************
 Rebuild (private)

 ******************************************************************************/

void
CMThreadsWidget::Rebuild()
{
	if (ShouldRebuild())
		{
		itsIsWaitingForReloadFlag = true;
		itsNeedsUpdateFlag        = false;
		FlushOldData();
		itsGetThreadsCmd->Send();
		}
	else
		{
		itsNeedsUpdateFlag = true;
		}
}

/******************************************************************************
 FlushOldData (private)

 ******************************************************************************/

void
CMThreadsWidget::FlushOldData()
{
	itsSelectingThreadFlag = true;
	itsTree->GetRoot()->DeleteAllChildren();
	itsSelectingThreadFlag = false;

	itsNeedsUpdateFlag    = false;
	itsChangingThreadFlag = false;
}

/******************************************************************************
 SaveOpenNodes (private)

 ******************************************************************************/

void
CMThreadsWidget::SaveOpenNodes()
{
	if (itsOpenIDList == nullptr)
		{
		itsOpenIDList = jnew JArray<JUInt64>();
		assert( itsOpenIDList != nullptr );
		itsOpenIDList->SetCompareFunction(JCompareUInt64);
		}
	else
		{
		itsOpenIDList->RemoveAll();
		}

	itsDisplayState = SaveDisplayState();
	SaveOpenNodes1(itsTree->GetRoot());
}

/******************************************************************************
 SaveOpenNodes1 (private)

 ******************************************************************************/

void
CMThreadsWidget::SaveOpenNodes1
	(
	JTreeNode* root
	)
{
	JTreeList* list = GetTreeList();

	const JSize count = root->GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		JTreeNode* child = root->GetChild(i);
		if (list->IsOpen(child))
			{
			auto* threadNode = dynamic_cast<CMThreadNode*>(child);
			assert( threadNode != nullptr );
			itsOpenIDList->InsertSorted(threadNode->GetID());
			}

		SaveOpenNodes1(child);
		}
}

/******************************************************************************
 RestoreOpenNodes (private)

 ******************************************************************************/

void
CMThreadsWidget::RestoreOpenNodes
	(
	JTreeNode* root
	)
{
	JTreeList* list = GetTreeList();

	const JSize count = root->GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		auto* child = dynamic_cast<CMThreadNode*>(root->GetChild(i));
		JIndex j;
		if (itsOpenIDList->SearchSorted(child->GetID(), JListT::kAnyMatch, &j))
			{
			list->Open(child);
			}

		RestoreOpenNodes(child);
		}
}

/******************************************************************************
 FinishedLoading

 ******************************************************************************/

void
CMThreadsWidget::FinishedLoading
	(
	const JIndex currentID
	)
{
	if (!itsIsWaitingForReloadFlag)		// program exited
		{
		FlushOldData();
		return;
		}

	if (itsOpenIDList != nullptr)
		{
		RestoreOpenNodes(itsTree->GetRoot());
		RestoreDisplayState(itsDisplayState);
		}

	if (currentID > 0)
		{
		SelectThread(currentID);
		}

	itsIsWaitingForReloadFlag = false;
}
