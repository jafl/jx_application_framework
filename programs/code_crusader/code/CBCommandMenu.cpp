/******************************************************************************
 CBCommandMenu.cpp

	This menu is an action menu, so the only message that is broadcast that
	is meaningful to outsiders is GetTargetInfo.  This message must be
	caught with ReceiveWithFeedback().

	BASE CLASS = JXTextMenu

	Copyright © 2001-02 by John Lindal.

 ******************************************************************************/

#include "CBCommandMenu.h"
#include "CBCommandManager.h"
#include "CBEditCommandsDialog.h"
#include "CBRunCommandDialog.h"
#include "CBProjectDocument.h"
#include "CBSymbolDirector.h"
#include "CBCTreeDirector.h"
#include "CBDTreeDirector.h"
#include "CBGoTreeDirector.h"
#include "CBJavaTreeDirector.h"
#include "CBPHPTreeDirector.h"
#include "CBFileListDirector.h"
#include "CBTextDocument.h"
#include "CBTree.h"
#include "CBRelPathCSF.h"
#include "cbGlobals.h"
#include "cbActionDefs.h"
#include <jAssert.h>

// Project menu

static const JUtf8Byte* kMenuStr =
	"    Perform one-off task..."
	"  | Customize this menu..."
	"%l| Add to project: none"
	"  | Manage project: none"
	"%l";

enum
{
	kRunCmd = 1,
	kEditCmd,
	kAddToProjIndex,
	kManageProjIndex,	// = kInitCmdCount

	kInitCmdCount = kManageProjIndex
};

// Add to Project menu

static const JUtf8Byte* kAddToProjMenuStr =
	"  Absolute path                              %i" kCBAddToProjAbsoluteAction
	"| Relative to project file   %k Meta-Shift-A %i" kCBAddToProjProjRelativeAction
	"| Relative to home directory                 %i" kCBAddToProjHomeRelativeAction;

enum
{
	kAddToProjAbsoluteCmd = 1,
	kAddToProjProjRelativeCmd,
	kAddToProjHomeRelativeCmd
};

// Manage Project menu

#include "jcc_show_symbol_list.xpm"
#include "jcc_show_c_tree.xpm"
#include "jcc_show_d_tree.xpm"
#include "jcc_show_go_tree.xpm"
#include "jcc_show_java_tree.xpm"
#include "jcc_show_php_tree.xpm"
#include "jcc_show_file_list.xpm"

static const JUtf8Byte* kManageProjMenuStr =
	"    Update symbol database         %k Meta-U       %i" kCBUpdateClassTreeAction
	"%l| Show symbol browser            %k Ctrl-F12     %i" kCBShowSymbolBrowserAction
	"  | Show C++ class tree                            %i" kCBShowCPPClassTreeAction
	"  | Show D class tree                              %i" kCBShowDClassTreeAction
	"  | Show Go struct/interface tree                  %i" kCBShowGoClassTreeAction
	"  | Show Java class tree                           %i" kCBShowJavaClassTreeAction
	"  | Show PHP class tree                            %i" kCBShowPHPClassTreeAction
	"  | Show file list                 %k Meta-Shift-F %i" kCBShowFileListAction;

enum
{
	kUpdateSymbolDBCmd = 1,
	kShowSymbolBrowserCmd,
	kShowCTreeCmd,
	kShowDTreeCmd,
	kShowGoTreeCmd,
	kShowJavaTreeCmd,
	kShowPHPTreeCmd,
	kShowFileListCmd
};

// JBroadcaster message types

const JUtf8Byte* CBCommandMenu::kGetTargetInfo = "GetTargetInfo::CBCommandMenu";

/******************************************************************************
 Constructor

	doc can be nullptr

 ******************************************************************************/

