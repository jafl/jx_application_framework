/******************************************************************************
 CMVarTreeWidget.cpp

	BASE CLASS = public JXNamedTreeListWidget

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "CMVarTreeWidget.h"
#include "CMVarNode.h"
#include "CMCommandDirector.h"
#include "cbmUtil.h"
#include "cmGlobals.h"
#include "cmActionDefs.h"
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXInputField.h>
#include <JXSelectionManager.h>
#include <JXTextSelection.h>
#include <JXColorManager.h>
#include <JTree.h>
#include <JNamedTreeList.h>
#include <JTableSelection.h>
#include <JPainter.h>
#include <JFontManager.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

const JIndex kValueColIndex = 3;
const JCoordinate kHMargin  = 5;
const JSize kIndentWidth    = 2;	// characters

// Edit menu additions

static const JUtf8Byte* kEditMenuAddStr =
	"  Copy complete name  %k Meta-Shift-C  %i" kCMCopyFullVarNameAction
	"| Copy value          %k Ctrl-C        %i" kCMCopyVarValueAction;

// Base menu

static const JUtf8Byte* kBaseMenuStr =
	"    Default %r"
	"%l| 10      %r"
	"%l| 2       %r"
	"  | 8       %r"
	"  | 16      %r"
	"%l| ASCII   %r";

enum
{
	kDefaultBase = 1,
	kBase10,
	kBase2,
	kBase8,
	kBase16,
	kBaseASCII
};

static const JSize kMenuIndexToBase[] = { 0, 10, 2, 8, 16, 1 };
const JSize kBaseCount                = sizeof(kMenuIndexToBase) / sizeof(JSize);

/******************************************************************************
 Constructor

 *****************************************************************************/

CMVarTreeWidget::CMVarTreeWidget
	(
	CMCommandDirector*	dir,
	const bool		mainDisplay,
	JXMenuBar*			menuBar,
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
	itsDir(dir),
	itsTree(tree),
	itsIsMainDisplayFlag(mainDisplay),
	itsWaitingForReloadFlag(false),
	itsDragType(kInvalidDrag)
{
	SetSelectionBehavior(true, true);

	// custom Edit menu

	JXTEBase* te         = GetEditMenuHandler();
	itsEditMenu          = te->AppendEditMenu(menuBar);
	const bool found = te->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &itsCopyPathCmdIndex);
	assert( found );
	itsCopyPathCmdIndex++;
	itsCopyValueCmdIndex = itsCopyPathCmdIndex+1;
	itsEditMenu->InsertMenuItems(itsCopyPathCmdIndex, kEditMenuAddStr, "CMVarTreeWidget");
	ListenTo(itsEditMenu);

	// Base conversion menus

	itsBaseMenu = menuBar->AppendTextMenu(JGetString("BaseMenuTitle::CMVarTreeWidget"));
	itsBaseMenu->SetMenuItems(kBaseMenuStr, "CMVarTreeWidget");
	itsBaseMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsBaseMenu);

	itsBasePopupMenu =
		jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10, 10);
	assert( itsBasePopupMenu != nullptr );
	itsBasePopupMenu->SetMenuItems(kBaseMenuStr, "CMVarTreeWidget");
	itsBasePopupMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsBasePopupMenu->SetToHiddenPopupMenu();
	ListenTo(itsBasePopupMenu);

	AppendCols(1);
	SetElasticColIndex(0);

	JString name;
	JSize size;
	CMGetPrefsManager()->GetDefaultFont(&name, &size);
	SetFont(name, size);

	SetIndentWidth(kIndentWidth * GetFont().GetCharWidth(GetFontManager(), JUtf8Character('0')));

	ListenTo(CMGetLink());
	ListenTo(&GetTableSelection());

	if (mainDisplay)
		{
		ListenTo(GetWindow());
		}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMVarTreeWidget::~CMVarTreeWidget()
{
	jdelete itsTree;
}

/******************************************************************************
 NewExpression

 ******************************************************************************/

