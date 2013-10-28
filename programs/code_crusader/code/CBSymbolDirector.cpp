/******************************************************************************
 CBSymbolDirector.cpp

	Window to display CBSymbolList.

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBSymbolDirector.h"
#include "CBSymbolSRDirector.h"
#include "CBSymbolTable.h"
#include "CBSymbolList.h"
#include "CBSymbolTypeList.h"
#include "CBEditSymbolPrefsDialog.h"
#include "CBFileHistoryMenu.h"
#include "CBViewManPageDialog.h"
#include "CBFindFileDialog.h"
#include "CBDiffFileDialog.h"
#include "CBSearchTextDialog.h"
#include "CBProjectDocument.h"
#include "CBFileListTable.h"
#include "CBCTreeDirector.h"
#include "CBJavaTreeDirector.h"
#include "CBTreeWidget.h"
#include "CBTree.h"
#include "CBClass.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "CBFileListDirector.h"
#include "CBCommandMenu.h"
#include "CBDocumentMenu.h"
#include "cbActionDefs.h"
#include "cbGlobals.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <JXScrollbarSet.h>
#include <JXImage.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jAssert.h>

static const JCharacter* kWindowTitleSuffix = " Symbols";

// setup information

const JFileVersion kCurrentPrefsVersion = 0;

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
	"%l| Close                          %k Meta-W       %i" kJXCloseWindowAction
	"  | Quit                           %k Meta-Q       %i" kJXQuitAction;

enum
{
	kNewTextEditorCmd = 1, kNewTextTemplateCmd, kNewProjectCmd, kNewShellCmd,
	kOpenSomethingCmd,
	kRecentProjectMenuCmd, kRecentTextMenuCmd,
	kCloseCmd, kQuitCmd
};

// Symbol menu

static const JCharacter* kSymbolMenuTitleStr = "Symbol";
static const JCharacter* kSymbolMenuStr =
	"    Edit search paths...                               %i" kCBEditSearchPathsAction
	"  | Update                 %k Meta-U                   %i" kCBUpdateClassTreeAction
	"%l| Open file              %k Left-dbl-click or Return %i" kCBOpenSelectedFilesAction
	"  | Copy selected names    %k Meta-C                   %i" kCBCopySymbolNameAction
//	"%l| Find symbol...         %k Meta-F                   %i" kCBFindFunctionAction
	"%l| Find selected symbols  %k Meta-dbl-click           %i" kCBFindSelectionInProjectAction
	"  | Close symbol browsers                              %i" kCBCloseAllSymSRAction;

enum
{
	kEditSearchPathsCmd = 1, kUpdateCurrentCmd,
	kOpenFileCmd, kCopySelNamesCmd,
//	kFindSymbolCmd,
	kFindSelectedSymbolCmd,
	kCloseAllSymSRCmd
};

// Project menu

static const JCharacter* kProjectMenuTitleStr = "Project";
static const JCharacter* kProjectMenuStr =
	"    Show C++ class tree                 %i" kCBShowCPPClassTreeAction
	"  | Show Java class tree                %i" kCBShowJavaClassTreeAction
	"  | Look up man page... %k Meta-I       %i" kCBViewManPageAction
	"%l| Show file list      %k Meta-Shift-F %i" kCBShowFileListAction
	"  | Find file...        %k Meta-D       %i" kCBFindFileAction
	"  | Search files...     %k Meta-F       %i" kCBSearchFilesAction
	"  | Compare files...                    %i" kCBDiffFilesAction
	"%l| Save all            %k Meta-Shift-S %i" kCBSaveAllTextFilesAction
	"  | Close all           %k Meta-Shift-W %i" kCBCloseAllTextFilesAction;

enum
{
	kShowCTreeCmd = 1, kShowJavaTreeCmd, kViewManPageCmd,
	kShowFileListCmd, kFindFileCmd, kSearchFilesCmd, kDiffFilesCmd,
	kSaveAllTextCmd, kCloseAllTextCmd
};

// Windows menu

static const JCharacter* kFileListMenuTitleStr = "Windows";

// Preferences menu

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
	"    Symbols..."
	"  | Toolbar buttons..."
	"  | File types..."
	"  | External editors..."
	"  | Miscellaneous..."
	"%l| Save window size as default";

enum
{
	kSymbolPrefsCmd = 1, kToolBarPrefsCmd,
	kEditFileTypesCmd, kChooseExtEditorsCmd, kMiscPrefsCmd,
	kSaveWindSizeCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSymbolDirector::CBSymbolDirector
	(
	CBProjectDocument* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(CBGetPrefsManager(), kCBSymbolDirectorID)
{
	CBSymbolDirectorX(supervisor);
}

CBSymbolDirector::CBSymbolDirector
	(
	istream&			projInput,
	const JFileVersion	projVers,
	istream*			setInput,
	const JFileVersion	setVers,
	istream*			symInput,
	const JFileVersion	symVers,
	CBProjectDocument*	supervisor,
	const JBoolean		subProject
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(CBGetPrefsManager(), kCBSymbolDirectorID)
{
	CBSymbolDirectorX(supervisor);

	const JBoolean useProjData = JI2B( setInput == NULL || setVers < 71 );
	if (!useProjData)
		{
		(GetWindow())->ReadGeometry(*setInput);

		// put SR windows on top of main window

		JBoolean active;
		*setInput >> active;
		if (active && !subProject)
			{
			Activate();
			}

		// read and create CBSymbolSRDirectors here
		}

	if (projVers >= 41)
		{
		if (projVers < 71 && useProjData)
			{
			(GetWindow())->ReadGeometry(projInput);
			}
		else if (projVers < 71)
			{
			JXWindow::SkipGeometry(projInput);
			}

		if (45 <= projVers && projVers < 74)
			{
			CBSymbolTypeList::SkipSetup(projInput, projVers);
			}

		itsSymbolList->ReadSetup(projInput, projVers, symInput, symVers);

		JBoolean active = kJFalse;
		if (47 <= projVers && projVers < 71)
			{
			projInput >> active;
			}
		if (useProjData && active && !subProject)
			{
			Activate();
			}
		}
}

// private

void
CBSymbolDirector::CBSymbolDirectorX
	(
	CBProjectDocument* projDoc
	)
{
	itsProjDoc = projDoc;
	ListenTo(itsProjDoc);

	itsSymbolList = new CBSymbolList(projDoc);
	assert( itsSymbolList != NULL );

	itsSRList = new JPtrArray<CBSymbolSRDirector>(JPtrArrayT::kForgetAll);
	assert( itsSRList != NULL );

	itsRaiseTreeOnRightClickFlag = kJFalse;

	BuildWindow(itsSymbolList);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSymbolDirector::~CBSymbolDirector()
{
	CloseSymbolBrowsers();

	delete itsSymbolList;
	delete itsSRList;			// objects deleted by JXDirector
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CBSymbolDirector::ReadSetup
	(
	istream&			symInput,
	const JFileVersion	symVers
	)
{
	CloseSymbolBrowsers();
	itsSymbolList->ReadSetup(symInput, symVers);
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
CBSymbolDirector::StreamOut
	(
	ostream& projOutput,
	ostream* setOutput,
	ostream* symOutput
	)
	const
{
	if (setOutput != NULL)
		{
		*setOutput << ' ';
		(GetWindow())->WriteGeometry(*setOutput);
		*setOutput << ' ' << IsActive() << ' ';
		}

	itsSymbolList->WriteSetup(projOutput, symOutput);
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBSymbolDirector::ReadPrefs
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentPrefsVersion)
		{
		return;
		}

	input >> itsRaiseTreeOnRightClickFlag;
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBSymbolDirector::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;
	output << ' ' << itsRaiseTreeOnRightClickFlag;
}

/******************************************************************************
 CloseSymbolBrowsers

 ******************************************************************************/

