/******************************************************************************
 CBSymbolSRDirector.cpp

	Window to display result of searching CBSymbolList.

	BASE CLASS = JXWindowDirector

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CBSymbolSRDirector.h"
#include "CBSymbolDirector.h"
#include "CBSymbolTable.h"
#include "CBSymbolList.h"
#include "CBProjectDocument.h"
#include "CBCTreeDirector.h"
#include "CBDTreeDirector.h"
#include "CBGoTreeDirector.h"
#include "CBJavaTreeDirector.h"
#include "CBPHPTreeDirector.h"
#include "CBTree.h"
#include "CBCommandMenu.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXScrollbarSet.h>
#include <jGlobals.h>
#include <jAssert.h>

// Actions menu

static const JUtf8Byte* kActionsMenuStr =
	"    Copy selected names           %k Meta-C"
	"%l| Update                        %k Meta-U"
	"  | Show C++ class tree           %k Meta-Shift-C"
	"  | Show D class tree             %k Meta-Shift-D"
	"  | Show Go struct/interface tree %k Meta-Shift-G"
	"  | Show Java class tree          %k Meta-Shift-J"
	"  | Show PHP class tree           %k Meta-Shift-P"
	"%l| Close window                  %k Meta-W"
	"  | Close all                     %k Meta-Shift-W";

enum
{
	kCopySelNamesCmd = 1,
	kUpdateCmd,
	kShowCTreeCmd, kShowDTreeCmd, kShowGoTreeCmd, kShowJavaTreeCmd, kShowPHPTreeCmd,
	kCloseWindowCmd, kCloseAllCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSymbolSRDirector::CBSymbolSRDirector
	(
	CBSymbolDirector*	mainSym,
	CBProjectDocument*	projDoc,
	CBSymbolList*		symbolList,
	const JString&		filterStr,
	const bool		isRegex,
	JError*				err
	)
	:
	JXWindowDirector(projDoc)
{
	CBSymbolSRDirectorX(mainSym, projDoc, symbolList, true);

	*err = itsSymbolTable->SetNameFilter(filterStr, isRegex);
	FitWindowToContent();

	JString title = JGetString("WindowTitlePrefix::CBSymbolSRDirector");
	title += filterStr;
	GetWindow()->SetTitle(title);
}

CBSymbolSRDirector::CBSymbolSRDirector
	(
	CBSymbolDirector*		mainSym,
	CBProjectDocument*		projDoc,
	CBSymbolList*			symbolList,
	const JArray<JIndex>&	displayList,
	const JString&			searchStr
	)
	:
	JXWindowDirector(projDoc)
{
	CBSymbolSRDirectorX(mainSym, projDoc, symbolList, true);

	itsSymbolTable->SetDisplayList(displayList);
	FitWindowToContent();

	JString title = JGetString("WindowTitlePrefix::CBSymbolSRDirector");;
	title += searchStr;
	GetWindow()->SetTitle(title);
}

// private

void
CBSymbolSRDirector::CBSymbolSRDirectorX
	(
	CBSymbolDirector*	owner,
	CBProjectDocument*	projDoc,
	CBSymbolList*		symbolList,
	const bool		focus
	)
{
	itsMainSymDir = owner;
	itsProjDoc    = projDoc;
	itsSymbolList = symbolList;

	BuildWindow(itsSymbolList, focus);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSymbolSRDirector::~CBSymbolSRDirector()
{
	itsMainSymDir->SRDirectorClosed(this);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "jcc_show_c_tree.xpm"
#include "jcc_show_d_tree.xpm"
#include "jcc_show_go_tree.xpm"
#include "jcc_show_java_tree.xpm"
#include "jcc_show_php_tree.xpm"

void
CBSymbolSRDirector::BuildWindow
	(
	CBSymbolList*	symbolList,
	const bool	focus
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,230, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,200);
	assert( scrollbarSet != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

// end JXLayout

	window->LockCurrentMinSize();

	if (focus)
		{
		window->ShouldFocusWhenShow(true);
		}

	itsSymbolTable =
		jnew CBSymbolTable(itsMainSymDir, symbolList,
						  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsSymbolTable != nullptr );
	itsSymbolTable->FitToEnclosure();

	itsActionsMenu = menuBar->AppendTextMenu(JGetString("ActionsMenuTitle::CBSymbolSRDirector"));
	itsActionsMenu->SetMenuItems(kActionsMenuStr, "CBSymbolSRDirector");
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	itsActionsMenu->SetItemImage(kShowCTreeCmd,    jcc_show_c_tree);
	itsActionsMenu->SetItemImage(kShowDTreeCmd,    jcc_show_d_tree);
	itsActionsMenu->SetItemImage(kShowGoTreeCmd,   jcc_show_go_tree);
	itsActionsMenu->SetItemImage(kShowJavaTreeCmd, jcc_show_java_tree);
	itsActionsMenu->SetItemImage(kShowPHPTreeCmd,  jcc_show_php_tree);

	itsCmdMenu =
		jnew CBCommandMenu(itsProjDoc, nullptr, menuBar,
						  JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsCmdMenu != nullptr );
	menuBar->AppendMenu(itsCmdMenu);
	ListenTo(itsCmdMenu);
}

/******************************************************************************
 FitWindowToContent (private)

 ******************************************************************************/

