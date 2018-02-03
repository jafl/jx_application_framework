/******************************************************************************
 CBTreeDirector.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include "CBTreeDirector.h"
#include "CBTreeWidget.h"
#include "CBTree.h"
#include "CBClass.h"
#include "CBFileHistoryMenu.h"
#include "CBFnListDirector.h"
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

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
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

static const JCharacter* kProjectMenuTitleStr = "Project";
static const JCharacter* kProjectMenuStr =
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

// Windows menu

static const JCharacter* kFileListMenuTitleStr = "Windows";

// Preferences menu

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
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

	In the 2nd version, d1 and d2 can be NULL.

 ******************************************************************************/

CBTreeDirector::CBTreeDirector
	(
	CBProjectDocument*		supervisor,
	CBTreeCreateFn*			createTreeFn,
	const JCharacter*		windowTitleSuffix,
	const JCharacter*		windowHelpName,
	const JXPM&				windowIcon,
	const JCharacter*		treeMenuTitle,
	const JCharacter*		treeMenuItems,
	const JCharacter*		treeMenuNamespace,
	const JIndex			toolBarPrefID,
	CBTreeInitToolBarFn*	initToolBarFn
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(CBGetPrefsManager(), kCBTreeSetupID),
	itsWindowTitleSuffix( windowTitleSuffix ),
	itsWindowHelpName( windowHelpName )
{
	JXScrollbarSet* scrollbarSet =
		CBTreeDirectorX(supervisor, windowIcon, treeMenuTitle, treeMenuItems,
						treeMenuNamespace, toolBarPrefID, initToolBarFn);

	itsTree = createTreeFn(this, CBTreeWidget::kBorderWidth);

	itsTreeWidget =
		jnew CBTreeWidget(this, itsTree, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 0,0, 100,100);
	assert( itsTreeWidget != NULL );
	itsTreeWidget->FitToEnclosure();

	JPrefObject::ReadPrefs();
}

CBTreeDirector::CBTreeDirector
	(
	std::istream&				projInput,
	const JFileVersion		projVers,
	std::istream*				setInput,
	const JFileVersion		setVers,
	std::istream*				symStream,
	const JFileVersion		origSymVers,
	CBProjectDocument*		supervisor,
	const JBoolean			subProject,
	CBTreeStreamInFn*		streamInTreeFn,
	const JCharacter*		windowTitleSuffix,
	const JCharacter*		windowHelpName,
	const JXPM&				windowIcon,
	const JCharacter*		treeMenuTitle,
	const JCharacter*		treeMenuItems,
	const JCharacter*		treeMenuNamespace,
	const JIndex			toolBarPrefID,
	CBTreeInitToolBarFn*	initToolBarFn,
	CBDirList*				dirList,
	const JBoolean			readCompileRunDialogs
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(CBGetPrefsManager(), kCBTreeSetupID),
	itsWindowTitleSuffix( windowTitleSuffix ),
	itsWindowHelpName( windowHelpName )
{
	JXScrollbarSet* scrollbarSet =
		CBTreeDirectorX(supervisor, windowIcon, treeMenuTitle, treeMenuItems,
						treeMenuNamespace, toolBarPrefID, initToolBarFn);

	itsTree = streamInTreeFn(projInput, projVers,
							 setInput, setVers, symStream, origSymVers,
							 this, CBTreeWidget::kBorderWidth, dirList);

	std::istream* symInput             = (projVers <= 41 ? &projInput : symStream);
	const JFileVersion symVers    = (projVers <= 41 ? projVers   : origSymVers);
	const JBoolean useSetProjData = JI2B( setInput == NULL || setVers < 71 );
	const JBoolean useSymProjData = JI2B( symInput == NULL || symVers < 71 );

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
	assert( itsTreeWidget != NULL );
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

	JBoolean active = kJFalse;
	if (31 <= projVers && projVers < 71)
		{
		projInput >> active;
		}
	if (!useSetProjData)	// overwrite
		{
		*setInput >> active;
		}
	if (active && !subProject)
		{
		Activate();
		}

/* symbol file */

	if (17 <= projVers && projVers < 71)
		{
		JSize fnListCount;
		projInput >> fnListCount;

		for (JIndex i=1; i<=fnListCount; i++)
			{
			JBoolean keep;
			CBFnListDirector* dir =
				jnew CBFnListDirector(projInput, projVers, &keep,
									 this, itsFnListPrinter, itsTreeWidget);
			assert( dir != NULL );
			if (useSymProjData && keep && !subProject)
				{
				dir->Activate();
				}
			else
				{
				dir->Close();
				}
			}
		}

	if (!useSymProjData)
		{
		JSize fnListCount;
		*symInput >> fnListCount;

		for (JIndex i=1; i<=fnListCount; i++)
			{
			JBoolean keep;
			CBFnListDirector* dir =
				jnew CBFnListDirector(*symInput, symVers, &keep,
									 this, itsFnListPrinter, itsTreeWidget);
			assert( dir != NULL );
			if (keep && !subProject)
				{
				dir->Activate();
				}
			else
				{
				dir->Close();
				}
			}
		}
}

// private

JXScrollbarSet*
CBTreeDirector::CBTreeDirectorX
	(
	CBProjectDocument*		doc,
	const JXPM&				windowIcon,
	const JCharacter*		treeMenuTitle,
	const JCharacter*		treeMenuItems,
	const JCharacter*		treeMenuNamespace,
	const JIndex			toolBarPrefID,
	CBTreeInitToolBarFn*	initToolBarFn
	)
{
	itsProjDoc = doc;
	ListenTo(itsProjDoc);

	itsFnBrowsers = jnew JPtrArray<CBFnListDirector>(JPtrArrayT::kForgetAll);
	assert( itsFnBrowsers != NULL );

	itsShowInheritedFnsFlag = kJTrue;
	itsFindFnDialog         = NULL;

	JXScrollbarSet* sbarSet = BuildWindow(windowIcon, treeMenuTitle, treeMenuItems,
										  treeMenuNamespace,
										  toolBarPrefID, initToolBarFn);

	// can't call GetWindow() until window is created

	itsPSPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPSPrinter != NULL );
	ListenTo(itsPSPrinter);

	itsEPSPrinter = jnew JXEPSPrinter(GetDisplay());
	assert( itsEPSPrinter != NULL );
	ListenTo(itsEPSPrinter);

	itsFnListPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsFnListPrinter != NULL );

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
	jdelete itsFnBrowsers;	// objects deleted by JXDirector
}