CBCommandMenu::CBCommandMenu
	(
	CBProjectDocument*		projDoc,
	CBTextDocument*			textDoc,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXTextMenu(JGetString("Title::CBCommandMenu"), enclosure, hSizing, vSizing, x,y, w,h)
{
	CBCommandMenuX(projDoc, textDoc);
}

CBCommandMenu::CBCommandMenu
	(
	CBProjectDocument*		projDoc,
	CBTextDocument*			textDoc,
	JXMenu*					owner,
	const JIndex			itemIndex,
	JXContainer*			enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	CBCommandMenuX(projDoc, textDoc);
}

// private

void
CBCommandMenu::CBCommandMenuX
	(
	CBProjectDocument*	projDoc,
	CBTextDocument*		textDoc
	)
{
	assert( projDoc != nullptr || textDoc != nullptr );

	SetProjectDocument(projDoc);
	itsTextDoc        = textDoc;
	itsAddToProjMenu  = nullptr;
	itsManageProjMenu = nullptr;

	SetMenuItems(kMenuStr);
	SetUpdateAction(kDisableNone);
	ListenTo(this);
	ListenTo(CBGetCommandManager());

	if (itsTextDoc != nullptr)
		{
		itsAddToProjMenu = jnew JXTextMenu(this, kAddToProjIndex, GetEnclosure());
		assert( itsAddToProjMenu != nullptr );
		itsAddToProjMenu->SetMenuItems(kAddToProjMenuStr, "CBCommandMenu");
		itsAddToProjMenu->SetUpdateAction(JXMenu::kDisableNone);
		ListenTo(itsAddToProjMenu);
		}

	itsManageProjMenu = jnew JXTextMenu(this, kManageProjIndex, GetEnclosure());
	assert( itsManageProjMenu != nullptr );
	itsManageProjMenu->SetMenuItems(kManageProjMenuStr, "CBCommandMenu");
	itsManageProjMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsManageProjMenu);

	itsManageProjMenu->SetItemImage(kShowSymbolBrowserCmd, jcc_show_symbol_list);
	itsManageProjMenu->SetItemImage(kShowCTreeCmd,         jcc_show_c_tree);
	itsManageProjMenu->SetItemImage(kShowDTreeCmd,         jcc_show_d_tree);
	itsManageProjMenu->SetItemImage(kShowGoTreeCmd,        jcc_show_go_tree);
	itsManageProjMenu->SetItemImage(kShowJavaTreeCmd,      jcc_show_java_tree);
	itsManageProjMenu->SetItemImage(kShowPHPTreeCmd,       jcc_show_php_tree);
	itsManageProjMenu->SetItemImage(kShowFileListCmd,      jcc_show_file_list);

	UpdateMenu();	// build menu so shortcuts work
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCommandMenu::~CBCommandMenu()
{
}

/******************************************************************************
 SetProjectDocument

 ******************************************************************************/

void
CBCommandMenu::SetProjectDocument
	(
	CBProjectDocument* projDoc
	)
{
	itsProjDoc = projDoc;
	if (itsProjDoc != nullptr)
		{
		ClearWhenGoingAway(itsProjDoc, &itsProjDoc);
		}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
CBCommandMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateMenu();
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );

		GetTargetInfo info;
		BuildTargetInfo(&info);
		HandleSelection(selection->GetIndex(), info);
		}

	else if (sender == itsAddToProjMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateAddToProjectMenu();
		}
	else if (sender == itsAddToProjMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleAddToProjectMenu(selection->GetIndex());
		}

	else if (sender == itsManageProjMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateManageProjectMenu();
		}
	else if (sender == itsManageProjMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleManageProjectMenu(selection->GetIndex());
		}

	else if (sender == CBGetCommandManager() && message.Is(CBCommandManager::kUpdateCommandMenu))
		{
		UpdateMenu();
		}

	else
		{
		JXTextMenu::Receive(sender, message);
		}
}

/******************************************************************************
 BuildTargetInfo (private)

 ******************************************************************************/

