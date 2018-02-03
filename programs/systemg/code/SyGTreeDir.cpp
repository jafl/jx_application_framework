/******************************************************************************
 SyGTreeDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "SyGTreeDir.h"
#include "SyGTreeSet.h"
#include "SyGFileTreeTable.h"
#include "SyGFileTree.h"
#include "SyGFolderDragSource.h"
#include "SyGPathInput.h"
#include "SyGTrashButton.h"
#include "SyGPrefsMgr.h"
#include "SyGGlobals.h"
#include "SyGFileVersions.h"
#include "SyGActionDefs.h"
#include <JXFSBindingManager.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXToolBar.h>
#include <JXTextButton.h>
#include <JXWDManager.h>
#include <JXWDMenu.h>
#include <JXCurrentPathMenu.h>
#include <JXHelpManager.h>
#include <JXMacWinPrefsDialog.h>
#include <jMountUtil.h>
#include <jFileUtil.h>
#include <jSysUtil.h>
#include <jFStreamUtil.h>
#include <sstream>
#include <jAssert.h>

#include <jx_help_specific.xpm>
#include <jx_help_toc.xpm>

const JCharacter* kDirPrefsName     = ".systemg_folder_prefs_";
const JCharacter* kOrigDirPrefsName = ".systemG.Desktop";

// Windows menu

static const JCharacter* kWindowsMenuTitleStr = "Windows";

// Preferences menu

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
	"    Preferences..."
	"  | File bindings..."
	"  | Toolbar buttons..."
	"  | Mac/Win/X emulation..."
	"%l| Save window setup as default";

enum
{
	kEditPrefsCmd = 1,
	kEditBindings,
	kEditToolBarCmd,
	kEditMacWinPrefsCmd,
	kSaveWindSizeCmd
};

// Help menu

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"    About"
	"%l| Table of Contents %k F1 %i" kJXHelpTOCAction
	"  | Getting started"
//	"  | This window       %k F1 %i" kJXHelpSpecificAction
	"%l| Changes"
	"  | Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kOverviewCmd,
//	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

// string ID's

static const JCharacter* kTrashNameID       = "TrashName::SyGTreeDir";
static const JCharacter* kTrashButtonHintID = "TrashButtonHint::SyGTreeDir";

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGTreeDir::SyGTreeDir
	(
	const JCharacter* startPath
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsAutoMountFlag(kJFalse)
{
	if ((SyGGetApplication())->IsMountPoint(startPath) &&
		!JIsMounted(startPath))
		{
		itsAutoMountFlag = kJTrue;
		(SyGGetApplication())->DisplayBusyCursor();
		JMount(startPath, kJTrue, kJTrue);
		}

	BuildWindow(startPath);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGTreeDir::~SyGTreeDir()
{
	if (itsAutoMountFlag)
		{
		(SyGGetApplication())->DisplayBusyCursor();		// can take time to flush buffers
		JMount(GetDirectory(), kJFalse);
		}
}

/******************************************************************************
 Close (virtual)

 ******************************************************************************/

JBoolean
SyGTreeDir::Close()
{
	SaveState();
	return JXWindowDirector::Close();
}

/******************************************************************************
 GetTable

 ******************************************************************************/

SyGFileTreeTable*
SyGTreeDir::GetTable()
	const
{
	return itsTreeSet->GetTable();
}

/******************************************************************************
 GetName

 ******************************************************************************/

const JString&
SyGTreeDir::GetName()
	const
{
	if (SyGIsTrashDirectory(GetDirectory()))
		{
		return JGetString(kTrashNameID);
		}
	else
		{
		return itsPathInput->GetText();
		}
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

JBoolean
SyGTreeDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = SyGGetDirectorySmallIcon(GetDirectory());
	return kJTrue;
}

/******************************************************************************
 GetDirectory

 ******************************************************************************/