/******************************************************************************
 ReloadSetup (virtual)

 ******************************************************************************/

void
CBTreeDirector::ReloadSetup
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	itsTree->ReloadSetup(input, vers);

	// skip function browser data and reconnect the existing ones

	JSize fnListCount;
	input >> fnListCount;

	for (JIndex i=1; i<=fnListCount; i++)
		{
		CBFnListDirector::SkipSetup(input);
		}

	ReconnectFunctionBrowsers();
}

/******************************************************************************
 StreamOut (virtual)

	dirList, d1, and d2 can be NULL

 ******************************************************************************/

void
CBTreeDirector::StreamOut
	(
	std::ostream&			projOutput,
	std::ostream*			setOutput,
	std::ostream*			symOutput,
	const CBDirList*	dirList
	)
	const
{
	itsTree->StreamOut(projOutput, setOutput, symOutput, dirList);

/* settings file */

	if (setOutput != NULL)
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

		*setOutput << ' ' << IsActive();

		*setOutput << ' ';
		}

/* symbol file */

	if (symOutput != NULL)
		{
		const JSize fnListCount = itsFnBrowsers->GetElementCount();
		*symOutput << ' ' << fnListCount;

		for (JIndex i=1; i<=fnListCount; i++)
			{
			*symOutput << ' ';
			(itsFnBrowsers->GetElement(i))->StreamOut(*symOutput);
			}

		*symOutput << ' ';
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
	const JBoolean reparseAll
	)
{
	itsTree->PrepareForUpdate(reparseAll);
}

/******************************************************************************
 TreeUpdateFinished

 ******************************************************************************/

JBoolean
CBTreeDirector::TreeUpdateFinished
	(
	const JArray<JFAID_t>& deadFileList
	)
{
	const JBoolean ok = itsTree->UpdateFinished(deadFileList);
	if (ok && !CBInUpdateThread())
		{
		ReconnectFunctionBrowsers();
		}
	return ok;
}

/******************************************************************************
 AskForFunctionToFind

 ******************************************************************************/

void
CBTreeDirector::AskForFunctionToFind()
{
	assert( itsFindFnDialog == NULL );

	itsFindFnDialog =
		jnew JXGetStringDialog(this, JGetString("FindFunctionTitle::CBTreeDirector"),
							  JGetString("FindFunctionPrompt::CBTreeDirector"));
	assert( itsFindFnDialog != NULL );

	itsFindFnDialog->BeginDialog();
	ListenTo(itsFindFnDialog);
}

