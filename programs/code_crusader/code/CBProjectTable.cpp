/******************************************************************************
 CBProjectTable.cpp

	Displays the files in a project.

	BASE CLASS = JXNamedTreeListWidget

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "CBProjectTable.h"
#include "CBProjectTree.h"
#include "CBGroupNode.h"
#include "CBLibraryNode.h"
#include "CBProjectDocument.h"
#include "CBRelPathCSF.h"
#include "CBProjectTableInput.h"
#include "cbGlobals.h"
#include "cbUtil.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <JXSelectionManager.h>
#include <JXTextSelection.h>
#include <JXDNDManager.h>
#include <JXFileSelection.h>
#include <JXWebBrowser.h>
#include <JXPathInput.h>
#include <JXTextMenu.h>
#include <JXImageButton.h>
#include <JXImage.h>
#include <JXDeleteObjectTask.h>
#include <JXGetStringDialog.h>
#include <jXUtil.h>
#include <JNamedTreeList.h>
#include <JTableSelection.h>
#include <JSimpleProcess.h>
#include <JLatentPG.h>
#include <JDirInfo.h>
#include <jFileUtil.h>
#include <jVCSUtil.h>
#include <jMouseUtil.h>
#include <jASCIIConstants.h>
#include <jAssert.h>

CBProjectTable::DropFileAction
	CBProjectTable::itsDropFileAction = CBProjectTable::kProjectRelative;

static const JCharacter* kSelectionDataID = "CBProjectTable";

// Context menu

static const JCharacter* kContextMenuStr =
	"    Open selected files                 %k Left-dbl-click or Return."
	"  | Open complement files               %k Middle-dbl-click or Meta-Tab."
	"%l| Edit sub-project configuration"
	"%l| New group"
	"  | Add files..."
	"  | Add directory tree..."
	"  | Remove selected items               %k Backspace."
	"%l| Edit file path                      %k Meta-left-dbl-click."
	"%l| Compare selected files with backup"
	"  | Compare selected files with version control"
	"  | Show selected files in file manager %k Meta-Return.";

enum
{
	kOpenFilesCmd = 1, kOpenComplFilesCmd,
	kEditSubprojConfigCmd,
	kNewGroupCmd, kAddFilesCmd, kAddDirTreeCmd, kRemoveSelCmd,
	kEditPathCmd,
	kDiffSmartCmd, kDiffVCSCmd, kShowLocationCmd
};

static const JCharacter* kOpenFilesItemStr     = "Open selected files";
static const JCharacter* kEditGroupNameItemStr = "Edit group name";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBProjectTable::CBProjectTable
	(
	CBProjectDocument*	doc,
	JXMenuBar*			menuBar,
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
	JXNamedTreeListWidget(treeList, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsDoc       = doc;
	itsSelDepth  = kEitherDepth;
	itsDragType  = kInvalidDrag;
	itsCSFButton = nullptr;

	itsDNDAction = kDNDInvalid;
	itsDNDBuffer = nullptr;

	itsIgnoreSelChangesFlag = kJFalse;
	SetSelectionBehavior(kJTrue, kJTrue);
	ListenTo(&(GetTableSelection()));

	itsLockedSelDepthFlag = kJFalse;

	itsEditMenu = GetEditMenuHandler()->AppendEditMenu(menuBar);
	ListenTo(itsEditMenu);

	itsContextMenu          = nullptr;
	itsAddFilesFilterDialog = nullptr;

	itsMarkWritableFlag =
		JI2B(getenv("CVSREAD") != nullptr &&
			 JGetVCSType(doc->GetFilePath()) == kJCVSType);

	WantInput(kJTrue, kJFalse, kJTrue);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBProjectTable::~CBProjectTable()
{
	jdelete itsDNDBuffer;
}

/******************************************************************************
 GetProjectNode

	We assert if the dynamic_cast fails because all nodes are required to be
	CBProjectNodes.

 ******************************************************************************/

CBProjectNode*
CBProjectTable::GetProjectNode
	(
	const JIndex index
	)
{
	CBProjectNode* node = dynamic_cast<CBProjectNode*>(GetTreeList()->GetNode(index));
	assert( node != nullptr );
	return node;
}

const CBProjectNode*
CBProjectTable::GetProjectNode
	(
	const JIndex index
	)
	const
{
	const CBProjectNode* node = dynamic_cast<const CBProjectNode*>(GetTreeList()->GetNode(index));
	assert( node != nullptr );
	return node;
}

/******************************************************************************
 NewGroup

	returnNode can be nullptr.

 ******************************************************************************/

JBoolean
CBProjectTable::NewGroup
	(
	CBGroupNode** returnNode
	)
{
	if (!EndEditing())
		{
		if (returnNode != nullptr)
			{
			*returnNode = nullptr;
			}
		return kJFalse;
		}

	CBProjectTree* tree = itsDoc->GetFileTree();

	CBGroupNode* newNode = jnew CBGroupNode(tree, kJFalse);
	assert( newNode != nullptr );

	if (returnNode != nullptr)
		{
		*returnNode = newNode;
		}

	JPoint cell;
	if ((GetTableSelection()).GetLastSelectedCell(&cell))
		{
		const JTreeNode* node = GetTreeList()->GetNode(cell.y);
		if (node->GetDepth() == kFileDepth)
			{
			node = node->GetParent();
			}

		(tree->GetRoot())->InsertAfter(node, newNode);
		}
	else
		{
		(tree->GetRoot())->Append(newNode);
		}

	JIndex index;
	const JBoolean found = GetTreeList()->FindNode(newNode, &index);
	assert( found );
	BeginEditing(JPoint(GetNodeColIndex(), index));
	return kJTrue;
}

/******************************************************************************
 AddDirectoryTree

 ******************************************************************************/

void
CBProjectTable::AddDirectoryTree()
{
	if (!EndEditing())
		{
		return;
		}

	CBRelPathCSF* csf = itsDoc->GetRelPathCSF();

	JString path;
	if (csf->ChooseRPath("", "", nullptr, &path))
		{
		AddDirectoryTree(path, csf->GetPathType());
		}
}

void
CBProjectTable::AddDirectoryTree
	(
	const JCharacter*				fullPath,
	const CBRelPathCSF::PathType	pathType
	)
{
	itsAddPath     = fullPath;
	itsAddPathType = pathType;

	assert( itsAddFilesFilterDialog == nullptr );
	itsAddFilesFilterDialog =
		jnew JXGetStringDialog(GetWindow()->GetDirector(),
							  JGetString("AddFilesTitle::CBProjectTable"),
							  JGetString("AddFilesPrompt::CBProjectTable"),
							  CBProjectDocument::GetAddFilesFilter());
	assert( itsAddFilesFilterDialog != nullptr );
	itsAddFilesFilterDialog->BeginDialog();
	ListenTo(itsAddFilesFilterDialog);
}

// private

void
CBProjectTable::AddDirectoryTree
	(
	const JCharacter*				fullPath,
	const JCharacter*				filterStr,
	const CBRelPathCSF::PathType	pathType
	)
{
	JString f = fullPath, p, n;
	JStripTrailingDirSeparator(&f);
	JSplitPathAndName(f, &p, &n);

	JBoolean changed = kJFalse;
	AddDirectoryTree(fullPath, n, filterStr, pathType, &changed);

	if (changed)
		{
		itsDoc->UpdateSymbolDatabase();
		}
}

