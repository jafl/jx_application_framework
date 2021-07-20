/******************************************************************************
 CBFileListDirector.cpp

	Window to display all files found by CBParseFiles().

	BASE CLASS = JXWindowDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBFileListDirector.h"
#include "CBFileListTable.h"
#include "CBProjectDocument.h"
#include "CBSymbolDirector.h"
#include "CBCTreeDirector.h"
#include "CBDTreeDirector.h"
#include "CBGoTreeDirector.h"
#include "CBJavaTreeDirector.h"
#include "CBPHPTreeDirector.h"
#include "CBTree.h"
#include "CBViewManPageDialog.h"
#include "CBFindFileDialog.h"
#include "CBDiffFileDialog.h"
#include "CBSearchTextDialog.h"
#include "CBCommandMenu.h"
#include "CBDocumentMenu.h"
#include "CBFileHistoryMenu.h"
#include "cbActionDefs.h"
#include "cbGlobals.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXFileListSet.h>
#include <JXToolBar.h>
#include <JXScrollbarSet.h>
#include <JXWebBrowser.h>
#include <JXImage.h>
#include <JString.h>
#include <jAssert.h>

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    New text file                  %k Meta-N       %i" kCBNewTextFileAction
	"  | New text file from template... %k Meta-Shift-N %i" kCBNewTextFileFromTmplAction
	"  | New project...                                 %i" kCBNewProjectAction
	"  | New shell...                                   %i" kCBNewShellAction
	"%l| Open...                        %k Meta-O       %i" kCBOpenSomethingAction
	"  | Recent projects"
	"  | Recent text files"
	"%l| Close                          %k Meta-W       %i" kJXCloseWindowAction
	"  | Quit                           %k Meta-Q       %i" kJXQuitAction;

enum
{
	kNewTextEditorCmd = 1, kNewTextTemplateCmd, kNewProjectCmd, kNewShellCmd,
	kOpenSomethingCmd,
	kRecentProjectMenuCmd, kRecentTextMenuCmd,
	kCloseCmd, kQuitCmd
};

// List menu

static const JUtf8Byte* kListMenuStr =
	"    Open selected files                 %k Return.     %i" kCBOpenSelectedFilesAction
	"  | Show selected files in file manager %k Meta-Return %i" kCBOpenSelectedFileLocationsAction
	"  | Update                              %k Meta-U      %i" kCBUpdateClassTreeAction
	"%l| Use wildcard filter %b                             %i" kCBUseWildcardFilterAction
	"  | Use regex filter    %b                             %i" kCBUseRegexFilterAction;

enum
{
	kOpenSelectionCmd = 1, kShowLocationCmd, kUpdateCmd,
	kUseWildcardCmd, kUseRegexCmd
};

// Project menu

static const JUtf8Byte* kProjectMenuStr =
	"    Show symbol browser %k Ctrl-F12     %i" kCBShowSymbolBrowserAction
	"  | Show C++ class tree                 %i" kCBShowCPPClassTreeAction
	"  | Show D class tree                   %i" kCBShowDClassTreeAction
	"  | Show Go struct/interface tree       %i" kCBShowGoClassTreeAction
	"  | Show Java class tree                %i" kCBShowJavaClassTreeAction
	"  | Show PHP class tree                 %i" kCBShowPHPClassTreeAction
	"  | Look up man page... %k Meta-I       %i" kCBViewManPageAction
	"%l| Find file...        %k Meta-D       %i" kCBFindFileAction
	"  | Search files...     %k Meta-F       %i" kCBSearchFilesAction
	"  | Compare files...                    %i" kCBDiffFilesAction
	"%l| Save all            %k Meta-Shift-S %i" kCBSaveAllTextFilesAction
	"  | Close all           %k Meta-Shift-W %i" kCBCloseAllTextFilesAction;

enum
{
	kShowSymbolBrowserCmd = 1,
	kShowCTreeCmd, kShowDTreeCmd, kShowGoTreeCmd, kShowJavaTreeCmd, kShowPHPTreeCmd,
	kViewManPageCmd,
	kFindFileCmd, kSearchFilesCmd, kDiffFilesCmd,
	kSaveAllTextCmd, kCloseAllTextCmd
};

// Preferences menu

static const JUtf8Byte* kPrefsMenuStr =
	"    Toolbar buttons..."
	"  | File types..."
	"  | External editors..."
	"  | File manager & web browser..."
	"  | Miscellaneous..."
	"%l| Save window size as default";

enum
{
	kToolBarPrefsCmd = 1,
	kEditFileTypesCmd, kChooseExtEditorsCmd,
	kShowLocationPrefsCmd, kMiscPrefsCmd,
	kSaveWindSizeCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFileListDirector::CBFileListDirector
	(
	CBProjectDocument* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	CBFileListDirectorX(supervisor);
}

CBFileListDirector::CBFileListDirector
	(
	std::istream&			projInput,
	const JFileVersion	projVers,
	std::istream*			setInput,
	const JFileVersion	setVers,
	std::istream*			symInput,
	const JFileVersion	symVers,
	CBProjectDocument*	supervisor,
	const bool		subProject
	)
	:
	JXWindowDirector(supervisor)
{
	CBFileListDirectorX(supervisor);

	if (projVers >= 20)
		{
		const bool useProjData = setInput == nullptr || setVers < 71;
		if (projVers < 71)
			{
			if (useProjData)
				{
				GetWindow()->ReadGeometry(projInput);
				}
			else
				{
				JXWindow::SkipGeometry(projInput);
				}

			bool active = false;
			if (projVers >= 50)
				{
				projInput >> JBoolFromString(active);
				}
			if (useProjData && active && !subProject)
				{
				Activate();
				}

			itsFLSet->ReadSetup(projInput);		// no way to skip
			}

		if (!useProjData)
			{
			GetWindow()->ReadGeometry(*setInput);

			bool active;
			*setInput >> JBoolFromString(active);
			if (active && !subProject)
				{
				Activate();
				}

			itsFLSet->ReadSetup(*setInput);
			}

		itsFLTable->ReadSetup(projInput, projVers, symInput, symVers);
		}
}

// private

void
CBFileListDirector::CBFileListDirectorX
	(
	CBProjectDocument* projDoc
	)
{
	itsProjDoc = projDoc;
	ListenTo(itsProjDoc);

	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBFileListDirector::~CBFileListDirector()
{
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
CBFileListDirector::StreamOut
	(
	std::ostream& projOutput,
	std::ostream* setOutput,
	std::ostream* symOutput
	)
	const
{
	if (setOutput != nullptr)
		{
		*setOutput << ' ';
		GetWindow()->WriteGeometry(*setOutput);
		*setOutput << ' ' << JBoolToString(IsActive());
		*setOutput << ' ';
		itsFLSet->WriteSetup(*setOutput);
		*setOutput << ' ';
		}

	itsFLTable->WriteSetup(projOutput, symOutput);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "jcc_file_list_window.xpm"

#include <jx_file_new.xpm>
#include <jx_file_open.xpm>
#include <jx_filter_wildcard.xpm>
#include <jx_filter_regex.xpm>
#include "jcc_show_symbol_list.xpm"
#include "jcc_show_c_tree.xpm"
#include "jcc_show_d_tree.xpm"
#include "jcc_show_go_tree.xpm"
#include "jcc_show_java_tree.xpm"
#include "jcc_show_php_tree.xpm"
#include "jcc_view_man_page.xpm"
#include "jcc_search_files.xpm"
#include "jcc_compare_files.xpm"
#include <jx_file_save_all.xpm>

void
CBFileListDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 340,450, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 340,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(CBGetPrefsManager(), kCBFileListToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 340,420);
	assert( itsToolBar != nullptr );

// end JXLayout

	AdjustWindowTitle();
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(true);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetWMClass(CBGetWMClassInstance(), CBGetFileListWindowClass());

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, jcc_file_list_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	JPoint desktopLoc;
	JCoordinate w,h;
	if (CBGetPrefsManager()->GetWindowSize(kCBFileListWindSizeID, &desktopLoc, &w, &h))
		{
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
		}

	itsFLSet =
		jnew JXFileListSet(itsToolBar->GetWidgetEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( itsFLSet != nullptr );
	itsFLSet->FitToEnclosure();

	JXScrollbarSet* scrollbarSet = itsFLSet->GetScrollbarSet();
	itsFLTable =
		jnew CBFileListTable(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsFLTable != nullptr );
	itsFLSet->SetTable(itsFLTable);
	ListenTo(itsFLTable);

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "CBFileListDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kNewTextEditorCmd, jx_file_new);
	itsFileMenu->SetItemImage(kOpenSomethingCmd, jx_file_open);

	auto* recentProjectMenu =
		jnew CBFileHistoryMenu(CBDocumentManager::kProjectFileHistory,
							  itsFileMenu, kRecentProjectMenuCmd, menuBar);
	assert( recentProjectMenu != nullptr );

	auto* recentTextMenu =
		jnew CBFileHistoryMenu(CBDocumentManager::kTextFileHistory,
							  itsFileMenu, kRecentTextMenuCmd, menuBar);
	assert( recentTextMenu != nullptr );

	itsListMenu = menuBar->AppendTextMenu(JGetString("ListMenuTitle::CBFileListDirector"));
	itsListMenu->SetMenuItems(kListMenuStr, "CBFileListDirector");
	itsListMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsListMenu);

	itsListMenu->SetItemImage(kUseWildcardCmd, jx_filter_wildcard);
	itsListMenu->SetItemImage(kUseRegexCmd,    jx_filter_regex);

	itsFLSet->AppendEditMenu(menuBar);

	itsProjectMenu = menuBar->AppendTextMenu(JGetString("ProjectMenuTitle::CBGlobal"));
	itsProjectMenu->SetMenuItems(kProjectMenuStr, "CBFileListDirector");
	itsProjectMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsProjectMenu);

	itsProjectMenu->SetItemImage(kShowSymbolBrowserCmd, jcc_show_symbol_list);
	itsProjectMenu->SetItemImage(kShowCTreeCmd,         jcc_show_c_tree);
	itsProjectMenu->SetItemImage(kShowDTreeCmd,         jcc_show_d_tree);
	itsProjectMenu->SetItemImage(kShowGoTreeCmd,        jcc_show_go_tree);
	itsProjectMenu->SetItemImage(kShowJavaTreeCmd,      jcc_show_java_tree);
	itsProjectMenu->SetItemImage(kShowPHPTreeCmd,       jcc_show_php_tree);
	itsProjectMenu->SetItemImage(kViewManPageCmd,       jcc_view_man_page);
	itsProjectMenu->SetItemImage(kSearchFilesCmd,       jcc_search_files);
	itsProjectMenu->SetItemImage(kDiffFilesCmd,         jcc_compare_files);
	itsProjectMenu->SetItemImage(kSaveAllTextCmd,       jx_file_save_all);

	itsCmdMenu =
		jnew CBCommandMenu(itsProjDoc, nullptr, menuBar,
						  JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsCmdMenu != nullptr );
	menuBar->AppendMenu(itsCmdMenu);
	ListenTo(itsCmdMenu);

	auto* fileListMenu =
		jnew CBDocumentMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
						   JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( fileListMenu != nullptr );
	menuBar->AppendMenu(fileListMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "CBFileListDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = CBGetApplication()->CreateHelpMenu(menuBar, "CBFileListDirector");
	ListenTo(itsHelpMenu);

	// must do this after creating menus

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
		{
		itsToolBar->AppendButton(itsFileMenu, kNewTextEditorCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenSomethingCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsListMenu, kUseWildcardCmd);
		itsToolBar->AppendButton(itsListMenu, kUseRegexCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsProjectMenu, kSearchFilesCmd);

		CBGetApplication()->AppendHelpMenuToToolBar(itsToolBar, itsHelpMenu);
		}
}

/******************************************************************************
 AdjustWindowTitle (private)

 ******************************************************************************/

