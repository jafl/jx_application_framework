/******************************************************************************
 CBTreeDirector.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright © 1996-99 by John Lindal.

 ******************************************************************************/

#include "CBTreeDirector.h"
#include "CBTreeWidget.h"
#include "CBTree.h"
#include "CBClass.h"
#include "CBFileHistoryMenu.h"
#include "CBEditTreePrefsDialog.h"
#include "CBViewManPageDialog.h"
#include "CBFindFileDialog.h"
#include "CBDiffFileDialog.h"
#include "CBSearchTextDialog.h"
#include "CBEditCPPMacroDialog.h"
#include "CBProjectDocument.h"
#include "CBSymbolDirector.h"
#include "CBFileListDirector.h"
#include "CBCommandMenu.h"
#include "CBDocumentMenu.h"
#include "cbActionDefs.h"
#include "cbGlobals.h"
#include <JXApplication.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXScrollbarSet.h>
#include <JXMenuBar.h>
#include <JXToolBar.h>
#include <JXPSPrinter.h>
#include <JXEPSPrinter.h>
#include <JXGetStringDialog.h>
#include <JXImage.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentPrefsVersion = 2;

	// version 1 saves autoMinMILinks, drawMILinksOnTop
	// version 2 saves CBTreeWidget::itsRaiseWhenSingleMatchFlag

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    New text file                  %k Meta-N       %i" kCBNewTextFileAction
	"  | New text file from template... %k Meta-Shift-N %i" kCBNewTextFileFromTmplAction
	"  | New project...                                 %i" kCBNewProjectAction
	"  | New shell...                                   %i" kCBNewShellAction
	"%l| Open...                        %k Meta-O       %i" kCBOpenSomethingAction
	"  | Recent projects"
	"  | Recent text files"
	"%l| PostScript page setup...                       %i" kJXPageSetupAction
	"  | Print PostScript...            %k Meta-P       %i" kJXPrintAction
	"  | Print EPS...                                   %i" kCBPrintTreeEPSAction
	"%l| Close                          %k Meta-W       %i" kJXCloseWindowAction
	"  | Quit                           %k Meta-Q       %i" kJXQuitAction;

enum
{
	kNewTextEditorCmd = 1, kNewTextTemplateCmd, kNewProjectCmd, kNewShellCmd,
	kOpenSomethingCmd,
	kRecentProjectMenuCmd, kRecentTextMenuCmd,
	kPSPageSetupCmd, kPrintPSCmd, kPrintEPSCmd,
	kCloseCmd, kQuitCmd
};

// Project menu

static const JUtf8Byte* kProjectMenuStr =
	"    Show symbol browser %k Ctrl-F12     %i" kCBShowSymbolBrowserAction
	"  | Look up man page... %k Meta-I       %i" kCBViewManPageAction
	"%l| Show file list      %k Meta-Shift-F %i" kCBShowFileListAction
	"  | Find file...        %k Meta-D       %i" kCBFindFileAction
	"  | Search files...     %k Meta-F       %i" kCBSearchFilesAction
	"  | Compare files...                    %i" kCBDiffFilesAction
	"%l| Save all            %k Meta-Shift-S %i" kCBSaveAllTextFilesAction
	"  | Close all           %k Meta-Shift-W %i" kCBCloseAllTextFilesAction;

enum
{
	kShowSymbolBrowserCmd = 1, kViewManPageCmd,
	kShowFileListCmd, kFindFileCmd, kSearchFilesCmd, kDiffFilesCmd,
	kSaveAllTextCmd, kCloseAllTextCmd
};

// Preferences menu

static const JUtf8Byte* kPrefsMenuStr =
	"    Tree..."
	"  | Toolbar buttons..."
	"  | File types..."
	"  | External editors..."
	"  | Miscellaneous..."
	"%l| Save window size as default";

enum
{
	kTreePrefsCmd = 1, kToolBarPrefsCmd,
	kEditFileTypesCmd, kChooseExtEditorsCmd, kMiscPrefsCmd,
	kSaveWindSizeCmd
};

/******************************************************************************
 Constructor

	In the 2nd version, d1 and d2 can be nullptr.

 ******************************************************************************/