CMVarNode*
CMVarTreeWidget::NewExpression
	(
	const JString& expr
	)
{
	CMVarNode* node =
		CMGetLink()->CreateVarNode(GetTreeList()->GetTree()->GetRoot(),
								   expr, expr, JString::empty);
	assert( node != nullptr );
	if (expr.IsEmpty())
		{
		node->SetName(JString::empty);		// compensate for ctor using " " instead of ""
		}

	JIndex i;
	const bool found = GetNamedTreeList()->FindNode(node, &i);
	assert( found );

	const JPoint cell(GetNodeColIndex(), i);
	BeginEditing(cell);
	itsEditingNewNodeFlag = true;

	ClearIncrementalSearchBuffer();
	return node;
}

/******************************************************************************
 DisplayExpression

 ******************************************************************************/

CMVarNode*
CMVarTreeWidget::DisplayExpression
	(
	const JString& expr
	)
{
	CMVarNode* node = CMGetLink()->CreateVarNode(itsTree->GetRoot(), expr, expr, JString::empty);
	assert ( node != nullptr );
	ShowNode(node);
	return node;
}

/******************************************************************************
 ShowNode

 ******************************************************************************/

void
CMVarTreeWidget::ShowNode
	(
	const CMVarNode* node
	)
{
	JIndex i;
	if (GetTreeList()->FindNode(node, &i))
		{
		SelectSingleCell(JPoint(GetNodeColIndex(), i));
		ClearIncrementalSearchBuffer();
		}
}

/******************************************************************************
 WatchExpression

 ******************************************************************************/

void
CMVarTreeWidget::WatchExpression()
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
		{
		auto* node =
			dynamic_cast<CMVarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		expr = node->GetFullName();
		CMGetLink()->WatchExpression(expr);
		}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 WatchLocation

 ******************************************************************************/

void
CMVarTreeWidget::WatchLocation()
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
		{
		auto* node =
			dynamic_cast<CMVarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		expr = node->GetFullName();
		CMGetLink()->WatchLocation(expr);
		}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 DisplayAsCString

 ******************************************************************************/

void
CMVarTreeWidget::DisplayAsCString()
{
	JPtrArray<JString> exprList(JPtrArrayT::kDeleteAll);

	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
		{
		auto* node =
			dynamic_cast<CMVarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		expr = node->GetFullName();
		expr.Prepend("(char*)(");
		expr.Append(")");
		exprList.Append(expr);
		}

	// DisplayExpression() messes with selection

	const JSize count = exprList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		itsDir->DisplayExpression(*(exprList.GetElement(i)));
		}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 Display1DArray

 ******************************************************************************/

void
CMVarTreeWidget::Display1DArray()
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
		{
		auto* node =
			dynamic_cast<CMVarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		expr = node->GetFullName();
		itsDir->Display1DArray(expr);
		}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 Plot1DArray

 ******************************************************************************/

void
CMVarTreeWidget::Plot1DArray()
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
		{
		auto* node =
			dynamic_cast<CMVarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		expr = node->GetFullName();
		itsDir->Plot1DArray(expr);
		}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 Display2DArray

 ******************************************************************************/

void
CMVarTreeWidget::Display2DArray()
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
		{
		auto* node =
			dynamic_cast<CMVarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		expr = node->GetFullName();
		itsDir->Display2DArray(expr);
		}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 ExamineMemory

 ******************************************************************************/

