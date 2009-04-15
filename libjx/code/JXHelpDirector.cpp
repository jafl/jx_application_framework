/******************************************************************************
 JXHelpDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXHelpDirector.h>
#include <JXWebBrowser.h>
#include <JXWindow.h>
#include <JXHelpText.h>
#include <JXSearchTextDialog.h>
#include <JXScrollbarSet.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JXImageButton.h>
#include <jXGlobals.h>
#include <JExtractHTMLTitle.h>
#include <JString.h>
#include <jAssert.h>

// File menu

static const JCharacter* kFileMenuTitleStr    = "File";
static const JCharacter* kFileMenuShortcutStr = "#F";

static const JCharacter* kMacFileMenuStr =
	"    Table of Contents"
	"%l| Save text...     %k Meta-S"
	"%l| Page setup..."
	"  | Print...         %k Meta-P"
	"%l| Close            %k Meta-W"
	"  | Close all        %k Meta-Shift-W";

static const JCharacter* kWinFileMenuStr =
	"    Table of Contents"
	"%l| Save text...     %h s %k Ctrl-S"
	"%l| Page setup..."
	"  | Print...         %h p %k Ctrl-P"
	"%l| Close            %h c %k Ctrl-W"
	"  | Close all        %h a %k Ctrl-Shift-W";

enum
{
	kShowTOCCmd = 1,
	kSaveTextCmd,
	kPageSetupCmd, kPrintCmd,
	kCloseCmd, kCloseAllCmd
};

// Search menu additions

static const JCharacter* kSearchAllStr    = "Search all sections";
static const JCharacter* kSearchAllAction = "SearchAllSections::JXHelpDirector";

// Preferences menu

static const JCharacter* kPrefsMenuTitleStr    = "Preferences";
static const JCharacter* kPrefsMenuShortcutStr = "#P";

static const JCharacter* kMacPrefsMenuStr =
	"    Web browser preferences..."
	"  | Save window size as default";

static const JCharacter* kWinPrefsMenuStr =
	"    Web browser preferences..."
	"  | Save window size as default";

enum
{
	kEditWWWPrefsCmd = 1,
	kSaveWindSizeCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

JXHelpDirector::JXHelpDirector
	(
	const JCharacter*	text,
	JXPSPrinter*		printer
	)
	:
	JXWindowDirector(JXGetHelpManager())
{
	BuildWindow(text, printer);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXHelpDirector::~JXHelpDirector()
{
}

/******************************************************************************
 ShowSubsection

 ******************************************************************************/

void
JXHelpDirector::ShowSubsection
	(
	const JCharacter* name
	)
{
	itsText->ShowSubsection(name);
}

/******************************************************************************
 Search

	Searches from the top using the current Search Text contents.

 ******************************************************************************/