CBTreeDirector::CBTreeDirector
	(
	CBProjectDocument*		supervisor,
	CBTreeCreateFn*			createTreeFn,
	const JUtf8Byte*		windowTitleSuffixID,
	const JUtf8Byte*		windowHelpName,
	const JXPM&				windowIcon,
	const JUtf8Byte*		treeMenuItems,
	const JUtf8Byte*		treeMenuNamespace,
	const JIndex			toolBarPrefID,
	CBTreeInitToolBarFn*	initToolBarFn
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(CBGetPrefsManager(), kCBTreeSetupID),
	itsWindowTitleSuffix( JGetString(windowTitleSuffixID) ),
	itsWindowHelpName( windowHelpName )
{
	JXScrollbarSet* scrollbarSet =
		CBTreeDirectorX(supervisor, windowIcon, treeMenuItems,
						treeMenuNamespace, toolBarPrefID, initToolBarFn);

	itsTree = createTreeFn(this, CBTreeWidget::kBorderWidth);

	itsTreeWidget =
		jnew CBTreeWidget(this, itsTree, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 0,0, 100,100);
	assert( itsTreeWidget != nullptr );
	itsTreeWidget->FitToEnclosure();

	JPrefObject::ReadPrefs();
}

static void skipFnListDirector
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	JSize fnListCount;
	input >> fnListCount;

	JString className;
	bool showInheritedFns;
	for (JIndex i=1; i<=fnListCount; i++)
		{
		JXWindow::SkipGeometry(input);
		input >> className >> JBoolFromString(showInheritedFns);
		if (vers >= 24)
			{
			JXScrollableWidget::SkipScrollSetup(input);
			}
		}
}

