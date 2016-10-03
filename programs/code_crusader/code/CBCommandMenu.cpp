/******************************************************************************
 CBCommandMenu.cpp

	This menu is an action menu, so the only message that is broadcast that
	is meaningful to outsiders is GetTargetInfo.  This message must be
	caught with ReceiveWithFeedback().

	BASE CLASS = JXTextMenu

	Copyright (C) 2001-02 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBCommandMenu.h"
#include "CBCommandManager.h"
#include "CBEditCommandsDialog.h"
#include "CBRunCommandDialog.h"
#include "CBProjectDocument.h"
#include "CBSymbolDirector.h"
#include "CBCTreeDirector.h"
#include "CBJavaTreeDirector.h"
#include "CBPHPTreeDirector.h"
#include "CBFileListDirector.h"
#include "CBTextDocument.h"
#include "CBRelPathCSF.h"
#include "cbGlobals.h"
#include "cbActionDefs.h"
#include <jAssert.h>

// Project menu

static const JCharacter* kMenuTitleStr = "Tasks";
static const JCharacter* kMenuStr =
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

static const JCharacter* kAddToProjectItemText  = "Add to project: ";
static const JCharacter* kManageProjectItemText = "Manage project: ";
static const JCharacter* kNoProjectName         = "none";

// Add to Project menu

static const JCharacter* kAddToProjMenuStr =
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
#include "jcc_show_java_tree.xpm"
#include "jcc_show_php_tree.xpm"
#include "jcc_show_file_list.xpm"

static const JCharacter* kManageProjMenuStr =
	"    Update symbol database         %k Meta-U       %i" kCBUpdateClassTreeAction
	"%l| Show symbol browser            %k Ctrl-F12     %i" kCBShowSymbolBrowserAction
	"  | Show C++ class tree                            %i" kCBShowCPPClassTreeAction
	"  | Show Java class tree                           %i" kCBShowJavaClassTreeAction
	"  | Show PHP class tree                            %i" kCBShowPHPClassTreeAction
	"  | Show file list                 %k Meta-Shift-F %i" kCBShowFileListAction;

enum
{
	kUpdateSymbolDBCmd = 1,
	kShowSymbolBrowserCmd,
	kShowCTreeCmd,
	kShowJavaTreeCmd,
	kShowPHPTreeCmd,
	kShowFileListCmd
};

// JBroadcaster message types

const JCharacter* CBCommandMenu::kGetTargetInfo = "GetTargetInfo::CBCommandMenu";

/******************************************************************************
 Constructor

	doc can be NULL

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
	JXTextMenu(kMenuTitleStr, enclosure, hSizing, vSizing, x,y, w,h)
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
	assert( projDoc != NULL || textDoc != NULL );

	SetProjectDocument(projDoc);
	itsTextDoc        = textDoc;
	itsAddToProjMenu  = NULL;
	itsManageProjMenu = NULL;

	SetMenuItems(kMenuStr);
	SetUpdateAction(kDisableNone);
	ListenTo(this);
	ListenTo(CBGetCommandManager());

	if (itsTextDoc != NULL)
		{
		itsAddToProjMenu = new JXTextMenu(this, kAddToProjIndex, GetEnclosure());
		assert( itsAddToProjMenu != NULL );
		itsAddToProjMenu->SetMenuItems(kAddToProjMenuStr, "CBCommandMenu");
		itsAddToProjMenu->SetUpdateAction(JXMenu::kDisableNone);
		ListenTo(itsAddToProjMenu);

		itsManageProjMenu = new JXTextMenu(this, kManageProjIndex, GetEnclosure());
		assert( itsManageProjMenu != NULL );
		itsManageProjMenu->SetMenuItems(kManageProjMenuStr, "CBCommandMenu");
		itsManageProjMenu->SetUpdateAction(JXMenu::kDisableNone);
		ListenTo(itsManageProjMenu);

		itsManageProjMenu->SetItemImage(kShowSymbolBrowserCmd, jcc_show_symbol_list);
		itsManageProjMenu->SetItemImage(kShowCTreeCmd,         jcc_show_c_tree);
		itsManageProjMenu->SetItemImage(kShowJavaTreeCmd,      jcc_show_java_tree);
		itsManageProjMenu->SetItemImage(kShowPHPTreeCmd,       jcc_show_php_tree);
		itsManageProjMenu->SetItemImage(kShowFileListCmd,      jcc_show_file_list);
		}

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
	if (itsProjDoc != NULL)
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
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );

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
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleAddToProjectMenu(selection->GetIndex());
		}

	else if (sender == itsManageProjMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateManageProjectMenu();
		}
	else if (sender == itsManageProjMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
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
	if (itsTextDoc == NULL)
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

	CBProjectDocument* projDoc = itsProjDoc;
	if (projDoc == NULL)
		{
		(CBGetDocumentManager())->GetActiveProjectDocument(&projDoc);
		}
	const JBoolean hasProject = JI2B( projDoc != NULL );

	(CBGetCommandManager())->AppendMenuItems(this, hasProject);

	if (projDoc != NULL)
		{
		(projDoc->GetCommandManager())->AppendMenuItems(this, hasProject);
		}

	// "Add to" sub-menu

	SetItemEnable(kAddToProjIndex, CanAddToProject());

	JString itemText = kAddToProjectItemText;
	if (projDoc != NULL)
		{
		itemText += projDoc->GetName();
		}
	else
		{
		itemText += kNoProjectName;
		}
	SetItemText(kAddToProjIndex, itemText);

	// "Manage" sub-menu

	SetItemEnable(kManageProjIndex, CanManageProject());

	itemText = kManageProjectItemText;
	if (projDoc != NULL)
		{
		itemText += projDoc->GetName();
		}
	else
		{
		itemText += kNoProjectName;
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
	if (projDoc != NULL)
		{
		(CBGetDocumentManager())->SetActiveProjectDocument(projDoc);
		}
	else
		{
		(CBGetDocumentManager())->GetActiveProjectDocument(&projDoc);
		}

	if (index == kRunCmd)
		{
		CBRunCommandDialog* dlog;
		if (itsTextDoc != NULL)
			{
			dlog = new CBRunCommandDialog(projDoc, itsTextDoc);
			}
		else
			{
			dlog = new CBRunCommandDialog(projDoc, info.GetFileList(), info.GetLineIndexList());
			}
		assert( dlog != NULL );
		dlog->BeginDialog();
		}
	else if (index == kEditCmd)
		{
		CBEditCommandsDialog* dlog = new CBEditCommandsDialog(projDoc);
		assert( dlog != NULL );
		dlog->BeginDialog();
		}

	else if (index > kInitCmdCount)
		{
		JIndex i = index - kInitCmdCount;

		CBCommandManager* mgr = CBGetCommandManager();
		if (i > mgr->GetCommandCount())
			{
			assert( projDoc != NULL );
			i  -= mgr->GetCommandCount();		// before changing mgr
			mgr = projDoc->GetCommandManager();
			}

		if (itsTextDoc != NULL)
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

	CBProjectDocument* projDoc = itsProjDoc;
	if (projDoc == NULL)
		{
		(CBGetDocumentManager())->GetActiveProjectDocument(&projDoc);
		}

	JBoolean onDisk;
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

JBoolean
CBCommandMenu::CanAddToProject()
	const
{
	return JI2B(itsProjDoc == NULL &&
				(CBGetDocumentManager())->HasProjectDocuments() &&
				itsTextDoc != NULL &&
				itsTextDoc->ExistsOnDisk());
}

/******************************************************************************
 UpdateManageProjectMenu (private)

 ******************************************************************************/