const JString&
SyGTreeDir::GetDirectory()
	const
{
	return GetTable()->GetFileTree()->GetDirectory();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SyGTreeDir::BuildWindow
	(
	const JCharacter* startPath
	)
{
	JXCurrentPathMenu* pathMenu = NULL;

// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 420,500, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 420,30);
	assert( menuBar != NULL );

	itsToolBar =
		jnew JXToolBar(SyGGetPrefsMgr(), kSMainToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 420,450);
	assert( itsToolBar != NULL );

	itsPathInput =
		jnew SyGPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,480, 340,20);
	assert( itsPathInput != NULL );

	SyGTrashButton* trashButton =
		jnew SyGTrashButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 390,480, 30,20);
	assert( trashButton != NULL );

	itsDragSrc =
		jnew SyGFolderDragSource(itsPathInput, &pathMenu, window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 0,480, 20,20);
	assert( itsDragSrc != NULL );

	itsUpButton =
		jnew JXTextButton(JGetString("itsUpButton::SyGTreeDir::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 360,480, 30,20);
	assert( itsUpButton != NULL );

// end JXLayout

	// folder setup

	JString prefsFile = JCombinePathAndName(startPath, kDirPrefsName);
	prefsFile += JGetUserName();
	if (!JFileExists(prefsFile))
		{
		const JString origPrefsFile = JCombinePathAndName(startPath, kOrigDirPrefsName);
		JRenameFile(origPrefsFile, prefsFile);
		}

	std::istream* input       = NULL;
	const JString* prefs = NULL;
	std::string s;
	if (!JFileReadable(prefsFile) &&
		(SyGGetApplication())->GetMountPointPrefs(startPath, &prefs))
		{
		s.assign(prefs->GetCString(), prefs->GetLength());
		input = jnew std::istringstream(s);
		}
	else
		{
		input = jnew std::ifstream(prefsFile);
		}
	assert( input != NULL );

	JFileVersion vers = 0;
	JSize w, h;
	if (input->good())
		{
		*input >> vers;

		if (vers <= kSyGCurrentDirSetupVersion)
			{
			window->ReadGeometry(*input);
			}
		else
			{
			jdelete input;
			input = NULL;
			}
		}
	else if ((SyGGetPrefsMgr())->GetDefaultWindowSize(&w, &h))
		{
		window->SetSize(w,h);

		jdelete input;
		input = NULL;
		}
	window->SetCloseAction(JXWindow::kCloseDirector);
	window->SetWMClass(SyGGetWMClassInstance(), SyGGetFolderWindowClass());
	window->ShouldFocusWhenShow(kJTrue);
	window->SetMinSize(150, 150);

	// Up button

	ListenTo(itsUpButton);

	// trash button

	trashButton->SetHint(JGetString(kTrashButtonHintID));

	// widgets

	if (input != NULL && input->good() && vers <= kSyGCurrentDirSetupVersion)
		{
		if (vers < 2)
			{
			JFileVersion v;
			*input >> v;
			}

		itsTreeSet =
			jnew SyGTreeSet(*input, vers, menuBar, startPath,
							itsPathInput, pathMenu, trashButton,
							itsToolBar->GetWidgetEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 1000,1000);
		}
	else
		{
		itsTreeSet =
			jnew SyGTreeSet(menuBar, startPath, itsPathInput, pathMenu,
							trashButton, itsToolBar->GetWidgetEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 1000,1000);
		}
	assert( itsTreeSet != NULL );
	// itsTreeSet has already called FitToEnclosure()
	ListenTo(GetTable());

	// menus

	JXWDMenu* windowsMenu =
		jnew JXWDMenu(kWindowsMenuTitleStr, menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( windowsMenu != NULL );
	menuBar->AppendMenu(windowsMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "SyGTreeDir");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "SyGTreeDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd, jx_help_toc);
//	itsHelpMenu->SetItemImage(kThisWindowCmd, JXPM(jx_help_specific));

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
		{
		GetTable()->LoadToolBarDefaults(itsToolBar);
		}

	// clean up

	jdelete input;

	GetDisplay()->GetWDManager()->DirectorCreated(this);
}

/******************************************************************************
 SaveState

 ******************************************************************************/

void
SyGTreeDir::SaveState()
{
	if (!(SyGGetPrefsMgr())->WillSaveFolderPrefs())
		{
		return;
		}

	const JString& path = GetDirectory();
	if ((SyGGetApplication())->IsMountPoint(path))
		{
		std::ostringstream data;
		WriteState(data);
		const std::string s = data.str();
		(SyGGetApplication())->SetMountPointPrefs(path, s.c_str());
		}

	JString prefsFile = JCombinePathAndName(path, kDirPrefsName);
	prefsFile += JGetUserName();

	// don't overwrite newer version of prefs

	std::ifstream input(prefsFile);
	if (input.good())
		{
		JFileVersion vers;
		input >> vers;
		if (vers > kSyGCurrentDirSetupVersion)
			{
			return;
			}
		}
	input.close();

	std::ofstream output(prefsFile);
	WriteState(output);
}

/******************************************************************************
 WriteState (private)

 ******************************************************************************/

void
SyGTreeDir::WriteState
	(
	std::ostream& output
	)
{
	output << kSyGCurrentDirSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsTreeSet->SavePreferences(output);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SyGTreeDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandlePrefsMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}

	else if (sender == itsUpButton && message.Is(JXButton::kPushed))
		{
		GetTable()->GoUp((GetDisplay()->GetLatestKeyModifiers()).meta());
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
SyGTreeDir::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kEditPrefsCmd)
		{
		(SyGGetPrefsMgr())->EditPrefs();
		}
	else if (index == kEditBindings)
		{
		(JXFSBindingManager::Instance())->EditBindings();
		}
	else if (index == kEditToolBarCmd)
		{
		itsToolBar->Edit();
		}
	else if (index == kEditMacWinPrefsCmd)
		{
		JXMacWinPrefsDialog::EditPrefs();
		}

	else if (index == kSaveWindSizeCmd)
		{
		GetTable()->SavePrefsAsDefault();
		}
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
SyGTreeDir::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		(SyGGetApplication())->DisplayAbout();
		}
	else if (index == kTOCCmd)
		{
		(JXGetHelpManager())->ShowTOC();
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection("SyGGettingStartedHelp");
		}
//	else if (index == kThisWindowCmd)
//		{
//		(JXGetHelpManager())->ShowSection("...");
//		}
	else if (index == kChangesCmd)
		{
		(JXGetHelpManager())->ShowChanges();
		}
	else if (index == kCreditsCmd)
		{
		(JXGetHelpManager())->ShowCredits();
		}
}
