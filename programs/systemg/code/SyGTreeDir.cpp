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

#include <jx_help_toc.xpm>

const JString kDirPrefsName    (".systemg_folder_prefs_", JString::kNoCopy);
const JString kOrigDirPrefsName(".systemG.Desktop", JString::kNoCopy);

// Preferences menu

static const JUtf8Byte* kPrefsMenuStr =
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

static const JUtf8Byte* kHelpMenuStr =
	"    About"
	"%l| Table of Contents %k F1 %i" kJXHelpTOCAction
	"  | Getting started"
	"%l| Changes"
	"  | Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kOverviewCmd,
	kChangesCmd,
	kCreditsCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGTreeDir::SyGTreeDir
	(
	const JString& startPath
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsAutoMountFlag(false)
{
	if ((SyGGetApplication())->IsMountPoint(startPath) &&
		!JIsMounted(startPath))
		{
		itsAutoMountFlag = true;
		(SyGGetApplication())->DisplayBusyCursor();
		JMount(startPath, true, true);
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
		JMount(GetDirectory(), false);
		}
}

/******************************************************************************
 Close (virtual)

 ******************************************************************************/

bool
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
		return JGetString("TrashName::SyGGlobals");
		}
	else
		{
		return itsPathInput->GetText()->GetText();
		}
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
SyGTreeDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = SyGGetDirectorySmallIcon(GetDirectory());
	return true;
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
	const JString& startPath
	)
{
	JXCurrentPathMenu* pathMenu = nullptr;

// begin JXLayout

	auto* window = jnew JXWindow(this, 420,500, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 420,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(SyGGetPrefsMgr(), kSMainToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 420,450);
	assert( itsToolBar != nullptr );

	itsPathInput =
		jnew SyGPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,480, 340,20);
	assert( itsPathInput != nullptr );

	auto* trashButton =
		jnew SyGTrashButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 390,480, 30,20);
	assert( trashButton != nullptr );

	itsDragSrc =
		jnew SyGFolderDragSource(itsPathInput, &pathMenu, window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 0,480, 20,20);
	assert( itsDragSrc != nullptr );

	itsUpButton =
		jnew JXTextButton(JGetString("itsUpButton::SyGTreeDir::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 360,480, 30,20);
	assert( itsUpButton != nullptr );

// end JXLayout

	// folder setup

	JString prefsFile = JCombinePathAndName(startPath, kDirPrefsName);
	prefsFile += JGetUserName();
	if (!JFileExists(prefsFile))
		{
		const JString origPrefsFile = JCombinePathAndName(startPath, kOrigDirPrefsName);
		JRenameFile(origPrefsFile, prefsFile);
		}

	std::istream* input  = nullptr;
	const JString* prefs = nullptr;
	std::string s;
	if (!JFileReadable(prefsFile) &&
		(SyGGetApplication())->GetMountPointPrefs(startPath, &prefs))
		{
		s.assign(prefs->GetBytes(), prefs->GetByteCount());
		input = jnew std::istringstream(s);
		}
	else
		{
		input = jnew std::ifstream(prefsFile.GetBytes());
		}
	assert( input != nullptr );

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
			input = nullptr;
			}
		}
	else if ((SyGGetPrefsMgr())->GetDefaultWindowSize(&w, &h))
		{
		window->SetSize(w,h);

		jdelete input;
		input = nullptr;
		}
	window->SetCloseAction(JXWindow::kCloseDirector);
	window->SetWMClass(SyGGetWMClassInstance(), SyGGetFolderWindowClass());
	window->ShouldFocusWhenShow(true);
	window->SetMinSize(150, 150);

	// Up button

	ListenTo(itsUpButton);

	// trash button

	trashButton->SetHint(JGetString("TrashButtonHint::SyGTreeDir"));

	// widgets

	if (input != nullptr && input->good() && vers <= kSyGCurrentDirSetupVersion)
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
	assert( itsTreeSet != nullptr );
	// itsTreeSet has already called FitToEnclosure()
	ListenTo(GetTable());

	// menus

	auto* windowsMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( windowsMenu != nullptr );
	menuBar->AppendMenu(windowsMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "SyGTreeDir");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "SyGTreeDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd, jx_help_toc);

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
		(SyGGetApplication())->SetMountPointPrefs(path, JString(s));
		}

	JString prefsFile = JCombinePathAndName(path, kDirPrefsName);
	prefsFile += JGetUserName();

	// don't overwrite newer version of prefs

	std::ifstream input(prefsFile.GetBytes());
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

	std::ofstream output(prefsFile.GetBytes());
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
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandlePrefsMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
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
	else if (index == kChangesCmd)
		{
		(JXGetHelpManager())->ShowChangeLog();
		}
	else if (index == kCreditsCmd)
		{
		(JXGetHelpManager())->ShowCredits();
		}
}