void
CBCommandMenu::UpdateManageProjectMenu()
{
	if (CanManageProject())
		{
		itsManageProjMenu->EnableAll();
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
	if (!CanManageProject())
		{
		return;
		}

	CBProjectDocument* projDoc = itsProjDoc;
	if (projDoc == NULL)
		{
		(CBGetDocumentManager())->GetActiveProjectDocument(&projDoc);
		}

	if (index == kUpdateSymbolDBCmd)
		{
		projDoc->UpdateSymbolDatabase();
		}
	else if (index == kShowSymbolBrowserCmd)
		{
		(projDoc->GetSymbolDirector())->Activate();
		}
	else if (index == kShowCTreeCmd)
		{
		(projDoc->GetCTreeDirector())->Activate();
		}
	else if (index == kShowJavaTreeCmd)
		{
		(projDoc->GetJavaTreeDirector())->Activate();
		}
	else if (index == kShowPHPTreeCmd)
		{
		(projDoc->GetPHPTreeDirector())->Activate();
		}
	else if (index == kShowFileListCmd)
		{
		(projDoc->GetFileListDirector())->Activate();
		}
}

/******************************************************************************
 CanManageProject (private)

 ******************************************************************************/

JBoolean
CBCommandMenu::CanManageProject()
	const
{
	return JI2B((CBGetDocumentManager())->HasProjectDocuments() &&
				itsTextDoc != NULL);
}
