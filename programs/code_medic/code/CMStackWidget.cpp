/******************************************************************************
 CMStackWidget.cpp

	BASE CLASS = public JXNamedTreeListWidget

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "CMStackWidget.h"
#include "CMStackFrameNode.h"
#include "CMStackArgNode.h"
#include "CMCommandDirector.h"
#include "CMStackDir.h"
#include "CMGetStack.h"
#include "CMGetFrame.h"
#include "cmGlobals.h"
#include <JXWindow.h>
#include <JTree.h>
#include <JNamedTreeList.h>
#include <JTableSelection.h>
#include <JPainter.h>
#include <JFontManager.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

const JIndex kArgValueColIndex = 3;
const JSize kIndentWidth       = 4;	// characters: "xx:"

/******************************************************************************
 Constructor

 *****************************************************************************/

CMStackWidget::CMStackWidget
	(
	CMCommandDirector*	commandDir,
	CMStackDir*			stackDir,
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
	itsStackDir(stackDir),
	itsTree(tree),
	itsNeedsUpdateFlag(false),
	itsSmartFrameSelectFlag(false),
	itsIsWaitingForReloadFlag(false),
	itsChangingFrameFlag(false),
	itsSelectingFrameFlag(false)
{
	itsLink = CMGetLink();
	ListenTo(itsLink);

	itsGetStackCmd = itsLink->CreateGetStack(itsTree, this);
	itsGetFrameCmd = itsLink->CreateGetFrame(this);

	AppendCols(1);	// argument values

	SetElasticColIndex(0);

	JString name;
	JSize size;
	CMGetPrefsManager()->GetDefaultFont(&name, &size);
	SetFont(name, size);

	SetIndentWidth(kIndentWidth * GetFont().GetCharWidth(GetFontManager(), JUtf8Character('0')));

	ListenTo(&(GetTableSelection()));
	ListenTo(GetWindow());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMStackWidget::~CMStackWidget()
{
	jdelete itsTree;
	jdelete itsGetStackCmd;
	jdelete itsGetFrameCmd;
}

/******************************************************************************
 GetStackFrame

 ******************************************************************************/

bool
CMStackWidget::GetStackFrame
	(
	const JUInt64				id,
	const CMStackFrameNode**	frame
	)
{
	const JTreeNode* root = itsTree->GetRoot();
	const JSize count     = root->GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		const auto* node =
			dynamic_cast<const CMStackFrameNode*>(root->GetChild(i));
		assert( node != nullptr );

		if (node->GetID() == id)
			{
			*frame = node;
			return true;
			}
		}

	*frame = nullptr;
	return false;
}

/******************************************************************************
 SelectFrame

 ******************************************************************************/

void
CMStackWidget::SelectFrame
	(
	const JUInt64 id
	)
{
	const JTreeList* list = GetTreeList();
	const JTreeNode* root = itsTree->GetRoot();

	const JSize count = root->GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		const auto* node =
			dynamic_cast<const CMStackFrameNode*>(root->GetChild(i));
		assert( node != nullptr );

		if (node->GetID() == id)
			{
			JIndex j;
			const bool found = list->FindNode(node, &j);
			assert( found );

			itsSelectingFrameFlag = true;
			SelectSingleCell(JPoint(GetNodeColIndex(), j));
			itsSelectingFrameFlag = false;

			break;
			}
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
CMStackWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	if (JIndex(cell.x) == GetToggleOpenColIndex() ||
		JIndex(cell.x) == GetNodeColIndex())
		{
		JXNamedTreeListWidget::TableDrawCell(p,cell,rect);
		}
	else if (JIndex(cell.x) == kArgValueColIndex)
		{
		const JPoint fakeCell(GetNodeColIndex(), cell.y);
		HilightIfSelected(p, fakeCell, rect);

		const JTreeNode* node = GetTreeList()->GetNode(cell.y);
		if (node->GetDepth() > 1)
			{
			JFont font = GetFont();
			font.SetStyle(GetCellStyle(cell));
			p.SetFont(font);

			const auto* argNode =
				dynamic_cast<const CMStackArgNode*>(node);
			assert( argNode != nullptr );
			p.String(rect, argNode->GetValue(), JPainter::kHAlignLeft, JPainter::kVAlignCenter);
			}
		}
}

/******************************************************************************
 GetMinCellWidth (virtual protected)

 ******************************************************************************/

JSize
CMStackWidget::GetMinCellWidth
	(
	const JPoint& cell
	)
	const
{
	if (JIndex(cell.x) > GetNodeColIndex())
		{
		const JTreeNode* node = GetTreeList()->GetNode(cell.y);
		if (node->GetDepth() > 1)
			{
			const auto* argNode =
				dynamic_cast<const CMStackArgNode*>(node);
			assert( argNode != nullptr );

			return GetFont().GetStringWidth(GetFontManager(), argNode->GetValue());
			}
		else
			{
			return 0;
			}
		}
	else
		{
		return JXNamedTreeListWidget::GetMinCellWidth(cell);
		}
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
CMStackWidget::AdjustCursor
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
CMStackWidget::HandleMouseDown
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
			while (node->GetDepth() > 1)
				{
				node = node->GetParent();
				}

			const auto* stackNode =
				dynamic_cast<const CMStackFrameNode*>(node);
			assert( stackNode != nullptr );

			JString fileName;
			JIndex lineIndex;
			if (stackNode->GetFile(&fileName, &lineIndex))
				{
				itsCommandDir->OpenSourceFile(fileName, lineIndex);
				}
			else
				{
				JGetUserNotification()->ReportError(JGetString("NoSourceFile::CMStackWidget"));
				}
			}
		else
			{
			SelectSingleCell(JPoint(GetNodeColIndex(), cell.y));
			}
		}
	else
		{
		itsSelectingFrameFlag = true;		// ignore selection changes during open/close
		JXNamedTreeListWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		itsSelectingFrameFlag = false;
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
CMStackWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsSelectingFrameFlag = true;		// ignore selection changes during open/close
	JXNamedTreeListWidget::HandleMouseUp(pt, button, buttonStates, modifiers);
	itsSelectingFrameFlag = false;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
CMStackWidget::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&   modifiers
	)
{
	if (c == kJUpArrow)
		{
		if (!SelectNextFrame(-1) && GetRowCount() > 0)
			{
			SelectSingleCell(JPoint(GetNodeColIndex(), GetRowCount()));
			}
		ClearIncrementalSearchBuffer();
		}
	else if (c == kJDownArrow)
		{
		if (!SelectNextFrame(+1) && GetRowCount() > 0)
			{
			SelectSingleCell(JPoint(GetNodeColIndex(), 1));
			}
		ClearIncrementalSearchBuffer();
		}
	else
		{
		if (c == kJLeftArrow || c == kJRightArrow)
			{
			itsSelectingFrameFlag = true;		// ignore selection changes during open/close
			}
		JXNamedTreeListWidget::HandleKeyPress(c, keySym, modifiers);
		itsSelectingFrameFlag = false;
		}
}