JBoolean
JXHelpDirector::Search()
	const
{
	itsText->SetCaretLocation(1);
	return itsText->SearchForward(kJFalse);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include <jx_help_toc.xpm>
#include <jx_file_save.xpm>
#include <jx_file_print.xpm>

void
JXHelpDirector::BuildWindow
	(
	const JCharacter*	text,
	JXPSPrinter*		printer
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 600,550, "");
    assert( window != NULL );
    SetWindow(window);

    JXScrollbarSet* scrollbarSet =
        new JXScrollbarSet(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 600,520);
    assert( scrollbarSet != NULL );

    JXMenuBar* menuBar =
        new JXMenuBar(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
    assert( menuBar != NULL );

    itsCloseButton =
        new JXTextButton(JGetString("itsCloseButton::JXHelpDirector::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 550,0, 50,30);
    assert( itsCloseButton != NULL );
    itsCloseButton->SetShortcuts(JGetString("itsCloseButton::JXHelpDirector::shortcuts::JXLayout"));

    itsTOCButton =
        new JXImageButton(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 500,0, 50,30);
    assert( itsTOCButton != NULL );

// end JXLayout

	window->SetMinSize(100,100);
	window->SetWMClass("Help", "JX_Help_Browser");

	JExtractHTMLTitle extractor;
	JString title;
	if (extractor.ExtractTitle(text, &title))
		{
		window->SetTitle(title);
		}
	else
		{
		title.Clear();
		window->SetTitle("Help");
		}

	ListenTo(itsCloseButton);

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		itsFileMenu->SetMenuItems(kMacFileMenuStr);
		}
	else
		{
		itsFileMenu->SetShortcuts(kFileMenuShortcutStr);
		itsFileMenu->SetMenuItems(kWinFileMenuStr);
		}
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kShowTOCCmd,  jx_help_toc);
	itsFileMenu->SetItemImage(kSaveTextCmd, jx_file_save);
	itsFileMenu->SetItemImage(kPrintCmd,    jx_file_print);

	itsText =
		new JXHelpText(title, text, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsText != NULL );
	itsText->FitToEnclosure();
	itsText->ShouldAllowDragAndDrop(kJTrue);
	itsText->UpdateScrollbars();
	itsText->SetPSPrinter(printer);

	// create menus

	itsText->AppendEditMenu(menuBar);	// for Copy, Select All

	JXTextMenu* searchMenu = itsText->AppendSearchMenu(menuBar);
	itsSearchAllCmdIndex   = searchMenu->GetItemCount() + 1;
	searchMenu->ShowSeparatorAfter(itsSearchAllCmdIndex-1);
	searchMenu->AppendItem(kSearchAllStr, kJFalse, kJFalse,
						   NULL, NULL, kSearchAllAction);
	ListenTo(searchMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		itsPrefsMenu->SetMenuItems(kMacPrefsMenuStr);
		}
	else
		{
		itsPrefsMenu->SetShortcuts(kPrefsMenuShortcutStr);
		itsPrefsMenu->SetMenuItems(kWinPrefsMenuStr);
		}
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	if ((JXGetHelpManager())->HasTOC())
		{
		itsTOCButton->SetImage(jx_help_toc);
		ListenTo(itsTOCButton);
		}
	else
		{
		itsTOCButton->Hide();
		menuBar->AdjustSize(itsTOCButton->GetFrameWidth(), 0);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXHelpDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXTextMenu* searchMenu;
	const JBoolean ok = itsText->GetSearchMenu(&searchMenu);
	assert( ok );

	if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == searchMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateCustomSearchMenuItems();
		}
	else if (sender == searchMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleCustomSearchMenuItems(selection->GetIndex());
		}

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrefsMenu();
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandlePrefsMenu(selection->GetIndex());
		}

	else if (sender == itsTOCButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowTOC();
		}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		Close();
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
JXHelpDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kShowTOCCmd, (JXGetHelpManager())->HasTOC());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
JXHelpDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kShowTOCCmd)
		{
		(JXGetHelpManager())->ShowTOC();
		}

	else if (index == kSaveTextCmd)
		{
		JChooseSaveFile* csf    = JGetChooseSaveFile();
		const JString& origName = (GetWindow())->GetTitle();
		JString fileName;
		if (csf->SaveFile("Save text as:", NULL, origName, &fileName))
			{
			itsText->WritePlainText(fileName, JTextEditor::kUNIXText);
			}
		}

	else if (index == kPageSetupCmd)
		{
		itsText->HandlePSPageSetup();
		}
	else if (index == kPrintCmd)
		{
		itsText->PrintPS();
		}

	else if (index == kEditWWWPrefsCmd)
		{
		(JXGetWebBrowser())->EditPrefs();
		}
	else if (index == kSaveWindSizeCmd)
		{
		(JXGetHelpManager())->SaveWindowPrefs(GetWindow());
		}

	else if (index == kCloseCmd)
		{
		Close();
		}
	else if (index == kCloseAllCmd)
		{
		(JXGetHelpManager())->CloseAll();
		}
}

/******************************************************************************
 UpdateCustomSearchMenuItems (private)

 ******************************************************************************/

void
JXHelpDirector::UpdateCustomSearchMenuItems()
{
	JXTextMenu* searchMenu;
	const JBoolean ok = itsText->GetSearchMenu(&searchMenu);
	assert( ok );

	if ((JXGetSearchTextDialog())->HasSearchText())
		{
		searchMenu->EnableItem(itsSearchAllCmdIndex);
		}
}

/******************************************************************************
 HandleCustomSearchMenuItems (private)

 ******************************************************************************/

void
JXHelpDirector::HandleCustomSearchMenuItems
	(
	const JIndex index
	)
{
	if (index == itsSearchAllCmdIndex)
		{
		(JXGetHelpManager())->SearchAllSections();
		}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
JXHelpDirector::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
JXHelpDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kEditWWWPrefsCmd)
		{
		(JXGetWebBrowser())->EditPrefs();
		}
	else if (index == kSaveWindSizeCmd)
		{
		(JXGetHelpManager())->SaveWindowPrefs(GetWindow());
		}
}