void
CMVarTreeWidget::ExamineMemory
	(
	const CMMemoryDir::DisplayType type
	)
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	JString expr;
	CMMemoryDir* dir = nullptr;
	while (iter.Next(&cell))
		{
		auto* node =
			dynamic_cast<CMVarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		expr = node->GetFullName();

		dir = jnew CMMemoryDir(itsDir, expr);
		assert(dir != nullptr);
		dir->SetDisplayType(type);
		dir->Activate();
		}

	if (dir == nullptr)
		{
		dir = jnew CMMemoryDir(itsDir, JString::empty);
		assert(dir != nullptr);
		dir->SetDisplayType(type);
		dir->Activate();
		}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CMVarTreeWidget::ReadSetup
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	auto* root = dynamic_cast<CMVarNode*>(itsTree->GetRoot());
	assert( root != nullptr );

	root->DeleteAllChildren();

	JSize count;
	input >> count;

	JString expr;
	JSize base;
	for (JIndex i=1; i<=count; i++)
		{
		input >> expr >> base;

		CMVarNode* node = DisplayExpression(expr);
		node->SetBase(base);
		}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CMVarTreeWidget::WriteSetup
	(
	std::ostream& output
	)
	const
{
	auto* root = dynamic_cast<CMVarNode*>(itsTree->GetRoot());
	assert( root != nullptr );

	const JSize count = root->GetChildCount();
	output << ' ' << count;

	for (JIndex i=1; i<=count; i++)
		{
		CMVarNode* node = root->GetVarChild(i);
		output << ' ' << node->GetName();
		output << ' ' << node->GetBase();
		}
}

/******************************************************************************
 HasSelection

 ******************************************************************************/

bool
CMVarTreeWidget::HasSelection()
	const
{
	return (GetTableSelection()).HasSelection();
}

/******************************************************************************
 RemoveSelection

 ******************************************************************************/

void
CMVarTreeWidget::RemoveSelection()
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	while (iter.Next(&cell))
		{
		JTreeNode* node = GetTreeList()->GetNode(cell.y);
		if (node->GetDepth() == 1)
			{
			jdelete node;
			}
		}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 IsEditable (virtual)

	Derived classes should override this if there are some cells that
	cannot be edited.

 ******************************************************************************/

bool
CMVarTreeWidget::IsEditable
	(
	const JPoint& cell
	)
	const
{
	const auto* node =
		dynamic_cast<const CMVarNode*>(GetTreeList()->GetNode(cell.y));
	assert( node != nullptr );

	return JXTreeListWidget::IsEditable(cell) &&
				((JIndex(cell.x) == GetNodeColIndex() &&
				  itsIsMainDisplayFlag && node->GetDepth() == 1) ||
				 (JIndex(cell.x) == kValueColIndex &&
				  node->ValueIsValid() && !(node->GetValue()).IsEmpty()));
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
CMVarTreeWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	if (JIndex(cell.x) == kValueColIndex)
		{
		const JPoint fakeCell(GetNodeColIndex(), cell.y);
		HilightIfSelected(p, fakeCell, rect);

		auto* node =
			dynamic_cast<CMVarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		JFont font = GetFont();
		font.SetStyle(node->GetFontStyle());
		p.SetFont(font);
		p.String(rect, node->GetValue(), JPainter::kHAlignLeft, JPainter::kVAlignCenter);
		}
	else
		{
		JXNamedTreeListWidget::TableDrawCell(p, cell, rect);
		}
}

/******************************************************************************
 GetMinCellWidth (virtual protected)

 ******************************************************************************/