/******************************************************************************
 SelectNextFrame (private)

 ******************************************************************************/

bool
CMStackWidget::SelectNextFrame
	(
	const JInteger delta
	)
{
	const JTableSelection& s = GetTableSelection();
	JNamedTreeList* treeList = GetNamedTreeList();

	JPoint cell;
	if (s.GetFirstSelectedCell(&cell))
		{
		const JTreeNode* node   = treeList->GetNode(cell.y);
		const JTreeNode* parent = node->GetParent();

		JIndex i;
		bool found = parent->FindChild(node, &i);
		assert( found );

		i += delta;
		if (parent->ChildIndexValid(i))
			{
			node  = parent->GetChild(i);
			found = treeList->FindNode(node, &i);
			assert( found );

			SelectSingleCell(JPoint(GetNodeColIndex(), i));
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
CMStackWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && message.Is(CMLink::kFrameChanged))
		{
		if (ShouldRebuild())
			{
			itsGetFrameCmd->Send();
			}
		else
			{
			itsNeedsUpdateFlag = true;
			}
		}
	else if (sender == itsLink && message.Is(CMLink::kThreadChanged))
		{
		Rebuild();
		}

	else if (sender == itsLink &&
			 (message.Is(CMLink::kProgramRunning)  ||
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

		const bool wasChanging = itsChangingFrameFlag;
		itsChangingFrameFlag       = false;

		if (!wasChanging &&
			itsGetFrameCmd->GetState() == CMCommand::kUnassigned)
			{
			itsSmartFrameSelectFlag = true;
			Rebuild();
			}
		}

	else if (sender == itsLink &&
			 (message.Is(CMLink::kCoreLoaded) ||
			  message.Is(CMLink::kAttachedToProcess)))
		{
		itsNeedsUpdateFlag      = true;
		itsSmartFrameSelectFlag = true;
		itsStackDir->Activate();
		}

	else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
		{
		Update();
		}

	else
		{
		JTableSelection& s = GetTableSelection();

		JPoint cell;
		if (!itsSelectingFrameFlag &&
			sender == &s && message.Is(JTableData::kRectChanged) &&
			s.GetFirstSelectedCell(&cell))
			{
			const JTreeNode* node = GetTreeList()->GetNode(cell.y);
			if (node->GetDepth() > 1)
				{
				itsCommandDir->DisplayExpression(
					(GetNamedTreeList()->GetNamedNode(cell.y))->GetName());
				node = node->GetParent();
				}

			const auto* stackNode =
				dynamic_cast<const CMStackFrameNode*>(node);
			assert( stackNode != nullptr );

			SwitchToFrame(stackNode->GetID());
			}

		JXNamedTreeListWidget::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
CMStackWidget::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !CMIsShuttingDown())
		{
		itsLink = CMGetLink();
		ListenTo(itsLink);

		FlushOldData();

		jdelete itsGetStackCmd;
		itsGetStackCmd = itsLink->CreateGetStack(itsTree, this);

		jdelete itsGetFrameCmd;
		itsGetFrameCmd = itsLink->CreateGetFrame(this);
		}
	else
		{
		JXNamedTreeListWidget::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 SwitchToFrame

 ******************************************************************************/

void
CMStackWidget::SwitchToFrame
	(
	const JUInt64 id
	)
{
	itsChangingFrameFlag = true;
	itsLink->SwitchToFrame(id);
}

/******************************************************************************
 Update

 ******************************************************************************/

void
CMStackWidget::Update()
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
CMStackWidget::ShouldRebuild()
	const
{
	return itsStackDir->IsActive() && !GetWindow()->IsIconified();
}

/******************************************************************************
 Rebuild (private)

 ******************************************************************************/

void
CMStackWidget::Rebuild()
{
	if (ShouldRebuild())
		{
		itsIsWaitingForReloadFlag = true;
		FlushOldData();
		itsGetStackCmd->Send();		// need stack before selecting frame
		if (!itsSmartFrameSelectFlag)
			{
			itsGetFrameCmd->Send();
			}
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
CMStackWidget::FlushOldData()
{
	itsSelectingFrameFlag = true;
	itsTree->GetRoot()->DeleteAllChildren();
	itsSelectingFrameFlag = false;

	itsNeedsUpdateFlag   = false;
	itsChangingFrameFlag = false;
}

/******************************************************************************
 FinishedLoading

 ******************************************************************************/

void
CMStackWidget::FinishedLoading
	(
	const JIndex initID
	)
{
	if (!itsIsWaitingForReloadFlag)		// program exited
		{
		FlushOldData();
		return;
		}

	SelectFrame(initID);
	if (itsSmartFrameSelectFlag && initID > 0)
		{
		itsLink->SwitchToFrame(initID);
		}
	itsIsWaitingForReloadFlag = false;
	itsSmartFrameSelectFlag   = false;
}