/******************************************************************************
 CloseFunctionBrowsers

 ******************************************************************************/

void
CBTreeDirector::CloseFunctionBrowsers()
{
	const JSize count = itsFnBrowsers->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsFnBrowsers->LastElement())->Close();
		}
}

/******************************************************************************
 ReconnectFunctionBrowsers (private)

 ******************************************************************************/

void
CBTreeDirector::ReconnectFunctionBrowsers()
{
	const JSize count = itsFnBrowsers->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		(itsFnBrowsers->GetElement(i))->Reconnect();
		}
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
	assert( dir != NULL );
	dir->Activate();
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
	const JCharacter*		treeMenuTitle,
	const JCharacter*		treeMenuItems,
	const JCharacter*		treeMenuNamespace,
	const JIndex			toolBarPrefID,
	CBTreeInitToolBarFn*	initToolBarFn
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 400,430, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != NULL );

	itsToolBar =
		jnew JXToolBar(CBGetPrefsManager(), toolBarPrefID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,400);
	assert( itsToolBar != NULL );

// end JXLayout

	AdjustWindowTitle();
	window->SetMinSize(150, 150);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetWMClass(CBGetWMClassInstance(), CBGetTreeWindowClass());

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != NULL );
	scrollbarSet->FitToEnclosure();

	JXDisplay* display = GetDisplay();
	JXImage* icon      = jnew JXImage(display, windowIcon);
	assert( icon != NULL );
	window->SetIcon(icon);

	JPoint desktopLoc;
	JCoordinate w,h;
	if (CBGetPrefsManager()->GetWindowSize(kCBTreeWindSizeID, &desktopLoc, &w, &h))
		{
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
		}

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "CBTreeDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kNewTextEditorCmd, jx_file_new);
	itsFileMenu->SetItemImage(kOpenSomethingCmd, jx_file_open);
	itsFileMenu->SetItemImage(kPrintPSCmd,       jx_file_print);

	CBFileHistoryMenu* recentProjectMenu =
		jnew CBFileHistoryMenu(CBDocumentManager::kProjectFileHistory,
							  itsFileMenu, kRecentProjectMenuCmd, menuBar);
	assert( recentProjectMenu != NULL );

	CBFileHistoryMenu* recentTextMenu =
		jnew CBFileHistoryMenu(CBDocumentManager::kTextFileHistory,
							  itsFileMenu, kRecentTextMenuCmd, menuBar);
	assert( recentTextMenu != NULL );

	itsTreeMenu = menuBar->AppendTextMenu("tree");
	itsTreeMenu->SetTitle(treeMenuTitle, NULL, kJFalse);
	itsTreeMenu->SetMenuItems(treeMenuItems, treeMenuNamespace);
	ListenTo(itsTreeMenu);

	itsProjectMenu = menuBar->AppendTextMenu(kProjectMenuTitleStr);
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
		jnew CBCommandMenu(itsProjDoc, NULL, menuBar,
						  JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsCmdMenu != NULL );
	menuBar->AppendMenu(itsCmdMenu);
	ListenTo(itsCmdMenu);

	CBDocumentMenu* fileListMenu =
		jnew CBDocumentMenu(kFileListMenuTitleStr, menuBar,
						   JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( fileListMenu != NULL );
	menuBar->AppendMenu(fileListMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "CBTreeDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = (CBGetApplication())->CreateHelpMenu(menuBar, "CBTreeDirector");
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

		(CBGetApplication())->AppendHelpMenuToToolBar(itsToolBar, itsHelpMenu);
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
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsTreeMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateTreeMenu();
		}
	else if (sender == itsTreeMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleTreeMenu(selection->GetIndex());
		}

	else if (sender == itsProjectMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateProjectMenu();
		}
	else if (sender == itsProjectMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleProjectMenu(selection->GetIndex());
		}

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrefsMenu();
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandlePrefsMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		(CBGetApplication())->UpdateHelpMenu(itsHelpMenu);
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		(CBGetApplication())->HandleHelpMenu(itsHelpMenu, itsWindowHelpName,
											 selection->GetIndex());
		}

	else if (sender == itsFindFnDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsTreeWidget->FindFunction(itsFindFnDialog->GetString(), kJTrue,
										kJXLeftButton);
			}
		itsFindFnDialog = NULL;
		}

	else if (sender == itsPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsTreeWidget->Print(*itsPSPrinter);
			}
		}

	else if (sender == itsEPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != NULL );
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
	const JBoolean canPrint = !itsTree->IsEmpty();
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
	(CBGetDocumentManager())->SetActiveProjectDocument(itsProjDoc);

	if (index == kNewTextEditorCmd)
		{
		(CBGetDocumentManager())->NewTextDocument();
		}
	else if (index == kNewTextTemplateCmd)
		{
		(CBGetDocumentManager())->NewTextDocumentFromTemplate();
		}
	else if (index == kNewProjectCmd)
		{
		(CBGetDocumentManager())->NewProjectDocument();
		}
	else if (index == kNewShellCmd)
		{
		(CBGetDocumentManager())->NewShellDocument();
		}
	else if (index == kOpenSomethingCmd)
		{
		(CBGetDocumentManager())->OpenSomething();
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
		(JXGetApplication())->Quit();
		}
}