void
CBCommandMenu::BuildTargetInfo
	(
	GetTargetInfo* info
	)
{
	if (itsTextDoc == nullptr)
		{
		BroadcastWithFeedback(info);
		}
}

/******************************************************************************
 UpdateMenu (private)

 ******************************************************************************/

void
CBCommandMenu::UpdateMenu()
{
	while (GetItemCount() > kInitCmdCount)
		{
		RemoveItem(GetItemCount());
		}

	CBProjectDocument* projDoc;
	const bool hasProject = GetProjectDocument(&projDoc);

	(CBGetCommandManager())->AppendMenuItems(this, hasProject);

	if (projDoc != nullptr)
		{
		(projDoc->GetCommandManager())->AppendMenuItems(this, hasProject);
		}

	// "Add to" sub-menu

	SetItemEnable(kAddToProjIndex, CanAddToProject());

	JString itemText = JGetString("AddToProjectItemText::CBCommandMenu");
	if (projDoc != nullptr)
		{
		itemText += projDoc->GetName();
		}
	else
		{
		itemText += JGetString("NoProjectName::CBCommandMenu");
		}
	SetItemText(kAddToProjIndex, itemText);

	// "Manage" sub-menu

	SetItemEnable(kManageProjIndex, hasProject);

	itemText = JGetString("ManageProjectItemText::CBCommandMenu");
	if (projDoc != nullptr)
		{
		itemText += projDoc->GetName();
		}
	else
		{
		itemText += JGetString("NoProjectName::CBCommandMenu");
		}
	SetItemText(kManageProjIndex, itemText);
}

/******************************************************************************
 HandleSelection (private)

 ******************************************************************************/

void
CBCommandMenu::HandleSelection
	(
	const JIndex			index,
	const GetTargetInfo&	info
	)
{
	CBProjectDocument* projDoc = itsProjDoc;
	if (projDoc != nullptr)
		{
		CBGetDocumentManager()->SetActiveProjectDocument(projDoc);
		}
	else
		{
		CBGetDocumentManager()->GetActiveProjectDocument(&projDoc);
		}

	if (index == kRunCmd)
		{
		CBRunCommandDialog* dlog;
		if (itsTextDoc != nullptr)
			{
			dlog = jnew CBRunCommandDialog(projDoc, itsTextDoc);
			}
		else
			{
			dlog = jnew CBRunCommandDialog(projDoc, info.GetFileList(), info.GetLineIndexList());
			}
		assert( dlog != nullptr );
		dlog->BeginDialog();
		}
	else if (index == kEditCmd)
		{
		auto* dlog = jnew CBEditCommandsDialog(projDoc);
		assert( dlog != nullptr );
		dlog->BeginDialog();
		}

	else if (index > kInitCmdCount)
		{
		JIndex i = index - kInitCmdCount;

		CBCommandManager* mgr = CBGetCommandManager();
		if (i > mgr->GetCommandCount())
			{
			assert( projDoc != nullptr );
			i  -= mgr->GetCommandCount();		// before changing mgr
			mgr = projDoc->GetCommandManager();
			}

		if (itsTextDoc != nullptr)
			{
			mgr->Exec(i, projDoc, itsTextDoc);
			}
		else
			{
			mgr->Exec(i, projDoc, info.GetFileList(), info.GetLineIndexList());
			}
		}
}

/******************************************************************************
 UpdateAddToProjectMenu (private)

 ******************************************************************************/

void
CBCommandMenu::UpdateAddToProjectMenu()
{
	if (CanAddToProject())
		{
		itsAddToProjMenu->EnableAll();
		}
	else
		{
		itsAddToProjMenu->DisableAll();
		}
}

/******************************************************************************
 HandleAddToProjectMenu (private)

 ******************************************************************************/