void
CBSymbolDirector::CloseSymbolBrowsers()
{
	const JSize count = itsSRList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsSRList->LastElement())->Close();
		}
}

/*****************************************************************************
 SRDirectorClosed

 ******************************************************************************/

void
CBSymbolDirector::SRDirectorClosed
	(
	CBSymbolSRDirector* dir
	)
{
	itsSRList->Remove(dir);
}

/******************************************************************************
 FileTypesChanged

 ******************************************************************************/

void
CBSymbolDirector::FileTypesChanged
	(
	const CBPrefsManager::FileTypesChanged& info
	)
{
	itsSymbolList->FileTypesChanged(info);
}

/******************************************************************************
 PrepareForListUpdate

 ******************************************************************************/

void
CBSymbolDirector::PrepareForListUpdate
	(
	const JBoolean		reparseAll,
	JProgressDisplay&	pg
	)
{
	itsSymbolList->PrepareForUpdate(reparseAll, pg);
}

/******************************************************************************
 ListUpdateFinished

 ******************************************************************************/

JBoolean
CBSymbolDirector::ListUpdateFinished
	(
	const JArray<JFAID_t>&	deadFileList,
	JProgressDisplay&		pg
	)
{
	const JBoolean ok = itsSymbolList->UpdateFinished(deadFileList, pg);
	if (ok && !CBInUpdateThread())
		{
		CloseSymbolBrowsers();
//		UpdateSymbolBrowsers();
		}
	return ok;
}