/******************************************************************************
 UpdateProjectMenu (private)

 ******************************************************************************/

void
CBTreeDirector::UpdateProjectMenu()
{
	itsProjectMenu->SetItemEnable(kCloseAllTextCmd,
								  (CBGetDocumentManager())->HasTextDocuments());
	itsProjectMenu->SetItemEnable(kSaveAllTextCmd,
								  (CBGetDocumentManager())->TextDocumentsNeedSave());
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
	(CBGetDocumentManager())->SetActiveProjectDocument(itsProjDoc);

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
		(CBGetDocumentManager())->SaveTextDocuments(kJTrue);
		}
	else if (index == kCloseAllTextCmd)
		{
		(CBGetDocumentManager())->CloseTextDocuments();
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
		(CBGetDocumentManager())->ChooseEditors();
		}
	else if (index == kMiscPrefsCmd)
		{
		(CBGetApplication())->EditMiscPrefs();
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
	CBEditTreePrefsDialog* dlog =
		jnew CBEditTreePrefsDialog(itsTree->GetFontSize(), itsShowInheritedFnsFlag,
								  itsTree->WillAutoMinimizeMILinks(),
								  itsTree->WillDrawMILinksOnTop(),
								  itsTreeWidget->WillRaiseWindowWhenSingleMatch());
	assert( dlog != NULL );
	dlog->BeginDialog();
}

/******************************************************************************
 SetTreePrefs

 ******************************************************************************/

void
CBTreeDirector::SetTreePrefs
	(
	const JSize		fontSize,
	const JBoolean	showInheritedFns,
	const JBoolean	autoMinMILinks,
	const JBoolean	drawMILinksOnTop,
	const JBoolean	raiseWhenSingleMatch
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
	input >> fontSize >> itsShowInheritedFnsFlag;
	itsTree->SetFontSize(fontSize);

	if (vers >= 1)
		{
		JBoolean autoMinMILinks, drawMILinksOnTop;
		input >> autoMinMILinks >> drawMILinksOnTop;
		itsTree->ShouldAutoMinimizeMILinks(autoMinMILinks);
		itsTree->ShouldDrawMILinksOnTop(drawMILinksOnTop);
		}

	if (vers >= 2)
		{
		JBoolean raiseWhenSingleMatch;
		input >> raiseWhenSingleMatch;
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
	output << ' ' << itsShowInheritedFnsFlag;
	output << ' ' << itsTree->WillAutoMinimizeMILinks();
	output << ' ' << itsTree->WillDrawMILinksOnTop();
	output << ' ' << itsTreeWidget->WillRaiseWindowWhenSingleMatch();
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
		CBCommandMenu::GetTargetInfo* info =
			dynamic_cast<CBCommandMenu::GetTargetInfo*>(message);
		assert( info != NULL );

		JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
		if (itsTree->GetSelectedClasses(&classList))
			{
			const JSize count = classList.GetElementCount();
			JString fullName;
			for (JIndex i=1; i<=count; i++)
				{
				if ((classList.GetElement(i))->GetFileName(&fullName))
					{
					info->AddFile(fullName);
					}
				}
			}
		}
	else
		{
		JXWindowDirector::ReceiveWithFeedback(sender, message);
		}
}

/*****************************************************************************
 DirectorClosed (virtual protected)

	Listen for function browsers that are closed.

 ******************************************************************************/

void
CBTreeDirector::DirectorClosed
	(
	JXDirector* theDirector
	)
{
	itsFnBrowsers->Remove((CBFnListDirector*) theDirector);	// safe: doesn't call object
	JXWindowDirector::DirectorClosed(theDirector);
}