CBTreeDirector::CBTreeDirector
	(
	std::istream&			projInput,
	const JFileVersion		projVers,
	std::istream*			setInput,
	const JFileVersion		setVers,
	std::istream*			symStream,
	const JFileVersion		origSymVers,
	CBProjectDocument*		supervisor,
	const bool			subProject,
	CBTreeStreamInFn*		streamInTreeFn,
	const JUtf8Byte*		windowTitleSuffixID,
	const JUtf8Byte*		windowHelpName,
	const JXPM&				windowIcon,
	const JUtf8Byte*		treeMenuItems,
	const JUtf8Byte*		treeMenuNamespace,
	const JIndex			toolBarPrefID,
	CBTreeInitToolBarFn*	initToolBarFn,
	CBDirList*				dirList,
	const bool			readCompileRunDialogs
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(CBGetPrefsManager(), kCBTreeSetupID),
	itsWindowTitleSuffix( JGetString(windowTitleSuffixID) ),
	itsWindowHelpName( windowHelpName )
{
	JXScrollbarSet* scrollbarSet =
		CBTreeDirectorX(supervisor, windowIcon, treeMenuItems,
						treeMenuNamespace, toolBarPrefID, initToolBarFn);

	itsTree = streamInTreeFn(projInput, projVers,
							 setInput, setVers, symStream, origSymVers,
							 this, CBTreeWidget::kBorderWidth, dirList);

	std::istream* symInput             = (projVers <= 41 ? &projInput : symStream);
	const JFileVersion symVers    = (projVers <= 41 ? projVers   : origSymVers);
	const bool useSetProjData = setInput == nullptr || setVers < 71;

/* settings file */

	if (!useSetProjData)
		{
		GetWindow()->ReadGeometry(*setInput);
		}
	if (24 <= projVers && projVers < 71 && useSetProjData)
		{
		GetWindow()->ReadGeometry(projInput);
		}
	else if (24 <= projVers && projVers < 71)
		{
		JXWindow::SkipGeometry(projInput);
		}

	itsTreeWidget =
		jnew CBTreeWidget(this, itsTree, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 0,0, 100,100);
	assert( itsTreeWidget != nullptr );
	itsTreeWidget->FitToEnclosure();

	if (projVers < 71)
		{
		itsTreeWidget->ReadSetup(projInput, projVers);
		}
	if (!useSetProjData)	// overwrite
		{
		itsTreeWidget->ReadSetup(*setInput, setVers);
		}

	JPrefObject::ReadPrefs();		// set tree parameters before updating

	if (readCompileRunDialogs)
		{
		supervisor->ConvertCompileRunDialogs(projInput, projVers);
		}

	if (17 <= projVers && projVers < 71)
		{
		itsPSPrinter->ReadXPSSetup(projInput);
		itsEPSPrinter->ReadXEPSSetup(projInput);
		itsFnListPrinter->ReadXPSSetup(projInput);
		}
	if (!useSetProjData)	// overwrite
		{
		itsPSPrinter->ReadXPSSetup(*setInput);
		itsEPSPrinter->ReadXEPSSetup(*setInput);
		itsFnListPrinter->ReadXPSSetup(*setInput);
		}

	// put fn list windows on top of tree window

	bool active = false;
	if (31 <= projVers && projVers < 71)
		{
		projInput >> JBoolFromString(active);
		}
	if (!useSetProjData)	// overwrite
		{
		*setInput >> JBoolFromString(active);
		}
	if (active && !subProject)
		{
		Activate();
		}

/* symbol file */

	if (17 <= projVers && projVers < 71)
		{
		skipFnListDirector(projInput, projVers);
		}
	else if (symInput != nullptr && 71 <= symVers && symVers < 88)
		{
		skipFnListDirector(*symInput, symVers);
		}
}

// private

JXScrollbarSet*
CBTreeDirector::CBTreeDirectorX
	(
	CBProjectDocument*		doc,
	const JXPM&				windowIcon,
	const JUtf8Byte*		treeMenuItems,
	const JUtf8Byte*		treeMenuNamespace,
	const JIndex			toolBarPrefID,
	CBTreeInitToolBarFn*	initToolBarFn
	)
{
	itsProjDoc = doc;
	ListenTo(itsProjDoc);

	itsShowInheritedFnsFlag = true;
	itsFindFnDialog         = nullptr;

	JXScrollbarSet* sbarSet = BuildWindow(windowIcon, treeMenuItems,
										  treeMenuNamespace,
										  toolBarPrefID, initToolBarFn);

	// can't call GetWindow() until window is created

	itsPSPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPSPrinter != nullptr );
	ListenTo(itsPSPrinter);

	itsEPSPrinter = jnew JXEPSPrinter(GetDisplay());
	assert( itsEPSPrinter != nullptr );
	ListenTo(itsEPSPrinter);

	itsFnListPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsFnListPrinter != nullptr );

	return sbarSet;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBTreeDirector::~CBTreeDirector()
{
	jdelete itsTree;
	jdelete itsPSPrinter;
	jdelete itsEPSPrinter;
	jdelete itsFnListPrinter;
}

/******************************************************************************
 ReloadSetup (virtual)

 ******************************************************************************/

void
CBTreeDirector::ReloadSetup
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	itsTree->ReloadSetup(input, vers);
}

/******************************************************************************
 StreamOut (virtual)

	dirList, d1, and d2 can be nullptr

 ******************************************************************************/

void
CBTreeDirector::StreamOut
	(
	std::ostream&		projOutput,
	std::ostream*		setOutput,
	std::ostream*		symOutput,
	const CBDirList*	dirList
	)
	const
{
	itsTree->StreamOut(projOutput, setOutput, symOutput, dirList);

/* settings file */

	if (setOutput != nullptr)
		{
		*setOutput << ' ';
		GetWindow()->WriteGeometry(*setOutput);

		*setOutput << ' ';
		itsTreeWidget->WriteSetup(*setOutput);

		*setOutput << ' ';
		itsPSPrinter->WriteXPSSetup(*setOutput);

		*setOutput << ' ';
		itsEPSPrinter->WriteXEPSSetup(*setOutput);

		*setOutput << ' ';
		itsFnListPrinter->WriteXPSSetup(*setOutput);

		*setOutput << ' ' << JBoolToString(IsActive());

		*setOutput << ' ';
		}
}

/******************************************************************************
 FileTypesChanged

 ******************************************************************************/

void
CBTreeDirector::FileTypesChanged
	(
	const CBPrefsManager::FileTypesChanged& info
	)
{
	itsTree->FileTypesChanged(info);
}