/******************************************************************************
 FindSymbol

	If fileName is not empty, it is used for context.

 ******************************************************************************/

JBoolean
CBSymbolDirector::FindSymbol
	(
	const JCharacter*	name,
	const JCharacter*	fileName,
	const JXMouseButton	button
	)
{
	(JXGetApplication())->DisplayBusyCursor();

	CBTree* cTree    = (itsProjDoc->GetCTreeDirector())->GetTree();
	CBTree* javaTree = (itsProjDoc->GetJavaTreeDirector())->GetTree();

	JFAID_t contextFileID = JFAID::kInvalidID;
	JString contextNamespace;
	CBLanguage contextLang = kCBOtherLang;
	JPtrArray<JString> cContextNamespaceList(JPtrArrayT::kDeleteAll);
	JPtrArray<JString> javaContextNamespaceList(JPtrArrayT::kDeleteAll);
	if (!JStringEmpty(fileName))
		{
		(itsProjDoc->GetAllFileList())->GetFileID(fileName, &contextFileID);

		JString path, name, className, suffix;
		JSplitPathAndName(fileName, &path, &name);
		JSplitRootAndSuffix(name, &className, &suffix);

		const CBClass* theClass;
		if (cTree->IsUniqueClassName(className, &theClass))
			{
			BuildAncestorList(*theClass, &cContextNamespaceList);
			}
		if (javaTree->IsUniqueClassName(className, &theClass))
			{
			BuildAncestorList(*theClass, &javaContextNamespaceList);
			}
		if (cContextNamespaceList.IsEmpty() &&
			javaContextNamespaceList.IsEmpty() &&
			itsSymbolList->IsUniqueClassName(className, &contextLang))
			{
			contextNamespace = className;
			}
		}

	JArray<JIndex> symbolList;
	const JBoolean foundSymbol =
		itsSymbolList->FindSymbol(name,
			contextFileID, contextNamespace, contextLang,
			&cContextNamespaceList, &javaContextNamespaceList,
			JI2B(button == kJXMiddleButton || button == kJXRightButton),
			JI2B(button == kJXLeftButton   || button == kJXRightButton),
			&symbolList);

	const JBoolean raiseTree =
		JI2B(!foundSymbol || (button == kJXRightButton && itsRaiseTreeOnRightClickFlag));

	CBTreeWidget* treeWidget = (itsProjDoc->GetCTreeDirector())->GetTreeWidget();
	const JBoolean cc = treeWidget->FindClass(name, button, raiseTree, kJFalse, raiseTree, kJTrue);
	const JBoolean cf = treeWidget->FindFunction(name, kJTrue, button, raiseTree, kJFalse, raiseTree, kJFalse);

	treeWidget = (itsProjDoc->GetJavaTreeDirector())->GetTreeWidget();
	const JBoolean jc = treeWidget->FindClass(name, button, raiseTree, kJFalse, raiseTree, kJTrue);
	const JBoolean jf = treeWidget->FindFunction(name, kJTrue, button, raiseTree, kJFalse, raiseTree, kJFalse);

	if (symbolList.GetElementCount() == 1 && button != kJXRightButton)
		{
		const JIndex symbolIndex = symbolList.GetFirstElement();

		CBLanguage lang;
		CBSymbolList::Type type;
		itsSymbolList->GetSymbol(symbolIndex, &lang, &type);

		JIndex lineIndex;
		const JString& fileName =
			itsSymbolList->GetFile(symbolIndex, &lineIndex);

		CBTextDocument* doc = NULL;
		if ((CBGetDocumentManager())->OpenTextDocument(fileName, lineIndex, &doc) &&
			CBSymbolList::ShouldSmartScroll(type))
			{
			(doc->GetTextEditor())->ScrollForDefinition(lang);
			}
		return kJTrue;
		}
	else if (foundSymbol)
		{
		CBSymbolSRDirector* dir =
			new CBSymbolSRDirector(this, itsProjDoc, itsSymbolList,
								   symbolList, name);
		assert( dir != NULL );
		dir->Activate();
		itsSRList->Append(dir);
		return kJTrue;
		}
	else
		{
		return JI2B( cc || cf || jc || jf );
		}
}