void
CBCommandMenu::HandleAddToProjectMenu
	(
	const JIndex index
	)
{
	if (!CanAddToProject())
		{
		return;
		}

	CBProjectDocument* projDoc;
	const bool hasProject = GetProjectDocument(&projDoc);
	assert( hasProject );

	bool onDisk;
	const JString fullName = itsTextDoc->GetFullName(&onDisk);
	assert( onDisk );

	CBRelPathCSF::PathType pathType;
	if (index == kAddToProjAbsoluteCmd)
		{
		pathType = CBRelPathCSF::kAbsolutePath;
		}
	else if (index == kAddToProjProjRelativeCmd)
		{
		pathType = CBRelPathCSF::kProjectRelative;
		}
	else
		{
		assert( index == kAddToProjHomeRelativeCmd );
		pathType = CBRelPathCSF::kHomeRelative;
		}

	projDoc->AddFile(fullName, pathType);
}

/******************************************************************************
 CanAddToProject (private)

 ******************************************************************************/

bool
CBCommandMenu::CanAddToProject()
	const
{
	return itsProjDoc == nullptr &&
				CBGetDocumentManager()->HasProjectDocuments() &&
				itsTextDoc != nullptr &&
				itsTextDoc->ExistsOnDisk();
}

/******************************************************************************
 UpdateManageProjectMenu (private)

 ******************************************************************************/

void
CBCommandMenu::UpdateManageProjectMenu()
{
	CBProjectDocument* projDoc;
	if (GetProjectDocument(&projDoc))
		{
		itsManageProjMenu->EnableAll();

		itsManageProjMenu->SetItemEnable(kShowCTreeCmd,
			!projDoc->GetCTreeDirector()->GetTree()->IsEmpty());
		itsManageProjMenu->SetItemEnable(kShowDTreeCmd,
			!projDoc->GetDTreeDirector()->GetTree()->IsEmpty());
		itsManageProjMenu->SetItemEnable(kShowGoTreeCmd,
			!projDoc->GetGoTreeDirector()->GetTree()->IsEmpty());
		itsManageProjMenu->SetItemEnable(kShowJavaTreeCmd,
			!projDoc->GetJavaTreeDirector()->GetTree()->IsEmpty());
		itsManageProjMenu->SetItemEnable(kShowPHPTreeCmd,
			!projDoc->GetPHPTreeDirector()->GetTree()->IsEmpty());
		}
	else
		{
		itsManageProjMenu->DisableAll();
		}
}

/******************************************************************************
 HandleManageProjectMenu (private)

 ******************************************************************************/

void
CBCommandMenu::HandleManageProjectMenu
	(
	const JIndex index
	)
{
	CBProjectDocument* projDoc;
	if (!GetProjectDocument(&projDoc))
		{
		return;
		}

	if (index == kUpdateSymbolDBCmd)
		{
		projDoc->UpdateSymbolDatabase();
		}
	else if (index == kShowSymbolBrowserCmd)
		{
		projDoc->GetSymbolDirector()->Activate();
		}
	else if (index == kShowCTreeCmd)
		{
		projDoc->GetCTreeDirector()->Activate();
		}
	else if (index == kShowDTreeCmd)
		{
		projDoc->GetDTreeDirector()->Activate();
		}
	else if (index == kShowGoTreeCmd)
		{
		projDoc->GetGoTreeDirector()->Activate();
		}
	else if (index == kShowJavaTreeCmd)
		{
		projDoc->GetJavaTreeDirector()->Activate();
		}
	else if (index == kShowPHPTreeCmd)
		{
		projDoc->GetPHPTreeDirector()->Activate();
		}
	else if (index == kShowFileListCmd)
		{
		projDoc->GetFileListDirector()->Activate();
		}
}

/******************************************************************************
 GetProjectDocument (private)

 ******************************************************************************/

bool
CBCommandMenu::GetProjectDocument
	(
	CBProjectDocument** projDoc
	)
	const
{
	*projDoc = itsProjDoc;
	if (*projDoc == nullptr)
		{
		CBGetDocumentManager()->GetActiveProjectDocument(projDoc);
		}

	return *projDoc != nullptr;
}