/******************************************************************************
 PrepareForTreeUpdate

 ******************************************************************************/

void
CBTreeDirector::PrepareForTreeUpdate
	(
	const bool reparseAll
	)
{
	itsTree->PrepareForUpdate(reparseAll);
}

/******************************************************************************
 TreeUpdateFinished

 ******************************************************************************/

bool
CBTreeDirector::TreeUpdateFinished
	(
	const JArray<JFAID_t>& deadFileList
	)
{
	return itsTree->UpdateFinished(deadFileList);
}

/******************************************************************************
 AskForFunctionToFind

 ******************************************************************************/

void
CBTreeDirector::AskForFunctionToFind()
{
	assert( itsFindFnDialog == nullptr );

	itsFindFnDialog =
		jnew JXGetStringDialog(this, JGetString("FindFunctionTitle::CBTreeDirector"),
							  JGetString("FindFunctionPrompt::CBTreeDirector"));
	assert( itsFindFnDialog != nullptr );

	itsFindFnDialog->BeginDialog();
	ListenTo(itsFindFnDialog);
}

/******************************************************************************
 ViewFunctionList

 ******************************************************************************/

void
CBTreeDirector::ViewFunctionList
	(
	const CBClass* theClass
	)
{
/*
	const JSize count = itsFnBrowsers->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CBFnListDirector* dir = itsFnBrowsers->GetElement(i);
		if (dir->IsShowingClass(theClass))
			{
			dir->Activate();
			return;
			}
		}

	CBFnListDirector* dir = jnew CBFnListDirector(this, itsFnListPrinter,
												 theClass, itsTreeWidget,
												 itsShowInheritedFnsFlag);
	assert( dir != nullptr );
	dir->Activate();
*/
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include <jx_file_new.xpm>
#include <jx_file_open.xpm>
#include <jx_file_print.xpm>
#include "jcc_view_man_page.xpm"
#include "jcc_show_symbol_list.xpm"
#include "jcc_show_file_list.xpm"
#include "jcc_search_files.xpm"
#include "jcc_compare_files.xpm"
#include <jx_file_save_all.xpm>

JXScrollbarSet*
CBTreeDirector::BuildWindow
	(
	const JXPM&				windowIcon,
	const JUtf8Byte*		treeMenuItems,
	const JUtf8Byte*		treeMenuNamespace,
	const JIndex			toolBarPrefID,
	CBTreeInitToolBarFn*	initToolBarFn
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,430, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(CBGetPrefsManager(), toolBarPrefID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,400);
	assert( itsToolBar != nullptr );

// end JXLayout

	AdjustWindowTitle();
	window->SetMinSize(150, 150);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetWMClass(CBGetWMClassInstance(), CBGetTreeWindowClass());

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure();

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, windowIcon);
	assert( icon != nullptr );
	window->SetIcon(icon);

	JPoint desktopLoc;
	JCoordinate w,h;
	if (CBGetPrefsManager()->GetWindowSize(kCBTreeWindSizeID, &desktopLoc, &w, &h))
		{
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
		}

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "CBTreeDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kNewTextEditorCmd, jx_file_new);
	itsFileMenu->SetItemImage(kOpenSomethingCmd, jx_file_open);
	itsFileMenu->SetItemImage(kPrintPSCmd,       jx_file_print);

	auto* recentProjectMenu =
		jnew CBFileHistoryMenu(CBDocumentManager::kProjectFileHistory,
							  itsFileMenu, kRecentProjectMenuCmd, menuBar);
	assert( recentProjectMenu != nullptr );

	auto* recentTextMenu =
		jnew CBFileHistoryMenu(CBDocumentManager::kTextFileHistory,
							  itsFileMenu, kRecentTextMenuCmd, menuBar);
	assert( recentTextMenu != nullptr );

	itsTreeMenu = menuBar->AppendTextMenu(JGetString("TreeMenuTitle::CBTreeDirector"));
	itsTreeMenu->SetMenuItems(treeMenuItems, treeMenuNamespace);
	ListenTo(itsTreeMenu);

	itsProjectMenu = menuBar->AppendTextMenu(JGetString("ProjectMenuTitle::CBGlobal"));
	itsProjectMenu->SetMenuItems(kProjectMenuStr, "CBTreeDirector");
	itsProjectMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsProjectMenu);

	itsProjectMenu->SetItemImage(kViewManPageCmd,       jcc_view_man_page);
	itsProjectMenu->SetItemImage(kShowSymbolBrowserCmd, jcc_show_symbol_list);
	itsProjectMenu->SetItemImage(kShowFileListCmd,      jcc_show_file_list);
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
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "CBTreeDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = CBGetApplication()->CreateHelpMenu(menuBar, "CBTreeDirector");
	ListenTo(itsHelpMenu);

	// must do this after creating menus

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
		{
		itsToolBar->AppendButton(itsFileMenu, kNewTextEditorCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenSomethingCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kPrintPSCmd);
		itsToolBar->NewGroup();

		initToolBarFn(itsToolBar, itsTreeMenu);

		CBGetApplication()->AppendHelpMenuToToolBar(itsToolBar, itsHelpMenu);
		}

	return scrollbarSet;
}