void
CBProjectTable::AddDirectoryTree
	(
	const JCharacter*				fullPath,
	const JCharacter*				relPath,
	const JCharacter*				filterStr,
	const CBRelPathCSF::PathType	pathType,
	JBoolean*						changed
	)
{
	JDirInfo* info;
	if (JDirInfo::Create(fullPath, &info))
		{
		info->SetWildcardFilter(filterStr);

		JPtrArray<JString> fullNameList(JPtrArrayT::kDeleteAll);

		const JSize count = info->GetEntryCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JDirEntry& entry = info->GetEntry(i);
			const JString& name    = entry.GetName();
			if (entry.IsFile() &&
				!name.EndsWith(CBProjectDocument::GetProjectFileSuffix()) &&
				!name.EndsWith(CBProjectDocument::GetSettingFileSuffix()) &&
				!name.EndsWith(CBProjectDocument::GetSymbolFileSuffix()) &&
				!name.EndsWith(CBBuildManager::GetSubProjectBuildSuffix()))
				{
				fullNameList.Append(entry.GetFullName());
				}
			}

		if (!fullNameList.IsEmpty())
			{
			CBGroupNode* node;
			JBoolean ok = NewGroup(&node);
			assert( ok );
			ok = EndEditing();
			assert( ok );
			node->SetName(relPath);

			JPtrArray<JTreeNode> list(JPtrArrayT::kForgetAll);
			list.Append(node);
			SelectNodes(list);

			AddFiles(fullNameList, pathType, kJFalse, kJTrue);
			fullNameList.DeleteAll();

			if (node->GetChildCount() == 0)
				{
				jdelete node;
				}
			else
				{
				*changed = kJTrue;
				}
			}

		for (JIndex i=1; i<=count; i++)
			{
			const JDirEntry& entry = info->GetEntry(i);
			if (entry.IsDirectory() && !JIsVCSDirectory(entry.GetName()))
				{
				const JString name = JCombinePathAndName(relPath, entry.GetName());
				AddDirectoryTree(entry.GetFullName(), name, filterStr, pathType, changed);
				}
			}

		jdelete info;
		}
}

/******************************************************************************
 AddFiles

 ******************************************************************************/

void
CBProjectTable::AddFiles()
{
	if (!EndEditing())
		{
		return;
		}

	CBRelPathCSF* csf = itsDoc->GetRelPathCSF();

	JPtrArray<JString> fullNameList(JPtrArrayT::kDeleteAll);
	if (csf->ChooseFiles("", nullptr, &fullNameList))
		{
		AddFiles(fullNameList, csf->GetPathType());
		}
}

JBoolean
CBProjectTable::AddFiles
	(
	const JPtrArray<JString>&		fullNameList,
	const CBRelPathCSF::PathType	pathType,
	const JBoolean					updateProject,
	const JBoolean					silent
	)
{
	if (fullNameList.IsEmpty())
		{
		return kJFalse;
		}

	(JXGetApplication())->DisplayBusyCursor();

	CBProjectTree* tree = itsDoc->GetFileTree();
	JTreeList* treeList = GetTreeList();
	JTableSelection& s  = GetTableSelection();

	CBProjectNode* parent = nullptr;
	JBoolean parentIsNew  = kJFalse;
	JIndex childIndex     = 0;
	JPoint cell;
	if (s.GetLastSelectedCell(&cell))
		{
		JTreeNode* node = treeList->GetNode(cell.y);
		if (node->GetDepth() == kGroupDepth)
			{
			parent = dynamic_cast<CBProjectNode*>(node);
			if (treeList->IsOpen(parent))
				{
				childIndex = 1;
				}
			}
		else
			{
			assert( node->GetDepth() == kFileDepth );
			parent     = dynamic_cast<CBProjectNode*>(node->GetParent());
			childIndex = node->GetIndexInParent() + 1;
			}
		}
	else
		{
		parent      = jnew CBGroupNode(tree);
		parentIsNew = kJTrue;
		}
	assert( parent != nullptr );
	treeList->Open(parent);

	CBProjectNode* root     = tree->GetProjectRoot();
	CBProjectNode* firstNew = nullptr;
	CBProjectNode* lastNew  = nullptr;

	const JSize count = fullNameList.GetElementCount();

	JLatentPG pg(10);
	pg.FixedLengthProcessBeginning(count, "Adding files...", kJTrue, kJFalse);

	for (JIndex i=1; i<=count; i++)
		{
		const JString* fullName = fullNameList.GetElement(i);
		if (!fullName->EndsWith("~")  &&
			!fullName->EndsWith("#")  &&
			!root->Includes(*fullName))
			{
			const JString fileName =
				CBRelPathCSF::ConvertToRelativePath(
					*fullName, itsDoc->GetFilePath(), pathType);

			CBFileNodeBase* newNode = CBFileNodeBase::New(tree, fileName);
			assert( newNode != nullptr );

			if (childIndex > 0)
				{
				parent->InsertAtIndex(childIndex, newNode);
				childIndex++;
				}
			else
				{
				parent->Append(newNode);
				}

			if (firstNew == nullptr)
				{
				firstNew = newNode;
				}
			else
				{
				lastNew = newNode;
				}
			}

		if (!pg.IncrementProgress())
			{
			break;
			}
		}

	pg.ProcessFinished();

	if (firstNew != nullptr)
		{
		JIndex index;
		const JBoolean ok = treeList->FindNode(firstNew, &index);
		assert( ok );
		SelectSingleCell(JPoint(GetNodeColIndex(), index));
		}
	if (lastNew != nullptr)
		{
		JIndex index;
		const JBoolean ok = treeList->FindNode(lastNew, &index);
		assert( ok );
		s.ExtendSelection(JPoint(GetNodeColIndex(), index));
		}

	if (firstNew == nullptr && lastNew == nullptr)
		{
		if (parentIsNew)
			{
			jdelete parent;
			}

		if (!silent && fullNameList.GetElementCount() == 1)
			{
			JString path, msg;
			JSplitPathAndName(*(fullNameList.FirstElement()), &path, &msg);
			msg += " is already in the project.";
			(JGetUserNotification())->ReportError(msg);
			}
		else if (!silent)
			{
			(JGetUserNotification())->ReportError(
				"All the files that you selected are already in the project.");
			}
		}
	else if (firstNew != nullptr && updateProject)
		{
		itsDoc->DelayUpdateSymbolDatabase();
		}

	return JI2B(firstNew != nullptr);
}

/******************************************************************************
 GetDepth (protected)

 ******************************************************************************/

JSize
CBProjectTable::GetDepth
	(
	const JIndex index
	)
	const
{
	return (GetTreeList()->GetNode(index))->GetDepth();
}

/******************************************************************************
 HasSelection

 ******************************************************************************/

JBoolean
CBProjectTable::HasSelection()
	const
{
	return (GetTableSelection()).HasSelection();
}

/******************************************************************************
 GetSelectionType

	*index contains the index of the first selected node.

 ******************************************************************************/

JBoolean
CBProjectTable::GetSelectionType
	(
	SelType*	type,
	JBoolean*	single,
	JIndex*		index
	)
	const
{
	const JTableSelection& s = GetTableSelection();
	JPoint cell1, cell2;
	if (s.GetFirstSelectedCell(&cell1))
		{
		if (itsSelDepth == kFileDepth)
			{
			*type = kFileSelection;
			}
		else
			{
			*type = kGroupSelection;
			}

		*single = JI2B(s.GetLastSelectedCell(&cell2) && cell1.y == cell2.y);
		*index  = cell1.y;
		return kJTrue;
		}
	else
		{
		*type   = kNoSelection;
		*single = kJFalse;
		*index  = 0;
		return kJFalse;
		}
}

/******************************************************************************
 SelectAll

 ******************************************************************************/

void
CBProjectTable::SelectAll()
{
	if (!EndEditing())
		{
		return;
		}

	JTableSelection& s = GetTableSelection();

	itsSelDepth = kFileDepth;
	s.SelectAll();

	if (!s.HasSelection())
		{
		itsSelDepth = kGroupDepth;
		s.SelectAll();
		}
}