JSize
CMVarTreeWidget::GetMinCellWidth
	(
	const JPoint& cell
	)
	const
{
	if (JIndex(cell.x) > GetNodeColIndex())
		{
		const auto* node =
			dynamic_cast<const CMVarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		return kHMargin + GetFont().GetStringWidth(GetFontManager(), node->GetValue());
		}
	else
		{
		return JXNamedTreeListWidget::GetMinCellWidth(cell);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CMVarTreeWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ClearIncrementalSearchBuffer();
	itsDragType = kInvalidDrag;

	JPoint cell;
	if (ScrollForWheel(button, modifiers))
		{
		// work has been done
		}
	else if (!GetCell(pt, &cell))
		{
		(GetTableSelection()).ClearSelection();
		}
	else if (JIndex(cell.x) == GetToggleOpenColIndex())
		{
		itsDragType = kOpenNodeDrag;
		JXNamedTreeListWidget::HandleMouseDown(pt, button, clickCount,
											   buttonStates, modifiers);
		}
	else if (button == kJXRightButton)
		{
		JTableSelection& s = GetTableSelection();
		JPoint fakeCell(GetNodeColIndex(), cell.y);
		if (!s.HasSelection() || !s.IsSelected(fakeCell))
			{
			SelectSingleCell(fakeCell);
			}
		itsBasePopupMenu->PopUp(this, pt, buttonStates, modifiers);
		}
	else if (clickCount == 1)
		{
		itsDragType = kSelectDrag;
		cell.x      = GetNodeColIndex();
		BeginSelectionDrag(cell, button, modifiers);
		}
	else if (clickCount == 2 && JIndex(cell.x) == GetNodeColIndex())
		{
		const JTreeNode* node = GetTreeList()->GetNode(cell.y);
		if (itsIsMainDisplayFlag && node->GetDepth() == 1)
			{
			BeginEditing(cell);
			}
		else
			{
			const auto* varNode = dynamic_cast<const CMVarNode*>(node);
			assert( varNode != nullptr );
			const JString expr = varNode->GetFullName();
			if (itsIsMainDisplayFlag)
				{
				NewExpression(expr);	// avoid Activate()
				}
			else
				{
				itsDir->DisplayExpression(expr);
				}
			}
		}
	else if (clickCount == 2 && JIndex(cell.x) == kValueColIndex)
		{
		BeginEditing(cell);
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
CMVarTreeWidget::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragType == kOpenNodeDrag)
		{
		JXNamedTreeListWidget::HandleMouseDrag(pt, buttonStates, modifiers);
		}
	else if (IsDraggingSelection())
		{
		ScrollForDrag(pt);

		JPoint cell;
		const bool ok = GetCell(JPinInRect(pt, GetBounds()), &cell);
		assert( ok );

		cell.x = GetNodeColIndex();
		JTable::ContinueSelectionDrag(cell);
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
CMVarTreeWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragType == kOpenNodeDrag)
		{
		JXNamedTreeListWidget::HandleMouseUp(pt, button, buttonStates, modifiers);
		}
	else if (IsDraggingSelection())
		{
		FinishSelectionDrag();
		}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
CMVarTreeWidget::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&   modifiers
	)
{
	if (c == kJReturnKey)
		{
		EndEditing();
		}

	else if (c == kJForwardDeleteKey || c == kJDeleteKey)
		{
		if (itsIsMainDisplayFlag)
			{
			RemoveSelection();
			}
		}

	else if (c == kJEscapeKey)
		{
		JPoint cell;
		if (GetEditedCell(&cell))
			{
			CancelEditing();
			JTreeNode* node = GetTreeList()->GetNode(cell.y);
			if (itsEditingNewNodeFlag &&
				JIndex(cell.x) == GetNodeColIndex() && node->GetDepth() == 1)
				{
				jdelete node;
				}
			}
		}

	else if ((c == kJUpArrow || c == kJDownArrow) && !IsEditing())
		{
		const bool hasSelection = HasSelection();
		if (!hasSelection && c == kJUpArrow && GetRowCount() > 0)
			{
			SelectSingleCell(JPoint(GetNodeColIndex(), GetRowCount()));
			}
		else if (!hasSelection && c == kJDownArrow && GetRowCount() > 0)
			{
			SelectSingleCell(JPoint(GetNodeColIndex(), 1));
			}
		else
			{
			HandleSelectionKeyPress(c, modifiers);
			}
		ClearIncrementalSearchBuffer();
		}

	else
		{
		JXNamedTreeListWidget::HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
CMVarTreeWidget::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	SelectSingleCell(JPoint(GetNodeColIndex(), cell.y));
	itsEditingNewNodeFlag = false;	// must override after BeginEditing()

	JXInputField* input = JXNamedTreeListWidget::CreateXInputField(cell, x,y, w,h);
	if (JIndex(cell.x) == kValueColIndex)
		{
		auto* node =
			dynamic_cast<CMVarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );
		input->GetText()->SetText(node->GetValue());
		}

	itsOrigEditValue = input->GetText()->GetText();

	input->SetIsRequired();
	input->GetText()->SetCharacterInWordFunction(CBMIsCharacterInWord);
	return input;
}

/******************************************************************************
 ExtractInputData

 ******************************************************************************/

bool
CMVarTreeWidget::ExtractInputData
	(
	const JPoint& cell
	)
{
	JXInputField* input = nullptr;
	const bool ok = GetXInputField(&input);
	assert( ok );
	const JString& text = input->GetText()->GetText();

	auto* node =
		dynamic_cast<CMVarNode*>(GetTreeList()->GetNode(cell.y));
	assert( node != nullptr );

	if (JIndex(cell.x) == GetNodeColIndex() &&
		JXNamedTreeListWidget::ExtractInputData(cell))
		{
		return true;
		}
	else if (JIndex(cell.x) == kValueColIndex && !text.IsEmpty())
		{
		if (text != itsOrigEditValue)
			{
			const JString name = node->GetFullName();
			CMGetLink()->SetValue(name, text);
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
CMVarTreeWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == CMGetLink() && message.Is(CMLink::kProgramFinished))
		{
		FlushOldData();
		}
	else if (sender == CMGetLink() &&
			 (message.Is(CMLink::kProgramRunning)       ||
			  message.Is(CMLink::kFrameChanged)         ||
			  message.Is(CMLink::kThreadChanged)        ||
			  message.Is(CMLink::kPrepareToLoadSymbols) ||
			  message.Is(CMLink::kCoreLoaded)           ||
			  message.Is(CMLink::kCoreCleared)          ||
			  message.Is(CMLink::kAttachedToProcess)    ||
			  message.Is(CMLink::kDetachedFromProcess)  ||
			  (message.Is(CMLink::kValueChanged) && !IsEditing())))
		{
		CancelEditing();
		}

	else if (sender == CMGetLink() && message.Is(CMLink::kDebuggerRestarted))
		{
		itsWaitingForReloadFlag = true;
		CancelEditing();
		}
	else if (sender == CMGetLink() && message.Is(CMLink::kDebuggerStarted))
		{
		if (!itsWaitingForReloadFlag)
			{
			(itsTree->GetRoot())->DeleteAllChildren();
			}
		itsWaitingForReloadFlag = false;
		}

	else if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		if (HasFocus())
			{
			UpdateEditMenu();
			}
		}
	else if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
		{
		if (HasFocus())
			{
			const auto* selection =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( selection != nullptr );
			HandleEditMenu(selection->GetIndex());
			}
		}

	else if (sender == itsBaseMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateBaseMenu(itsBaseMenu);
		}
	else if (sender == itsBasePopupMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateBaseMenu(itsBasePopupMenu);
		}
	else if ((sender == itsBaseMenu || sender == itsBasePopupMenu) &&
			 message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleBaseMenu(selection->GetIndex());
		}

	else if (sender == GetWindow() && message.Is(JXWindow::kIconified))
		{
		ShouldUpdate(false);
		}
	else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
		{
		ShouldUpdate(true);
		}

	else
		{
		JTableSelection& s = GetTableSelection();
		if (sender == &s && message.Is(JTableData::kRectChanged))
			{
			// refresh value column to show fake selection

			const auto* selection =
				dynamic_cast<const JTableData::RectChanged*>(&message);
			assert( selection != nullptr );
			JRect r = selection->GetRect();
			r.right = kValueColIndex;
			TableRefreshCellRect(r);
			}

		JXNamedTreeListWidget::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
CMVarTreeWidget::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (!CMIsShuttingDown())
		{
		ListenTo(CMGetLink());
		}

	JXNamedTreeListWidget::ReceiveGoingAway(sender);
}

/******************************************************************************
 ShouldUpdate

 ******************************************************************************/

void
CMVarTreeWidget::ShouldUpdate
	(
	const bool update
	)
{
	dynamic_cast<CMVarNode*>(itsTree->GetRoot())->ShouldUpdate(update);
}

/******************************************************************************
 FlushOldData (private)

 ******************************************************************************/

void
CMVarTreeWidget::FlushOldData()
{
	CancelEditing();

	JTreeNode* root   = itsTree->GetRoot();
	const JSize count = root->GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		auto* child = dynamic_cast<CMVarNode*>(root->GetChild(i));
		assert( child != nullptr );
		child->DeleteAllChildren();
		child->SetValue(JString::empty);
		}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
CMVarTreeWidget::UpdateEditMenu()
{
	JXTEBase* te = GetEditMenuHandler();

	JIndex index;
	if (te->EditMenuCmdToIndex(JTextEditor::kSelectAllCmd, &index))
		{
		itsEditMenu->EnableItem(index);
		}

	if ((GetTableSelection()).HasSelection() &&
		te->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &index))
		{
		assert( itsCopyPathCmdIndex == index+1 );

		itsEditMenu->EnableItem(index);
		itsEditMenu->EnableItem(itsCopyPathCmdIndex);
		itsEditMenu->EnableItem(itsCopyValueCmdIndex);
		}
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
CMVarTreeWidget::HandleEditMenu
	(
	const JIndex index
	)
{
	ClearIncrementalSearchBuffer();

	JTextEditor::CmdIndex cmd;
	if (!GetEditMenuHandler()->EditMenuIndexToCmd(index, &cmd))
		{
		cmd = JTextEditor::kSeparatorCmd;
		}

	if (cmd == JTextEditor::kCopyCmd)
		{
		CopySelectedItems(false, false);
		}
	else if (index == itsCopyPathCmdIndex)
		{
		CopySelectedItems(true, false);
		}
	else if (index == itsCopyValueCmdIndex)
		{
		CopySelectedItems(false, true);
		}
	else if (cmd == JTextEditor::kSelectAllCmd)
		{
		JTableSelection& s = GetTableSelection();
		s.ClearSelection();
		s.SelectCol(GetNodeColIndex());
		}
}

/******************************************************************************
 CopySelectedItems (private)

 ******************************************************************************/

void
CMVarTreeWidget::CopySelectedItems
	(
	const bool useFullName,
	const bool copyValue
	)
	const
{
	const JTableSelection& s = GetTableSelection();
	if (s.HasSelection())
		{
		JPtrArray<JString> list(JPtrArrayT::kDeleteAll);

		JTableSelectionIterator iter(&s);
		JPoint cell;
		while (iter.Next(&cell))
			{
			const auto* node =
				dynamic_cast<const CMVarNode*>(GetTreeList()->GetNode(cell.y));
			assert( node != nullptr );
			if (copyValue)
				{
				list.Append(node->GetValue());
				}
			else if (useFullName)
				{
				list.Append(node->GetFullName());
				}
			else
				{
				list.Append(node->GetName());
				}
			}

		auto* data = jnew JXTextSelection(GetDisplay(), list);
		assert( data != nullptr );

		GetSelectionManager()->SetData(kJXClipboardName, data);
		}
}

/******************************************************************************
 UpdateBaseMenu (private)

 ******************************************************************************/

void
CMVarTreeWidget::UpdateBaseMenu
	(
	JXTextMenu* menu
	)
{
	JInteger base = -1;

	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	while (iter.Next(&cell))
		{
		const auto* node =
			dynamic_cast<const CMVarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );
		const JInteger b = node->GetBase();

		if (base >= 0 && b != base)
			{
			menu->EnableAll();
			return;
			}
		base = b;
		}

	if (base < 0)
		{
		menu->DisableAll();
		}
	else
		{
		menu->EnableAll();

		for (JUnsignedOffset i=0; i<kBaseCount; i++)
			{
			if (JSize(base) == kMenuIndexToBase[i])
				{
				menu->CheckItem(i+1);
				break;
				}
			}
		}
}

/******************************************************************************
 HandleBaseMenu (private)

 ******************************************************************************/

void
CMVarTreeWidget::HandleBaseMenu
	(
	const JIndex index
	)
{
	ClearIncrementalSearchBuffer();

	assert( index <= kBaseCount );
	const JSize base = kMenuIndexToBase[index-1];

	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	while (iter.Next(&cell))
		{
		auto* node =
			dynamic_cast<CMVarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );
		node->SetBase(base);
		}
}