/******************************************************************************
 AdjustWindowTitle (private)

 ******************************************************************************/

void
CBTreeDirector::AdjustWindowTitle()
{
	const JString title = itsProjDoc->GetName() + itsWindowTitleSuffix;
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBTreeDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
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

	else if (sender == itsTreeMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateTreeMenu();
		}
	else if (sender == itsTreeMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleTreeMenu(selection->GetIndex());
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
		CBGetApplication()->HandleHelpMenu(itsHelpMenu, itsWindowHelpName,
											 selection->GetIndex());
		}

	else if (sender == itsFindFnDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			itsTreeWidget->FindFunction(itsFindFnDialog->GetString(), true,
										kJXLeftButton);
			}
		itsFindFnDialog = nullptr;
		}

	else if (sender == itsPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const auto* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			itsTreeWidget->Print(*itsPSPrinter);
			}
		}

	else if (sender == itsEPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const auto* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			itsTreeWidget->Print(*itsEPSPrinter);
			}
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
 UpdateFileMenu (private)

 ******************************************************************************/

void
CBTreeDirector::UpdateFileMenu()
{
	const bool canPrint = !itsTree->IsEmpty();
	itsFileMenu->SetItemEnable(kPrintPSCmd,  canPrint);
	itsFileMenu->SetItemEnable(kPrintEPSCmd, canPrint);

	itsFileMenu->SetItemEnable(kCloseCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
CBTreeDirector::HandleFileMenu
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

	else if (index == kPSPageSetupCmd)
		{
		itsPSPrinter->BeginUserPageSetup();
		}
	else if (index == kPrintPSCmd)
		{
		itsPSPrinter->BeginUserPrintSetup();
		}
	else if (index == kPrintEPSCmd)
		{
		itsEPSPrinter->BeginUserPrintSetup();
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
 UpdateProjectMenu (private)

 ******************************************************************************/

void
CBTreeDirector::UpdateProjectMenu()
{
	itsProjectMenu->SetItemEnable(kCloseAllTextCmd,
								  CBGetDocumentManager()->HasTextDocuments());
	itsProjectMenu->SetItemEnable(kSaveAllTextCmd,
								  CBGetDocumentManager()->TextDocumentsNeedSave());
}

/******************************************************************************
 HandleProjectMenu (private)

 ******************************************************************************/

void
CBTreeDirector::HandleProjectMenu
	(
	const JIndex index
	)
{
	CBGetDocumentManager()->SetActiveProjectDocument(itsProjDoc);

	if (index == kShowSymbolBrowserCmd)
		{
		(itsProjDoc->GetSymbolDirector())->Activate();
		}
	else if (index == kViewManPageCmd)
		{
		(CBGetViewManPageDialog())->Activate();
		}

	else if (index == kShowFileListCmd)
		{
		(itsProjDoc->GetFileListDirector())->Activate();
		}
	else if (index == kFindFileCmd)
		{
		(CBGetFindFileDialog())->Activate();
		}
	else if (index == kSearchFilesCmd)
		{
		(CBGetSearchTextDialog())->Activate();
		}
	else if (index == kDiffFilesCmd)
		{
		(CBGetDiffFileDialog())->Activate();
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
CBTreeDirector::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
CBTreeDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kTreePrefsCmd)
		{
		EditTreePrefs();
		}
	else if (index == kToolBarPrefsCmd)
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
	else if (index == kMiscPrefsCmd)
		{
		CBGetApplication()->EditMiscPrefs();
		}

	else if (index == kSaveWindSizeCmd)
		{
		CBGetPrefsManager()->SaveWindowSize(kCBTreeWindSizeID, GetWindow());
		}
}

/******************************************************************************
 EditTreePrefs (private)

 ******************************************************************************/

void
CBTreeDirector::EditTreePrefs()
{
	auto* dlog =
		jnew CBEditTreePrefsDialog(itsTree->GetFontSize(), itsShowInheritedFnsFlag,
								  itsTree->WillAutoMinimizeMILinks(),
								  itsTree->WillDrawMILinksOnTop(),
								  itsTreeWidget->WillRaiseWindowWhenSingleMatch());
	assert( dlog != nullptr );
	dlog->BeginDialog();
}

/******************************************************************************
 SetTreePrefs

 ******************************************************************************/

void
CBTreeDirector::SetTreePrefs
	(
	const JSize		fontSize,
	const bool	showInheritedFns,
	const bool	autoMinMILinks,
	const bool	drawMILinksOnTop,
	const bool	raiseWhenSingleMatch
	)
{
	itsShowInheritedFnsFlag = showInheritedFns;

	itsTree->SetFontSize(fontSize);
	itsTree->ShouldAutoMinimizeMILinks(autoMinMILinks);
	itsTree->ShouldDrawMILinksOnTop(drawMILinksOnTop);

	itsTreeWidget->ShouldRaiseWindowWhenSingleMatch(raiseWhenSingleMatch);
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBTreeDirector::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentPrefsVersion)
		{
		return;
		}

	JSize fontSize;
	input >> fontSize >> JBoolFromString(itsShowInheritedFnsFlag);
	itsTree->SetFontSize(fontSize);

	if (vers >= 1)
		{
		bool autoMinMILinks, drawMILinksOnTop;
		input >> JBoolFromString(autoMinMILinks) >> JBoolFromString(drawMILinksOnTop);
		itsTree->ShouldAutoMinimizeMILinks(autoMinMILinks);
		itsTree->ShouldDrawMILinksOnTop(drawMILinksOnTop);
		}

	if (vers >= 2)
		{
		bool raiseWhenSingleMatch;
		input >> JBoolFromString(raiseWhenSingleMatch);
		itsTreeWidget->ShouldRaiseWindowWhenSingleMatch(raiseWhenSingleMatch);
		}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBTreeDirector::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;

	output << ' ' << itsTree->GetFontSize();
	output << ' ' << JBoolToString(itsShowInheritedFnsFlag);
	output << ' ' << JBoolToString(itsTree->WillAutoMinimizeMILinks());
	output << ' ' << JBoolToString(itsTree->WillDrawMILinksOnTop());
	output << ' ' << JBoolToString(itsTreeWidget->WillRaiseWindowWhenSingleMatch());
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
CBTreeDirector::ReceiveWithFeedback
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

		JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
		if (itsTree->GetSelectedClasses(&classList))
			{
			const JSize count = classList.GetElementCount();
			JString fullName;
			for (JIndex i=1; i<=count; i++)
				{
				if ((classList.GetElement(i))->GetFileName(&fullName))
					{
					// since cmd-; works fine with .h file, no real value
					// to using source file instead of header file

					JString complName;
					if (0 && CBGetDocumentManager()->GetComplementFile(
							fullName, CBGetPrefsManager()->GetFileType(fullName),
							&complName, GetProjectDoc(), true))
						{
						info->AddFile(complName);
						}
					else
						{
						info->AddFile(fullName);
						}
					}
				}
			}
		}
	else
		{
		JXWindowDirector::ReceiveWithFeedback(sender, message);
		}
}