void
CBFileListDirector::AdjustWindowTitle()
{
	const JString title = itsProjDoc->GetName() + JGetString("WindowTitleSuffix::CBFileListDirector");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBFileListDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFLTable && message.Is(JXFileListTable::kProcessSelection))
		{
		OpenSelectedFiles();
		}

	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsListMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateListMenu();
		}
	else if (sender == itsListMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleListMenu(selection->GetIndex());
		}

	else if (sender == itsProjectMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateProjectMenu();
		}
	else if (sender == itsProjectMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleProjectMenu(selection->GetIndex());
		}

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrefsMenu();
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandlePrefsMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		CBGetApplication()->UpdateHelpMenu(itsHelpMenu);
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		CBGetApplication()->HandleHelpMenu(itsHelpMenu, "CBFileListHelp",
											 selection->GetIndex());
		}

	else if (sender == itsProjDoc && message.Is(JXFileDocument::kNameChanged))
		{
		AdjustWindowTitle();
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 OpenSelectedFiles (private)

 ******************************************************************************/

void
CBFileListDirector::OpenSelectedFiles()
	const
{
	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	if (itsFLTable->GetSelection(&fileList))
		{
		CBGetDocumentManager()->OpenSomething(fileList);
		}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
CBFileListDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kCloseCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
CBFileListDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	CBGetDocumentManager()->SetActiveProjectDocument(itsProjDoc);

	if (index == kNewTextEditorCmd)
		{
		CBGetDocumentManager()->NewTextDocument();
		}
	else if (index == kNewTextTemplateCmd)
		{
		CBGetDocumentManager()->NewTextDocumentFromTemplate();
		}
	else if (index == kNewProjectCmd)
		{
		CBGetDocumentManager()->NewProjectDocument();
		}
	else if (index == kNewShellCmd)
		{
		CBGetDocumentManager()->NewShellDocument();
		}
	else if (index == kOpenSomethingCmd)
		{
		CBGetDocumentManager()->OpenSomething();
		}

	else if (index == kCloseCmd)
		{
		GetWindow()->Close();
		}
	else if (index == kQuitCmd)
		{
		JXGetApplication()->Quit();
		}
}

/******************************************************************************
 UpdateListMenu (private)

 ******************************************************************************/

void
CBFileListDirector::UpdateListMenu()
{
	const bool allowOpen =
		itsFLTable->HasFocus() && itsFLTable->HasSelection();
	itsListMenu->SetItemEnable(kOpenSelectionCmd, allowOpen);
	itsListMenu->SetItemEnable(kShowLocationCmd,  allowOpen);

	const JXFileListSet::FilterType type = itsFLSet->GetFilterType();
	if (type == JXFileListSet::kWildcardFilter)
		{
		itsListMenu->CheckItem(kUseWildcardCmd);
		}
	else if (type == JXFileListSet::kRegexFilter)
		{
		itsListMenu->CheckItem(kUseRegexCmd);
		}
}

/******************************************************************************
 HandleListMenu (private)

 ******************************************************************************/

void
CBFileListDirector::HandleListMenu
	(
	const JIndex index
	)
{
	if (index == kOpenSelectionCmd)
		{
		OpenSelectedFiles();
		}
	else if (index == kShowLocationCmd)
		{
		itsFLTable->ShowSelectedFileLocations();
		}
	else if (index == kUpdateCmd)
		{
		itsProjDoc->UpdateSymbolDatabase();
		}

	else if (index == kUseWildcardCmd)
		{
		itsFLSet->ToggleWildcardFilter();
		}
	else if (index == kUseRegexCmd)
		{
		itsFLSet->ToggleRegexFilter();
		}
}

/******************************************************************************
 UpdateProjectMenu (private)

 ******************************************************************************/

void
CBFileListDirector::UpdateProjectMenu()
{
	itsProjectMenu->SetItemEnable(kShowCTreeCmd,
		!itsProjDoc->GetCTreeDirector()->GetTree()->IsEmpty());
	itsProjectMenu->SetItemEnable(kShowDTreeCmd,
		!itsProjDoc->GetDTreeDirector()->GetTree()->IsEmpty());
	itsProjectMenu->SetItemEnable(kShowGoTreeCmd,
		!itsProjDoc->GetGoTreeDirector()->GetTree()->IsEmpty());
	itsProjectMenu->SetItemEnable(kShowJavaTreeCmd,
		!itsProjDoc->GetJavaTreeDirector()->GetTree()->IsEmpty());
	itsProjectMenu->SetItemEnable(kShowPHPTreeCmd,
		!itsProjDoc->GetPHPTreeDirector()->GetTree()->IsEmpty());

	itsProjectMenu->SetItemEnable(kCloseAllTextCmd,
								  CBGetDocumentManager()->HasTextDocuments());
	itsProjectMenu->SetItemEnable(kSaveAllTextCmd,
								  CBGetDocumentManager()->TextDocumentsNeedSave());
}

/******************************************************************************
 HandleProjectMenu (private)

 ******************************************************************************/

void
CBFileListDirector::HandleProjectMenu
	(
	const JIndex index
	)
{
	CBGetDocumentManager()->SetActiveProjectDocument(itsProjDoc);

	if (index == kShowSymbolBrowserCmd)
		{
		itsProjDoc->GetSymbolDirector()->Activate();
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
	else if (index == kViewManPageCmd)
		{
		CBGetViewManPageDialog()->Activate();
		}

	else if (index == kFindFileCmd)
		{
		CBGetFindFileDialog()->Activate();
		}
	else if (index == kSearchFilesCmd)
		{
		CBGetSearchTextDialog()->Activate();
		}
	else if (index == kDiffFilesCmd)
		{
		CBGetDiffFileDialog()->Activate();
		}

	else if (index == kSaveAllTextCmd)
		{
		CBGetDocumentManager()->SaveTextDocuments(true);
		}
	else if (index == kCloseAllTextCmd)
		{
		CBGetDocumentManager()->CloseTextDocuments();
		}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
CBFileListDirector::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
CBFileListDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kToolBarPrefsCmd)
		{
		itsToolBar->Edit();
		}
	else if (index == kEditFileTypesCmd)
		{
		CBGetPrefsManager()->EditFileTypes();
		}
	else if (index == kChooseExtEditorsCmd)
		{
		CBGetDocumentManager()->ChooseEditors();
		}
	else if (index == kShowLocationPrefsCmd)
		{
		(JXGetWebBrowser())->EditPrefs();
		}
	else if (index == kMiscPrefsCmd)
		{
		CBGetApplication()->EditMiscPrefs();
		}

	else if (index == kSaveWindSizeCmd)
		{
		CBGetPrefsManager()->SaveWindowSize(kCBFileListWindSizeID, GetWindow());
		}
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
CBFileListDirector::ReceiveWithFeedback
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
		itsFLTable->GetSelection(info->GetFileList());
		}
	else
		{
		JXWindowDirector::ReceiveWithFeedback(sender, message);
		}
}