/******************************************************************************
 BuildAncestorList (private)

 ******************************************************************************/

void
CBSymbolDirector::BuildAncestorList
	(
	const CBClass&		theClass,
	JPtrArray<JString>*	list
	)
	const
{
	list->Append(theClass.GetFullName());

	const JSize count = theClass.GetParentCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CBClass* p;
		if (theClass.GetParent(i, &p))
			{
			BuildAncestorList(*p, list);
			}
		}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "jcc_symbol_window.xpm"

#include <jx_file_new.xpm>
#include <jx_file_open.xpm>
#include "jcc_show_c_tree.xpm"
#include "jcc_show_java_tree.xpm"
#include "jcc_view_man_page.xpm"
#include "jcc_show_file_list.xpm"
#include "jcc_search_files.xpm"
#include "jcc_compare_files.xpm"
#include <jx_file_save_all.xpm>

void
CBSymbolDirector::BuildWindow
	(
	CBSymbolList* symbolList
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 400,430, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		new JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != NULL );

	itsToolBar =
		new JXToolBar(CBGetPrefsManager(), kCBSymbolToolBarID, menuBar, 150,150, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,400);
	assert( itsToolBar != NULL );

// end JXLayout

	AdjustWindowTitle();
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetWMClass(CBGetWMClassInstance(), CBGetSymbolWindowClass());

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != NULL );
	scrollbarSet->FitToEnclosure();

	itsSymbolTable =
		new CBSymbolTable(this, symbolList,
						  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsSymbolTable != NULL );
	itsSymbolTable->FitToEnclosure();

	JXDisplay* display = GetDisplay();
	JXImage* icon      = new JXImage(display, jcc_symbol_window);
	assert( icon != NULL );
	window->SetIcon(icon);

	JPoint desktopLoc;
	JCoordinate w,h;
	if ((CBGetPrefsManager())->GetWindowSize(kCBSymbolWindSizeID, &desktopLoc, &w, &h))
		{
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
		}

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "CBSymbolDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kNewTextEditorCmd, jx_file_new);
	itsFileMenu->SetItemImage(kOpenSomethingCmd, jx_file_open);

	CBFileHistoryMenu* recentProjectMenu =
		new CBFileHistoryMenu(CBDocumentManager::kProjectFileHistory,
							  itsFileMenu, kRecentProjectMenuCmd, menuBar);
	assert( recentProjectMenu != NULL );

	CBFileHistoryMenu* recentTextMenu =
		new CBFileHistoryMenu(CBDocumentManager::kTextFileHistory,
							  itsFileMenu, kRecentTextMenuCmd, menuBar);
	assert( recentTextMenu != NULL );

	itsSymbolMenu = menuBar->AppendTextMenu(kSymbolMenuTitleStr);
	itsSymbolMenu->SetMenuItems(kSymbolMenuStr, "CBSymbolDirector");
	ListenTo(itsSymbolMenu);

	itsProjectMenu = menuBar->AppendTextMenu(kProjectMenuTitleStr);
	itsProjectMenu->SetMenuItems(kProjectMenuStr, "CBSymbolDirector");
	itsProjectMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsProjectMenu);

	itsProjectMenu->SetItemImage(kShowCTreeCmd,    jcc_show_c_tree);
	itsProjectMenu->SetItemImage(kShowJavaTreeCmd, jcc_show_java_tree);
	itsProjectMenu->SetItemImage(kViewManPageCmd,  jcc_view_man_page);
	itsProjectMenu->SetItemImage(kShowFileListCmd, jcc_show_file_list);
	itsProjectMenu->SetItemImage(kSearchFilesCmd,  jcc_search_files);
	itsProjectMenu->SetItemImage(kDiffFilesCmd,    jcc_compare_files);
	itsProjectMenu->SetItemImage(kSaveAllTextCmd,  jx_file_save_all);

	itsCmdMenu =
		new CBCommandMenu(itsProjDoc, NULL, menuBar,
						  JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsCmdMenu != NULL );
	menuBar->AppendMenu(itsCmdMenu);
	ListenTo(itsCmdMenu);

	CBDocumentMenu* fileListMenu =
		new CBDocumentMenu(kFileListMenuTitleStr, menuBar,
						   JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( fileListMenu != NULL );
	menuBar->AppendMenu(fileListMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "CBSymbolDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = (CBGetApplication())->CreateHelpMenu(menuBar, "CBSymbolDirector");
	ListenTo(itsHelpMenu);

	// must do this after creating menus

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
		{
		itsToolBar->AppendButton(itsFileMenu, kNewTextEditorCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenSomethingCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsSymbolMenu, kEditSearchPathsCmd);

		(CBGetApplication())->AppendHelpMenuToToolBar(itsToolBar, itsHelpMenu);
		}
}

/******************************************************************************
 AdjustWindowTitle (private)

 ******************************************************************************/

void
CBSymbolDirector::AdjustWindowTitle()
{
	const JString title = itsProjDoc->GetName() + kWindowTitleSuffix;
	(GetWindow())->SetTitle(title);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBSymbolDirector::Receive
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

	else if (sender == itsSymbolMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateSymbolMenu();
		}
	else if (sender == itsSymbolMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleSymbolMenu(selection->GetIndex());
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
		(CBGetApplication())->HandleHelpMenu(itsHelpMenu, kCBSymbolHelpName,
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
 UpdateFileMenu (private)

 ******************************************************************************/

void
CBSymbolDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kCloseCmd, !(GetWindow())->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
CBSymbolDirector::HandleFileMenu
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

	else if (index == kCloseCmd)
		{
		(GetWindow())->Close();
		}
	else if (index == kQuitCmd)
		{
		(JXGetApplication())->Quit();
		}
}

/******************************************************************************
 UpdateSymbolMenu (private)

 ******************************************************************************/

void
CBSymbolDirector::UpdateSymbolMenu()
{
	itsSymbolMenu->EnableItem(kEditSearchPathsCmd);
	itsSymbolMenu->EnableItem(kUpdateCurrentCmd);

	if (!itsSymbolList->IsEmpty())
		{
//		itsSymbolMenu->EnableItem(kFindSymbolCmd);
		}

	if (itsSymbolTable->HasSelection())
		{
		itsSymbolMenu->EnableItem(kOpenFileCmd);
		itsSymbolMenu->EnableItem(kCopySelNamesCmd);
		itsSymbolMenu->EnableItem(kFindSelectedSymbolCmd);
		}

	if (HasSymbolBrowsers())
		{
		itsSymbolMenu->EnableItem(kCloseAllSymSRCmd);
		}
}

/******************************************************************************
 HandleSymbolMenu (private)

 ******************************************************************************/

void
CBSymbolDirector::HandleSymbolMenu
	(
	const JIndex index
	)
{
	(CBGetDocumentManager())->SetActiveProjectDocument(itsProjDoc);

	if (index == kEditSearchPathsCmd)
		{
		itsProjDoc->EditSearchPaths(this);
		}
	else if (index == kUpdateCurrentCmd)
		{
		itsProjDoc->UpdateSymbolDatabase();
		}

	else if (index == kOpenFileCmd)
		{
		itsSymbolTable->DisplaySelectedSymbols();
		}
	else if (index == kCopySelNamesCmd)
		{
		itsSymbolTable->CopySelectedSymbolNames();
		}

//	else if (index == kFindSymbolCmd)
//		{
//		}
	else if (index == kFindSelectedSymbolCmd)
		{
		itsSymbolTable->FindSelectedSymbols(kJXRightButton);
		}
	else if (index == kCloseAllSymSRCmd)
		{
		CloseSymbolBrowsers();
		}
}

/******************************************************************************
 UpdateProjectMenu (private)

 ******************************************************************************/

void
CBSymbolDirector::UpdateProjectMenu()
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
CBSymbolDirector::HandleProjectMenu
	(
	const JIndex index
	)
{
	(CBGetDocumentManager())->SetActiveProjectDocument(itsProjDoc);

	if (index == kShowCTreeCmd)
		{
		(itsProjDoc->GetCTreeDirector())->Activate();
		}
	else if (index == kShowJavaTreeCmd)
		{
		(itsProjDoc->GetJavaTreeDirector())->Activate();
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
CBSymbolDirector::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
CBSymbolDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kSymbolPrefsCmd)
		{
		EditPrefs();
		}
	else if (index == kToolBarPrefsCmd)
		{
		itsToolBar->Edit();
		}
	else if (index == kEditFileTypesCmd)
		{
		(CBGetPrefsManager())->EditFileTypes();
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
		(CBGetPrefsManager())->SaveWindowSize(kCBSymbolWindSizeID, GetWindow());
		}
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
CBSymbolDirector::EditPrefs()
{
	CBEditSymbolPrefsDialog* dlog =
		new CBEditSymbolPrefsDialog(itsRaiseTreeOnRightClickFlag);
	assert( dlog != NULL );
	dlog->BeginDialog();
}

/******************************************************************************
 SetPrefs

 ******************************************************************************/

void
CBSymbolDirector::SetPrefs
	(
	const JBoolean raiseTreeOnRightClick,
	const JBoolean writePrefs
	)
{
	itsRaiseTreeOnRightClickFlag = raiseTreeOnRightClick;

	if (writePrefs)
		{
		JPrefObject::WritePrefs();
		}
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
CBSymbolDirector::ReceiveWithFeedback
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
		itsSymbolTable->GetFileNamesForSelection(info->GetFileList(),
												 info->GetLineIndexList());
		}
	else
		{
		JXWindowDirector::ReceiveWithFeedback(sender, message);
		}
}

#define JTemplateType CBSymbolSRDirector
#include <JPtrArray.tmpls>
#undef JTemplateType