/******************************************************************************
 ClearSelection

 ******************************************************************************/

void
CBProjectTable::ClearSelection()
{
	(GetTableSelection()).ClearSelection();
}

/******************************************************************************
 SelectFileNodes

 ******************************************************************************/

void
CBProjectTable::SelectFileNodes
	(
	const JPtrArray<JTreeNode>& nodeList
	)
{
	JTreeList* treeList = GetTreeList();
	JTableSelection& s  = GetTableSelection();

	const JIndex colIndex = GetNodeColIndex();
	const JSize count     = nodeList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JTreeNode* node = nodeList.GetElement(i);
		treeList->Open(node->GetParent());

		JIndex rowIndex;
		const JBoolean found = treeList->FindNode(node, &rowIndex);
		assert( found );

		s.SelectCell(JPoint(colIndex, rowIndex));
		}
}

/******************************************************************************
 OpenSelection

 ******************************************************************************/

void
CBProjectTable::OpenSelection()
{
	if (!EndEditing())
		{
		return;
		}

	JTableSelection& s = GetTableSelection();

	JPoint cell;
	if (s.GetSingleSelectedCell(&cell) && GetDepth(cell.y) == kGroupDepth)
		{
		BeginEditing(cell);
		}
	else
		{
		JTableSelectionIterator iter(&s);
		while (iter.Next(&cell))
			{
			(GetProjectNode(cell.y))->OpenFile();
			}
		}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 OpenComplementFiles

 ******************************************************************************/

void
CBProjectTable::OpenComplementFiles()
{
	if (!EndEditing())
		{
		return;
		}

	JTableSelectionIterator iter(&(GetTableSelection()));

	JPoint cell;
	while (iter.Next(&cell))
		{
		(GetProjectNode(cell.y))->OpenComplementFile();
		}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 RemoveSelection

 ******************************************************************************/

void
CBProjectTable::RemoveSelection()
{
	CancelEditing();

	if (HasSelection() &&
		(JGetUserNotification())->AskUserNo(JGetString("OKToRemove::CBProjectTable")))
		{
		JTableSelectionIterator iter(&(GetTableSelection()));
		JTreeList* treeList = GetTreeList();

		itsLockedSelDepthFlag = kJTrue;

		JPoint cell;
		while (iter.Next(&cell))
			{
			jdelete treeList->GetNode(cell.y);
			}

		itsLockedSelDepthFlag = kJFalse;

		itsDoc->DelayUpdateSymbolDatabase();
		}
}

/******************************************************************************
 PlainDiffSelection

 ******************************************************************************/

void
CBProjectTable::PlainDiffSelection()
{
	if (!EndEditing())
		{
		return;
		}

	const JTableSelection& s = GetTableSelection();
	const JBoolean silent    = JI2B(s.GetSelectedCellCount() > 1);

	JTableSelectionIterator iter(&s);

	JPoint cell;
	while (iter.Next(&cell))
		{
		(GetProjectNode(cell.y))->ViewPlainDiffs(silent);
		}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 VCSDiffSelection

 ******************************************************************************/

void
CBProjectTable::VCSDiffSelection()
{
	if (!EndEditing())
		{
		return;
		}

	const JTableSelection& s = GetTableSelection();
	const JBoolean silent    = JI2B(s.GetSelectedCellCount() > 1);

	JTableSelectionIterator iter(&s);

	JPoint cell;
	while (iter.Next(&cell))
		{
		(GetProjectNode(cell.y))->ViewVCSDiffs(silent);
		}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 ShowSelectedFileLocations

 ******************************************************************************/

void
CBProjectTable::ShowSelectedFileLocations()
{
	if (!EndEditing())
		{
		return;
		}

	JTableSelectionIterator iter(&(GetTableSelection()));
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	JString fullName;
	JPoint cell;
	while (iter.Next(&cell))
		{
		if ((GetProjectNode(cell.y))->GetFullName(&fullName))
			{
			list.Append(fullName);
			}
		}

	(JXGetWebBrowser())->ShowFileLocations(list);
	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 EditFilePath

 ******************************************************************************/

void
CBProjectTable::EditFilePath()
{
	JPoint cell;
	if ((GetTableSelection()).GetSingleSelectedCell(&cell) &&
		GetDepth(cell.y) == kFileDepth)
		{
		BeginEditing(cell);
		}
}

/******************************************************************************
 EditSubprojectConfig

 ******************************************************************************/

void
CBProjectTable::EditSubprojectConfig()
{
	JPoint cell;
	if ((GetTableSelection()).GetSingleSelectedCell(&cell))
		{
		CBProjectNode* node = GetProjectNode(cell.y);
		if (node->GetType() == kCBLibraryNT)
			{
			CBLibraryNode* lib = dynamic_cast<CBLibraryNode*>(node);
			assert( lib != nullptr );
			lib->EditSubprojectConfig();
			}
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBProjectTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateEditMenu();
		}
	else if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleEditMenu(selection->GetIndex());
		}

	else if (sender == itsContextMenu && message.Is(JXTextMenu::kNeedsUpdate))
		{
		UpdateContextMenu();
		}
	else if (sender == itsContextMenu && message.Is(JXTextMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleContextMenu(selection->GetIndex());
		}

	else if (sender == itsCSFButton && message.Is(JXButton::kPushed))
		{
		JXInputField* inputField;
		JBoolean ok = GetXInputField(&inputField);
		assert( ok );

		JPoint cell;
		ok = GetEditedCell(&cell);
		assert( ok );

		JString newName = inputField->GetText();

		ok = (itsDoc->GetRelPathCSF())->ChooseRelFile("", "", newName, &newName);	// kills inputField
		if (BeginEditing(cell) && ok && GetXInputField(&inputField))
			{
			inputField->SetText(newName);
			}
		}

	else if (sender == itsAddFilesFilterDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			const JString& filter = itsAddFilesFilterDialog->GetString();
			CBProjectDocument::SetAddFilesFilter(filter);
			AddDirectoryTree(itsAddPath, filter, itsAddPathType);
			}
		itsAddFilesFilterDialog = nullptr;
		}

	else
		{
		if (sender == &(GetTableSelection()) && !itsIgnoreSelChangesFlag)
			{
			CleanSelection();
			}

		// JXTreeListWidget preserves the current, valid selection

		else if (sender == GetTreeList()->GetTree() &&
				 message.Is(JTree::kPrepareForNodeMove))
			{
			itsIgnoreSelChangesFlag = kJTrue;
			}
		else if (sender == GetTreeList()->GetTree() &&
				 message.Is(JTree::kNodeMoveFinished))
			{
			itsIgnoreSelChangesFlag = kJFalse;
			}

		JXNamedTreeListWidget::Receive(sender, message);
		}
}

/******************************************************************************
 AdjustToTree (virtual protected)

 ******************************************************************************/

void
CBProjectTable::AdjustToTree()
{
	const JIndex colIndex = GetNodeColIndex();
	const JSize count     = GetRowCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (GetDepth(i) == kGroupDepth)
			{
			SetCellStyle(JPoint(colIndex, i),
						 JFontStyle(kJTrue, kJFalse, 0, kJFalse));
			}
		}

	JXNamedTreeListWidget::AdjustToTree();
}

/******************************************************************************
 CleanSelection (private)

	Only items at itsSelDepth are allowed to be selected.

 ******************************************************************************/

void
CBProjectTable::CleanSelection()
{
	if (itsIgnoreSelChangesFlag)
		{
		return;
		}

	itsIgnoreSelChangesFlag = kJTrue;

	UpdateSelDepth();

	JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);

	JPoint cell;
	while (iter.Next(&cell))
		{
		if (!IsSelectable(cell, kJTrue))
			{
			s.SelectCell(cell, kJFalse);
			}
		}

	ClearIncrementalSearchBuffer();
	itsIgnoreSelChangesFlag = kJFalse;
}

/******************************************************************************
 UpdateSelDepth (private)

	We have to call IndexValid() because this can get called while
	JTableSelection is preparing to synchronize with JTable.

 ******************************************************************************/

void
CBProjectTable::UpdateSelDepth()
{
	// While dragging, we shouldn't reset the selection depth.

	if ((itsLockedSelDepthFlag || itsDragType == kSelectDrag) &&
		itsSelDepth != kEitherDepth)
		{
		return;
		}

	const JTableSelection& s = GetTableSelection();

	JPoint cell1, cell2;
	if (s.GetAnchor(&cell1) && GetTreeList()->IndexValid(cell1.y))
		{
		// maintain depth during JTableSelection::UndoSelection()

		itsSelDepth = GetDepth(cell1.y);
		}
	else if (!s.HasSelection())
		{
		itsSelDepth = kEitherDepth;
		}
	else if (s.GetFirstSelectedCell(&cell1) &&
			 s.GetLastSelectedCell(&cell2) &&
			 ((cell1.y == cell2.y && GetTreeList()->IndexValid(cell1.y)) ||
			  itsSelDepth == kEitherDepth))
		{
		itsSelDepth = GetDepth(cell1.y);
		}
}

/******************************************************************************
 IsSelectable (virtual)

 ******************************************************************************/

JBoolean
CBProjectTable::IsSelectable
	(
	const JPoint&	cell,
	const JBoolean	forExtend
	)
	const
{
	return JI2B(JIndex(cell.x) == GetNodeColIndex() &&
				(!forExtend ||
				 itsSelDepth == kEitherDepth ||
				 GetDepth(cell.y) == itsSelDepth) );
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
CBProjectTable::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXNamedTreeListWidget::Draw(p, rect);

	if (itsDNDAction != kDNDInvalid)
		{
		p.ResetClipRect();

		const JSize origLineWidth = p.GetLineWidth();
		p.SetLineWidth(2);

		const JRect b = GetBounds();
		if (itsDNDAction == kDNDFileBombsight ||
			(itsDNDAction == kDNDNewGroup && GetRowCount() > 0))
			{
			const JRect r = GetCellRect(itsDNDCell);
			p.Line(b.left, r.bottom-1, b.right, r.bottom-1);
			}
		else if (itsDNDAction == kDNDAppendToGroup)
			{
			p.RectInside(GetCellRect(itsDNDCell));
			}
		else if (itsDNDAction == kDNDGroupBombsight &&
				 JIndex(itsDNDCell.y) <= GetRowCount())
			{
			const JRect r = GetCellRect(itsDNDCell);
			p.Line(b.left, r.top, b.right, r.top);
			}
		else if (itsDNDAction == kDNDGroupBombsight)
			{
			p.Line(b.left, b.bottom-1, b.right, b.bottom-1);
			}

		p.SetLineWidth(origLineWidth);
		}
}

/******************************************************************************
 GetImage (virtual protected)

 ******************************************************************************/

JBoolean
CBProjectTable::GetImage
	(
	const JIndex	index,
	const JXImage**	image
	)
	const
{
	const CBProjectNode* node = GetProjectNode(index);
	if (itsMarkWritableFlag && node->GetType() == kCBFileNT)
		{
		const CBFileNodeBase* n = dynamic_cast<const CBFileNodeBase*>(node);
		assert( n != nullptr );

		JString fullName;
		if (n->GetFullName(&fullName) && JFileWritable(fullName))
			{
			*image = (node->IncludedInMakefile() ?
					  CBGetWritableSourceFileIcon() : CBGetWritablePlainFileIcon());
			return kJTrue;
			}
		}

	if (node->GetType() == kCBLibraryNT)
		{
		*image = CBGetLibraryFileIcon();
		return kJTrue;
		}
	else if (node->IncludedInMakefile())
		{
		*image = CBGetSourceFileIcon();
		return kJTrue;
		}
	else if (node->GetType() == kCBFileNT)
		{
		*image = CBGetPlainFileIcon();
		return kJTrue;
		}
	else
		{
		*image = nullptr;
		return kJFalse;
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CBProjectTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	(CBGetDocumentManager())->SetActiveProjectDocument(itsDoc);

	ClearIncrementalSearchBuffer();
	itsDragType = kInvalidDrag;
	itsStartPt  = pt;

	JPoint cell;
	if (ScrollForWheel(button, modifiers))
		{
		// work has been done
		}
	else if (!GetCell(pt, &cell))
		{
		ClearSelection();
		if (button == kJXRightButton && clickCount == 1)
			{
			CreateContextMenu();
			itsContextMenu->PopUp(this, pt, buttonStates, modifiers);
			}
		}
	else if (JIndex(cell.x) == GetToggleOpenColIndex())
		{
		itsDragType = kOpenNodeDrag;
		JXNamedTreeListWidget::HandleMouseDown(pt, button, clickCount,
											   buttonStates, modifiers);
		}
	else if (button == kJXLeftButton && clickCount == 1 &&
			 !modifiers.shift() && !modifiers.control() && !modifiers.meta())
		{
		itsDragType = kWaitForDNDDrag;
		if (!(GetTableSelection()).IsSelected(cell))
			{
			SelectSingleCell(cell, kJFalse);
			}
		}
	else if (button == kJXLeftButton && clickCount == 2 && modifiers.meta())
		{
		EditFilePath();
		}
	else if (button == kJXLeftButton && clickCount == 2)
		{
		OpenSelection();
		}
	else if (button == kJXMiddleButton && clickCount == 1)
		{
		SelectSingleCell(cell, kJFalse);
		}
	else if (button == kJXMiddleButton && clickCount == 2)
		{
		OpenComplementFiles();
		}
	else if (button == kJXRightButton && clickCount == 1)
		{
		if (!(GetTableSelection()).IsSelected(cell))
			{
			SelectSingleCell(cell, kJFalse);
			}
		CreateContextMenu();
		itsContextMenu->PopUp(this, pt, buttonStates, modifiers);
		}
	else if (clickCount == 1)
		{
		BeginSelectionDrag(cell, button, modifiers);
		itsDragType = kSelectDrag;		// must be last so UpdateSelDepth() works
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
CBProjectTable::HandleMouseDrag
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
	else if (itsDragType == kWaitForDNDDrag && JMouseMoved(itsStartPt, pt))
		{
		JXFileSelection* data = jnew JXFileSelection(this, kSelectionDataID);
		assert( data != nullptr );

		if (BeginDND(pt, buttonStates, modifiers, data))
			{
			itsDragType = kInvalidDrag;
			}
		}
	else if (IsDraggingSelection())
		{
		ScrollForDrag(pt);

		JPoint cell;
		const JBoolean ok = GetCell(JPinInRect(pt, GetBounds()), &cell);
		assert( ok );

		cell.x = GetNodeColIndex();
		JTable::ContinueSelectionDrag(cell);
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
CBProjectTable::HandleMouseUp
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
	else if (itsDragType == kWaitForDNDDrag)
		{
		JPoint cell;
		const JBoolean ok = GetCell(itsStartPt, &cell);
		assert( ok );
		SelectSingleCell(cell, kJFalse);
		}
	else if (IsDraggingSelection())
		{
		FinishSelectionDrag();

		// Ctrl-drag can set anchor to wrong depth, which will
		// clear selection on next extend drag.

		JTableSelection& s = GetTableSelection();
		JPoint cell;
		if (s.GetAnchor(&cell) &&
			GetDepth(cell.y) != itsSelDepth)
			{
			s.ClearAnchor();
			}
		}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
CBProjectTable::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	(CBGetDocumentManager())->SetActiveProjectDocument(itsDoc);

	if (key == kJReturnKey)
		{
		if (IsEditing())
			{
			EndEditing();
			}
		else
			{
			OpenSelection();
			}
		}

	else if (key == kJDeleteKey || key == kJForwardDeleteKey)
		{
		RemoveSelection();
		}

	else if (key == '\t' &&
			 !modifiers.GetState(kJXMetaKeyIndex)   &&
			 modifiers.GetState(kJXControlKeyIndex) &&
			 !modifiers.shift())
		{
		OpenComplementFiles();
		}

	else if ((key == kJUpArrow || key == kJDownArrow) && !IsEditing())
		{
		const JBoolean hasSelection = HasSelection();
		if (!hasSelection && key == kJUpArrow && GetRowCount() > 0)
			{
			SelectSingleCell(JPoint(GetNodeColIndex(), GetRowCount()));
			}
		else if (!hasSelection && key == kJDownArrow && GetRowCount() > 0)
			{
			SelectSingleCell(JPoint(GetNodeColIndex(), 1));
			}
		else
			{
			HandleSelectionKeyPress(key, modifiers);
			}
		ClearIncrementalSearchBuffer();
		}

	else
		{
		JXNamedTreeListWidget::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 DNDInit (virtual protected)

 ******************************************************************************/

void
CBProjectTable::DNDInit
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	assert( itsSelDepth != kEitherDepth );
}

/******************************************************************************
 GetSelectionData (virtual protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
CBProjectTable::GetSelectionData
	(
	JXSelectionData*	data,
	const JCharacter*	id
	)
{
	if (strcmp(id, kSelectionDataID) == 0)
		{
		assert( HasSelection() );

		JPtrArray<JString>* list = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert( list != nullptr );

		GetSelectedFileNames(list);

		JXFileSelection* fileData = dynamic_cast<JXFileSelection*>(data);
		assert( fileData != nullptr );
		fileData->SetData(list);
		}
	else
		{
		JXNamedTreeListWidget::GetSelectionData(data, id);
		}
}

/******************************************************************************
 GetSelectedFileNames

 ******************************************************************************/

void
CBProjectTable::GetSelectedFileNames
	(
	JPtrArray<JString>* list
	)
	const
{
	list->DeleteAll();

	JTableSelectionIterator iter(&(GetTableSelection()));

	JPoint cell;
	while (iter.Next(&cell))
		{
		const CBProjectNode* node = GetProjectNode(cell.y);
		if (node->GetType() == kCBGroupNT)
			{
			const JSize count = node->GetChildCount();
			for (JIndex i=1; i<=count; i++)
				{
				CopyFileToDNDList(node->GetChild(i), list);
				}
			}
		else
			{
			CopyFileToDNDList(node, list);
			}
		}
}

/******************************************************************************
 CopyFileToDNDList (private)

 ******************************************************************************/

void
CBProjectTable::CopyFileToDNDList
	(
	const JTreeNode*	node,
	JPtrArray<JString>*	list
	)
	const
{
	const CBFileNodeBase* fNode = dynamic_cast<const CBFileNodeBase*>(node);
	assert( fNode != nullptr );

	JString* s = jnew JString;
	assert( s != nullptr );
	if (fNode->GetFullName(s))
		{
		list->Append(s);
		}
	else
		{
		jdelete s;
		}
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
CBProjectTable::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return GetDNDManager()->GetDNDActionPrivateXAtom();
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

 ******************************************************************************/

void
CBProjectTable::HandleDNDResponse
	(
	const JXContainer*	target,
	const JBoolean		dropAccepted,
	const Atom			action
	)
{
	if (itsSelDepth == kFileDepth)
		{
		DisplayCursor(GetDNDManager()->GetDNDFileCursor(dropAccepted, action));
		}
	else
		{
		JXNamedTreeListWidget::HandleDNDResponse(target, dropAccepted, action);
		}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept any drops from ourselves and text/uri-list from anybody else.

 ******************************************************************************/

JBoolean
CBProjectTable::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	if (source == this)
		{
		return kJTrue;
		}

	const Atom urlXAtom1 = GetSelectionManager()->GetURLXAtom(),
			   urlXAtom2 = GetSelectionManager()->GetURLNoCharsetXAtom();

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		const Atom a = typeList.GetElement(i);
		if (a == urlXAtom1 || a == urlXAtom2)
			{
			*action = GetDNDManager()->GetDNDActionPrivateXAtom();
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

	This is called when the mouse enters the widget.

 ******************************************************************************/

void
CBProjectTable::HandleDNDEnter()
{
	itsDNDAction = kDNDInvalid;
	itsDNDCell.Set(0,0);
}

/******************************************************************************
 HandleDNDHere (virtual protected)

	This is called while the mouse is inside the widget.

 ******************************************************************************/

void
CBProjectTable::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget*	source
	)
{
	const JSize depth = (source == this ? itsSelDepth : JSize(kFileDepth));
	assert( depth > kEitherDepth );

	JTableSelection& s    = GetTableSelection();
	const JSize rowCount  = GetRowCount();
	const JIndex colIndex = GetNodeColIndex();

	JPoint cell;
	const JBoolean inCell = GetCell(pt, &cell);
	cell.x = colIndex;

	if (depth == kFileDepth && inCell && GetDepth(cell.y) == kFileDepth)
		{
		itsDNDCell    = cell;
		const JRect r = GetCellRect(itsDNDCell);
		if (pt.y <= r.ycenter())
			{
			itsDNDCell.y--;
			}

		itsDNDAction = kDNDFileBombsight;
		if (source == this &&
			(s.IsSelected(itsDNDCell) ||
			 (RowIndexValid(itsDNDCell.y+1) &&
			  s.IsSelected(itsDNDCell.y+1, itsDNDCell.x))))
			{
			itsDNDAction = kDNDInvalid;
			}
		}
	else if (depth == kFileDepth && inCell)
		{
		itsDNDAction = kDNDAppendToGroup;
		itsDNDCell   = cell;
		}
	else if (depth == kFileDepth && pt.y <= 0)
		{
		itsDNDAction = kDNDAppendToGroup;
		itsDNDCell.Set(colIndex, 1);
		}
	else if (depth == kFileDepth && rowCount > 0 &&
			 GetDepth(rowCount) == kFileDepth)
		{
		itsDNDCell.Set(colIndex, rowCount);
		itsDNDAction = ((s.IsSelected(itsDNDCell) && source == this) ?
						kDNDInvalid : kDNDFileBombsight);
		}
	else if (depth == kFileDepth)
		{
		itsDNDAction = kDNDNewGroup;
		itsDNDCell.Set(colIndex, rowCount);
		}

	else if (depth == kGroupDepth && inCell)
		{
		JBoolean found;

		JTreeList* list = GetTreeList();
		JTreeNode* node = list->GetNode(cell.y);
		if (node->GetDepth() == kFileDepth)
			{
			node = node->GetParent();
			}
		JIndex rowIndex;
		found = list->FindNode(node, &rowIndex);
		assert( found );

		JTreeNode* root = (list->GetTree())->GetRoot();
		JIndex rootIndex;
		found = root->FindChild(node, &rootIndex);
		assert( found );

		itsDNDCell.Set(colIndex, rowIndex);

		JRect r = GetCellRect(itsDNDCell);
		if (rootIndex < root->GetChildCount())
			{
			JIndex rowIndex1;
			found = list->FindNode(root->GetChild(rootIndex+1), &rowIndex1);
			assert( found );

			r = JCovering(r, GetCellRect(JPoint(colIndex, rowIndex1-1)));
			if (pt.y > r.ycenter())
				{
				itsDNDCell.y = rowIndex1;
				}
			}
		else
			{
			r.bottom = (GetBounds()).bottom;
			if (pt.y > r.ycenter())
				{
				itsDNDCell.y = rowCount+1;
				}
			}

		itsDNDAction = ((CellValid(itsDNDCell) && s.IsSelected(itsDNDCell)) ?
						kDNDInvalid : kDNDGroupBombsight);
		}
	else if (depth == kGroupDepth)
		{
		itsDNDAction = kDNDGroupBombsight;
		itsDNDCell.Set(colIndex, pt.y <= 0 ? 1 : rowCount+1);
		}

	Refresh();
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

	This is called when the mouse leaves the widget without dropping data.

 ******************************************************************************/

void
CBProjectTable::HandleDNDLeave()
{
	itsDNDAction = kDNDInvalid;
	itsDNDCell.Set(0,0);
	Refresh();
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

	Since we only accept text/uri-list, we don't bother to check typeList.

 ******************************************************************************/

void
CBProjectTable::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	if (source == this && itsSelDepth == kFileDepth && itsDNDAction == kDNDFileBombsight)
		{
		JTreeNode* node = GetTreeList()->GetNode(itsDNDCell.y);
		InsertFileSelectionAfter(node);
		}
	else if (source == this && itsSelDepth == kFileDepth && itsDNDAction == kDNDAppendToGroup)
		{
		JTreeNode* groupNode = GetTreeList()->GetNode(itsDNDCell.y);
		AppendFileSelectionToGroup(groupNode);
		}
	else if (source == this && itsSelDepth == kFileDepth && itsDNDAction == kDNDNewGroup)
		{
		CBGroupNode* groupNode = jnew CBGroupNode(itsDoc->GetFileTree());
		assert( groupNode != nullptr );
		AppendFileSelectionToGroup(groupNode);
		}
	else if (source == this && itsSelDepth == kGroupDepth && itsDNDAction == kDNDGroupBombsight)
		{
		JTreeNode* groupNode = (CellValid(itsDNDCell) ?
								GetTreeList()->GetNode(itsDNDCell.y) :
								(JTreeNode*) nullptr);
		InsertGroupSelectionBefore(groupNode);
		}
	else if (source != this)
		{
		if (!PrivateHandleDNDDrop(time, GetSelectionManager()->GetURLXAtom()))
			{
			PrivateHandleDNDDrop(time, GetSelectionManager()->GetURLNoCharsetXAtom());
			}
		}

	HandleDNDLeave();
}

/******************************************************************************
 PrivateHandleDNDDrop (private)

 ******************************************************************************/

JBoolean
CBProjectTable::PrivateHandleDNDDrop
	(
	const Time	time,
	const Atom	type
	)
{
	JXSelectionManager* selMgr = GetSelectionManager();
	JXDNDManager* dndMgr       = GetDNDManager();

	Atom returnType;
	unsigned char* data;
	JSize dataLength;
	JXSelectionManager::DeleteMethod delMethod;
	if (!selMgr->GetData(dndMgr->GetDNDSelectionName(),
						 time, type,
						 &returnType, &data, &dataLength, &delMethod))
		{
		return kJFalse;
		}

	if (returnType == type)
		{
		selMgr->DeleteData(&data, delMethod);
		return kJFalse;
		}

	JPtrArray<JString> fileNameList(JPtrArrayT::kDeleteAll),
					   urlList(JPtrArrayT::kDeleteAll),
					   pathList(JPtrArrayT::kDeleteAll);
	JXUnpackFileNames((char*) data, dataLength, &fileNameList, &urlList);

	const JSize fileCount = fileNameList.GetElementCount();

	// insert files and directories into tree

	for (JIndex i=fileCount; i>=1; i--)
		{
		JString* fullName = fileNameList.GetElement(i);
		if (JDirectoryExists(*fullName))
			{
			fileNameList.RemoveElement(i);
			pathList.AppendElement(fullName);
			}
		else if (!JFileExists(*fullName))
			{
			fileNameList.DeleteElement(i);
			}
		}

	if (fileNameList.IsEmpty() && pathList.IsEmpty() && urlList.IsEmpty())
		{
		(JGetUserNotification())->ReportError(
			"You can only drop files and/or directories.");
		}
	else if (fileNameList.IsEmpty() && pathList.IsEmpty())
		{
		JXReportUnreachableHosts(urlList);
		}
	else if (itsDropFileAction == kAskPathType)
		{
		JArray<Atom> actionList;
		actionList.AppendElement(CBRelPathCSF::kAbsolutePath);
		actionList.AppendElement(CBRelPathCSF::kProjectRelative);
		actionList.AppendElement(CBRelPathCSF::kHomeRelative);

		JPtrArray<JString> descriptionList(JPtrArrayT::kForgetAll);
		JString absPathText = "Add files using absolute path";
		JString projRelText = "Add files using path relative to project file";
		JString homeRelText = "Add files using path relative to home directory";
		descriptionList.Append(&absPathText);
		descriptionList.Append(&projRelText);
		descriptionList.Append(&homeRelText);

		Atom action = CBRelPathCSF::kProjectRelative;
		if (dndMgr->ChooseDropAction(actionList, descriptionList, &action))
			{
			InsertExtDroppedFiles(fileNameList, (CBRelPathCSF::PathType) action);

			const JSize pathCount = pathList.GetElementCount();
			for (JIndex i=1; i<=pathCount; i++)
				{
				AddDirectoryTree(*(pathList.GetElement(i)), (CBRelPathCSF::PathType) action);
				}
			}
		}
	else
		{
		InsertExtDroppedFiles(fileNameList, (CBRelPathCSF::PathType) itsDropFileAction);

		const JSize pathCount = pathList.GetElementCount();
		for (JIndex i=1; i<=pathCount; i++)
			{
			AddDirectoryTree(*(pathList.GetElement(i)), (CBRelPathCSF::PathType) itsDropFileAction);
			}
		}
	}

	selMgr->DeleteData(&data, delMethod);
	return kJTrue;
}

/******************************************************************************
 InsertFileSelectionAfter (private)

 ******************************************************************************/

void
CBProjectTable::InsertFileSelectionAfter
	(
	JTreeNode* after
	)
{
	JPtrArray<JTreeNode> list(JPtrArrayT::kForgetAll);
	GetSelectedNodes(&list);

	const JSize depth = after->GetDepth();
	const JSize count = list.GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		JTreeNode* node = list.GetElement(i);
		if (depth == kGroupDepth)
			{
			after->Prepend(node);
			}
		else
			{
			(after->GetParent())->InsertAfter(after, node);
			}
		}
}

/******************************************************************************
 AppendFileSelectionToGroup (private)

 ******************************************************************************/

void
CBProjectTable::AppendFileSelectionToGroup
	(
	JTreeNode* group
	)
{
	JPtrArray<JTreeNode> list(JPtrArrayT::kForgetAll);
	GetSelectedNodes(&list);

	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		group->Append(list.GetElement(i));
		}
}

/******************************************************************************
 InsertGroupSelectionBefore (private)

	If 'before' is nullptr, the selected groups are moved to the end of the list.

 ******************************************************************************/

void
CBProjectTable::InsertGroupSelectionBefore
	(
	JTreeNode* before
	)
{
	JPtrArray<JTreeNode> list(JPtrArrayT::kForgetAll);
	GetSelectedNodes(&list);

	JTreeNode* root   = (GetTreeList()->GetTree())->GetRoot();
	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JTreeNode* node = list.GetElement(i);
		if (before != nullptr)
			{
			root->InsertBefore(before, node);
			}
		else
			{
			root->Append(node);
			}
		}
}

/******************************************************************************
 InsertExtDroppedFiles (private)

 ******************************************************************************/

void
CBProjectTable::InsertExtDroppedFiles
	(
	const JPtrArray<JString>&		fileNameList,
	const CBRelPathCSF::PathType	pathType
	)
{
	if (fileNameList.IsEmpty())
		{
		return;
		}

	if (itsDNDAction == kDNDFileBombsight ||
		itsDNDAction == kDNDAppendToGroup)
		{
		SelectSingleCell(itsDNDCell, kJFalse);
		}
	else
		{
		ClearSelection();
		}

	const DNDAction saveAction = itsDNDAction;
	itsDNDAction               = kDNDInvalid;	// pg in AddFiles() can call Draw()
	const JBoolean added       = AddFiles(fileNameList, pathType);
	itsDNDAction               = saveAction;

	// if insert before first in group, it actually appends

	if (added && itsDNDAction == kDNDFileBombsight &&
		GetDepth(itsDNDCell.y) == kGroupDepth)
		{
		JTreeNode* node = GetTreeList()->GetNode(itsDNDCell.y);
		InsertFileSelectionAfter(node);
		}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
CBProjectTable::UpdateEditMenu()
{
JBoolean ok;
JIndex index;

	if (!HasFocus())
		{
		return;
		}

	JXTEBase* te = GetEditMenuHandler();

	if (HasSelection())
		{
		ok = te->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &index);
		assert( ok );
		itsEditMenu->EnableItem(index);

		ok = te->EditMenuCmdToIndex(JTextEditor::kDeleteSelCmd, &index);
		assert( ok );
		itsEditMenu->EnableItem(index);
		}

	ok = te->EditMenuCmdToIndex(JTextEditor::kSelectAllCmd, &index);
	assert( ok );
	itsEditMenu->EnableItem(index);
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
CBProjectTable::HandleEditMenu
	(
	const JIndex index
	)
{
	if (!HasFocus())
		{
		return;
		}

	JXTEBase* te = GetEditMenuHandler();

	JTextEditor::CmdIndex cmd;
	if (te->EditMenuIndexToCmd(index, &cmd))
		{
		if (cmd == JTextEditor::kCopyCmd)
			{
			CopySelectedNames();
			}
		else if (cmd == JTextEditor::kSelectAllCmd)
			{
			SelectAll();
			}
		else if (cmd == JTextEditor::kDeleteSelCmd)
			{
			RemoveSelection();
			}
		}
}

/******************************************************************************
 CopySelectedNames

	Copies the names of the selected items to the text clipboard.

 ******************************************************************************/

void
CBProjectTable::CopySelectedNames()
	const
{
	const JTableSelection& s = GetTableSelection();
	if (s.HasSelection())
		{
		JPtrArray<JString> list(JPtrArrayT::kForgetAll);

		JTableSelectionIterator iter(&s);
		JPoint cell;
		while (iter.Next(&cell))
			{
			const JString& name = (GetNamedTreeNode(cell.y))->GetName();
			list.Append(const_cast<JString*>(&name));
			}

		JXTextSelection* data = jnew JXTextSelection(GetDisplay(), list);
		assert( data != nullptr );

		GetSelectionManager()->SetData(kJXClipboardName, data);
		}
}

/******************************************************************************
 CreateContextMenu (private)

 ******************************************************************************/

#include "jcc_compare_backup.xpm"
#include "jcc_compare_vcs.xpm"

void
CBProjectTable::CreateContextMenu()
{
	if (itsContextMenu == nullptr)
		{
		itsContextMenu = jnew JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsContextMenu != nullptr );
		itsContextMenu->SetMenuItems(kContextMenuStr, "CBProjectTable");
		itsContextMenu->SetUpdateAction(JXMenu::kDisableNone);
		itsContextMenu->SetToHiddenPopupMenu();
		ListenTo(itsContextMenu);

		itsContextMenu->SetItemImage(kDiffSmartCmd, jcc_compare_backup);
		itsContextMenu->SetItemImage(kDiffVCSCmd,   jcc_compare_vcs);
		}
}

/******************************************************************************
 UpdateContextMenu (private)

 ******************************************************************************/

void
CBProjectTable::UpdateContextMenu()
{
	SelType selType;
	JBoolean single;
	JIndex index;
	const JBoolean hasSelection = GetSelectionType(&selType, &single, &index);
	if (hasSelection && selType == kFileSelection)
		{
		itsContextMenu->SetItemText(kOpenFilesCmd, kOpenFilesItemStr);

		itsContextMenu->EnableItem(kRemoveSelCmd);
		itsContextMenu->EnableItem(kOpenFilesCmd);
		itsContextMenu->EnableItem(kOpenComplFilesCmd);
		itsContextMenu->SetItemEnable(kEditPathCmd, single);
		itsContextMenu->SetItemEnable(kEditSubprojConfigCmd,
			JI2B(single && (GetProjectNode(index))->GetType() == kCBLibraryNT));
		itsContextMenu->EnableItem(kDiffSmartCmd);
		itsContextMenu->EnableItem(kDiffVCSCmd);
		itsContextMenu->EnableItem(kShowLocationCmd);
		}
	else if (hasSelection)
		{
		itsContextMenu->SetItemText(kOpenFilesCmd, kEditGroupNameItemStr);

		itsContextMenu->EnableItem(kRemoveSelCmd);
		itsContextMenu->SetItemEnable(kOpenFilesCmd, single);
		itsContextMenu->DisableItem(kOpenComplFilesCmd);
		itsContextMenu->DisableItem(kEditPathCmd);
		itsContextMenu->DisableItem(kEditSubprojConfigCmd);
		itsContextMenu->EnableItem(kDiffSmartCmd);
		itsContextMenu->EnableItem(kDiffVCSCmd);
		itsContextMenu->EnableItem(kShowLocationCmd);
		}
	else
		{
		itsContextMenu->SetItemText(kOpenFilesCmd, kOpenFilesItemStr);

		itsContextMenu->DisableItem(kRemoveSelCmd);
		itsContextMenu->DisableItem(kOpenFilesCmd);
		itsContextMenu->DisableItem(kOpenComplFilesCmd);
		itsContextMenu->DisableItem(kEditPathCmd);
		itsContextMenu->DisableItem(kEditSubprojConfigCmd);
		itsContextMenu->DisableItem(kDiffSmartCmd);
		itsContextMenu->DisableItem(kDiffVCSCmd);
		itsContextMenu->DisableItem(kShowLocationCmd);
		}
}

/******************************************************************************
 HandleContextMenu (private)

 ******************************************************************************/

void
CBProjectTable::HandleContextMenu
	(
	const JIndex index
	)
{
	(CBGetDocumentManager())->SetActiveProjectDocument(itsDoc);

	if (!EndEditing())
		{
		return;
		}

	if (index == kNewGroupCmd)
		{
		NewGroup();
		}
	else if (index == kAddFilesCmd)
		{
		AddFiles();
		}
	else if (index == kAddDirTreeCmd)
		{
		AddDirectoryTree();
		}
	else if (index == kRemoveSelCmd)
		{
		RemoveSelection();
		}

	else if (index == kOpenFilesCmd)
		{
		OpenSelection();
		}
	else if (index == kOpenComplFilesCmd)
		{
		OpenComplementFiles();
		}

	else if (index == kEditPathCmd)
		{
		EditFilePath();
		}
	else if (index == kEditSubprojConfigCmd)
		{
		EditSubprojectConfig();
		}

	else if (index == kDiffSmartCmd)
		{
		PlainDiffSelection();
		}
	else if (index == kDiffVCSCmd)
		{
		VCSDiffSelection();
		}
	else if (index == kShowLocationCmd)
		{
		ShowSelectedFileLocations();
		}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
CBProjectTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsCSFButton == nullptr );

	ClearSelection();

	JXInputField* inputField =
		JXNamedTreeListWidget::CreateXInputField(cell, x,y, w,h);
	inputField->SetIsRequired();
	inputField->SetCharacterInWordFunction(JXPathInput::IsCharacterInWord);

	if (GetDepth(cell.y) == kFileDepth)
		{
		CBFileNodeBase* node =
			dynamic_cast<CBFileNodeBase*>(GetProjectNode(cell.y));
		assert( node != nullptr );
		inputField->SetText(node->GetFileName());

		JRect r;
		JBoolean ok = GetImageRect(cell.y, &r);
		assert( ok );
		r.Expand(kJXDefaultBorderWidth, kJXDefaultBorderWidth);
		itsCSFButton = jnew JXImageButton(this, kFixedLeft, kFixedTop,
										 r.left, r.top, r.width(), r.height());
		assert( itsCSFButton != nullptr );

		const JXImage* image;
		ok = GetImage(cell.y, &image);
		assert( ok );
		itsCSFButton->SetImage(const_cast<JXImage*>(image), kJFalse);

		itsCSFButton->SetHint(JGetString("CSFButtonHint::CBProjectTable"));
		ListenTo(itsCSFButton);
		}

	return inputField;
}

/******************************************************************************
 CreateTreeListInput (virtual protected)

 ******************************************************************************/

JXInputField*
CBProjectTable::CreateTreeListInput
	(
	const JPoint&		cell,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	if (GetDepth(cell.y) == kFileDepth)
		{
		CBProjectTableInput* obj =
			jnew CBProjectTableInput(this, enclosure, hSizing, vSizing, x,y, w,h);
		assert( obj != nullptr );
		obj->SetBasePath(itsDoc->GetFilePath());
		return obj;
		}
	else
		{
		return JXNamedTreeListWidget::CreateTreeListInput(cell, enclosure, hSizing, vSizing, x,y, w,h);
		}
}

/******************************************************************************
 ExtractInputData (virtual protected)

	Derived class must override to extract the information from its active
	input field, check it, and delete the input field if successful.

	Should return kJTrue if the data is valid and the process succeeded.

 ******************************************************************************/

JBoolean
CBProjectTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	JBoolean ok = kJFalse;
	if (GetDepth(cell.y) == kFileDepth)
		{
		JXInputField* inputField;
		const JBoolean ok1 = GetXInputField(&inputField);
		assert( ok1 );

		if (inputField->InputValid())
			{
			CBFileNodeBase* node =
				dynamic_cast<CBFileNodeBase*>(GetProjectNode(cell.y));
			assert( node != nullptr );

			const JString origName = node->GetFileName();

			JString origFullName, newFullName;
			const JBoolean existed = node->GetFullName(&origFullName);

			node->SetFileName(inputField->GetText());

			const JBoolean exists = node->GetFullName(&newFullName);

			JBoolean success = kJTrue;
			if (itsInputAction == kRename && existed && !newFullName.IsEmpty() &&
				!JSameDirEntry(origFullName, newFullName))
				{
				JBoolean replace = kJTrue;
				if (exists)
					{
					const JCharacter* map[] =
						{
						"f", newFullName
						};
					const JString msg = JGetString("OKToReplace::CBProjectTable", map, sizeof(map));
					replace = (JGetUserNotification())->AskUserNo(msg);
					}
				else
					{
					JString p, n;
					JSplitPathAndName(newFullName, &p, &n);
					if (!JDirectoryExists(p))
						{
						JCreateDirectory(p);
						}
					}

				if (replace)
					{
					const JError err = JRenameVCS(origFullName, newFullName);
					if (err.OK())
						{
						CBCleanUpAfterRename(origFullName, nullptr);
						}
					else
						{
						err.ReportIfError();
						success = kJFalse;
						}
					}
				else
					{
					success = kJFalse;
					}
				}

			if (success)
				{
				JString path;
				if (node->GetFullName(&path) &&
					!(itsDoc->GetDirectories()).Contains(path))
					{
					itsDoc->DelayUpdateSymbolDatabase();
					}
				}
			else
				{
				node->SetFileName(origName);
				}

			ok = kJTrue;
			}
		}
	else
		{
		ok = JXNamedTreeListWidget::ExtractInputData(cell);
		}

	if (ok)
		{
		SelectSingleCell(cell, kJFalse);
		}
	return ok;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
CBProjectTable::PrepareDeleteXInputField()
{
	JXNamedTreeListWidget::PrepareDeleteXInputField();

	JXDeleteObjectTask<JBroadcaster>::Delete(itsCSFButton);
	itsCSFButton = nullptr;
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBProjectTable::ReadSetup
	(
	std::istream&			projInput,
	const JFileVersion	projVers,
	std::istream*			setInput,
	const JFileVersion	setVers
	)
{
	const JBoolean useProjData = JI2B( setInput == nullptr || setVers < 71 );
	if (!useProjData)
		{
		JNamedTreeList* treeList = GetNamedTreeList();

		JString groupName;
		while (1)
			{
			JBoolean keepGoing;
			*setInput >> keepGoing;
			if (setInput->fail() || !keepGoing)
				{
				break;
				}

			*setInput >> groupName;

			JIndex i;
			if (treeList->Find(groupName, &i))
				{
				treeList->Open(i);
				}
			}

		ReadScrollSetup(*setInput);
		}

	// have to read from the project file, even if we don't need it

	if (34 <= projVers && projVers < 71)
		{
		JSize groupCount;
		projInput >> groupCount;

		JTreeList* treeList = GetTreeList();
		for (JIndex i=groupCount; i>=1; i--)
			{
			JBoolean isOpen;
			projInput >> isOpen;
			if (useProjData && isOpen && treeList->IndexValid(i))
				{
				treeList->Open(i);
				}
			}

		if (useProjData)
			{
			ReadScrollSetup(projInput);
			}
		else
			{
			SkipScrollSetup(projInput);
			}
		}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CBProjectTable::WriteSetup
	(
	std::ostream& projOutput,
	std::ostream* setOutput
	)
	const
{
	if (setOutput != nullptr)
		{
		const JTreeList* treeList = GetTreeList();
		const JTreeNode* rootNode = (treeList->GetTree())->GetRoot();

		const JSize groupCount = rootNode->GetChildCount();
		for (JIndex i=1; i<=groupCount; i++)
			{
			const JNamedTreeNode* child =
				dynamic_cast<const JNamedTreeNode*>(rootNode->GetChild(i));
			assert( child != nullptr );

			if (treeList->IsOpen(child))
				{
				*setOutput << ' ' << kJTrue;
				*setOutput << ' ' << child->GetName();
				}
			}

		*setOutput << ' ' << kJFalse << ' ';
		WriteScrollSetup(*setOutput);

		*setOutput << ' ';
		}
}

/******************************************************************************
 Global functions for CBProjectTable

 ******************************************************************************/

/******************************************************************************
 Global functions for CBProjectTable::DropFileAction

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&						input,
	CBProjectTable::DropFileAction&	action
	)
{
	long temp;
	input >> temp;
	action = (CBProjectTable::DropFileAction) temp;
	assert( action == CBProjectTable::kAskPathType ||
			action == CBProjectTable::kAbsolutePath ||
			action == CBProjectTable::kProjectRelative ||
			action == CBProjectTable::kHomeRelative );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&								output,
	const CBProjectTable::DropFileAction	action
	)
{
	output << (long) action;
	return output;
}