void
CBSymbolSRDirector::FitWindowToContent()
{
	GetWindow()->AdjustSize(
		itsSymbolTable->GetBoundsWidth() - itsSymbolTable->GetApertureWidth(),
		JMin(itsSymbolTable->GetBoundsHeight() - itsSymbolTable->GetApertureHeight(),
			 (JCoordinate) 300));
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBSymbolSRDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateActionsMenu();
		}
	else if (sender == itsActionsMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleActionsMenu(selection->GetIndex());
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
CBSymbolSRDirector::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	if (sender == itsCmdMenu && message->Is(CBCommandMenu::kGetTargetInfo))
		{
		auto* info =
			dynamic_cast<CBCommandMenu::GetTargetInfo*>(message);
		assert( info != nullptr );
		itsSymbolTable->GetFileNamesForSelection(info->GetFileList(),
												 info->GetLineIndexList());
		}
	else
		{
		JXWindowDirector::ReceiveWithFeedback(sender, message);
		}
}

/******************************************************************************
 UpdateActionsMenu (private)

 ******************************************************************************/

void
CBSymbolSRDirector::UpdateActionsMenu()
{
	itsActionsMenu->SetItemEnable(kCopySelNamesCmd, itsSymbolTable->HasSelection());

	itsActionsMenu->SetItemEnable(kShowCTreeCmd,
		!itsProjDoc->GetCTreeDirector()->GetTree()->IsEmpty());
	itsActionsMenu->SetItemEnable(kShowDTreeCmd,
		!itsProjDoc->GetDTreeDirector()->GetTree()->IsEmpty());
	itsActionsMenu->SetItemEnable(kShowGoTreeCmd,
		!itsProjDoc->GetGoTreeDirector()->GetTree()->IsEmpty());
	itsActionsMenu->SetItemEnable(kShowJavaTreeCmd,
		!itsProjDoc->GetJavaTreeDirector()->GetTree()->IsEmpty());
	itsActionsMenu->SetItemEnable(kShowPHPTreeCmd,
		!itsProjDoc->GetPHPTreeDirector()->GetTree()->IsEmpty());
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
CBSymbolSRDirector::HandleActionsMenu
	(
	const JIndex index
	)
{
	if (index == kCopySelNamesCmd)
		{
		itsSymbolTable->CopySelectedSymbolNames();
		}

	else if (index == kUpdateCmd)
		{
		itsProjDoc->UpdateSymbolDatabase();	// can delete us
		}
	else if (index == kShowCTreeCmd)
		{
		itsProjDoc->GetCTreeDirector()->Activate();
		}
	else if (index == kShowDTreeCmd)
		{
		itsProjDoc->GetDTreeDirector()->Activate();
		}
	else if (index == kShowGoTreeCmd)
		{
		itsProjDoc->GetGoTreeDirector()->Activate();
		}
	else if (index == kShowJavaTreeCmd)
		{
		itsProjDoc->GetJavaTreeDirector()->Activate();
		}
	else if (index == kShowPHPTreeCmd)
		{
		itsProjDoc->GetPHPTreeDirector()->Activate();
		}

	else if (index == kCloseWindowCmd)
		{
		Close();
		}
	else if (index == kCloseAllCmd)
		{
		itsMainSymDir->CloseSymbolBrowsers();
		}
}
