/******************************************************************************
 MenuDocument.cpp

	BASE CLASS = public JXFileDocument

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#include "MenuDocument.h"
#include "FileHistoryMenu.h"
#include "MenuTable.h"
#include "MDIServer.h"
#include "globals.h"
#include "actionDefs.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXCharInput.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXGetStringDialog.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXMacWinPrefsDialog.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kFileSignature = "jx_menu_editor";
const JFileVersion kCurrentFileVersion = 0;

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    New...            %k Meta-N       %i" kNewMenuAction
	"%l| Open...           %k Meta-O       %i" kOpenMenuAction
	"  | Recent"
	"%l| Save              %k Meta-S       %i" kSaveMenuAction
	"  | Save as...        %k Ctrl-S       %i" kSaveMenuAsAction
	"  | Save a copy as... %k Ctrl-Shift-S %i" kSaveMenuCopyAsAction
	"  | Revert to saved                   %i" kRevertMenusAction
	"  | Save all          %k Meta-Shift-S %i" kSaveAllMenusAction
	"%l| Show in file manager              %i" kShowInFileMgrAction
	"%l| Close             %k Meta-W       %i" kJXCloseWindowAction
	"%l| Quit              %k Meta-Q       %i" kJXQuitAction;

enum
{
	kNewCmd = 1,
	kOpenCmd, kRecentMenuCmd,
	kSaveCmd, kSaveAsCmd, kSaveCopyAsCmd, kRevertCmd, kSaveAllCmd,
	kShowInFileMgrCmd,
	kCloseCmd,
	kQuitCmd
};

// Preferences menu

static const JUtf8Byte* kPrefsMenuStr =
	"    Edit tool bar..."
	"  | File manager..."
	"  | Mac/Win/X emulation..."
	"%l| Save table layout as default";

enum
{
	kEditToolBarCmd = 1,
	kFilePrefsCmd,
	kEditMacWinPrefsCmd,
	kSaveLayoutPrefsCmd
};

/******************************************************************************
 Create (static)

	Create an empty menu.

 *****************************************************************************/

bool
MenuDocument::Create
	(
	MenuDocument** doc
	)
{
	auto* dlog = jnew JXGetStringDialog(
		JGetString("NewMenuNameWindowTitle::MenuDocument"),
		JGetString("EditMenuNamePrompt::MenuDocument"));

	if (dlog->DoDialog())
	{
		*doc = jnew MenuDocument(dlog->GetString(), false);
		(**doc).Activate();
		return true;
	}

	*doc = nullptr;
	return false;
}

/******************************************************************************
 Create (static)

	Load menu from a file.

 *****************************************************************************/

bool
MenuDocument::Create
	(
	const JString&	fullName,
	MenuDocument**	doc
	)
{
	*doc = nullptr;

	JXFileDocument* fdoc;
	if (JXGetDocumentManager()->FileDocumentIsOpen(fullName, &fdoc))
	{
		*doc = dynamic_cast<MenuDocument*>(fdoc);
		assert( *doc != nullptr );

		(**doc).Activate();
		return true;
	}

	std::ifstream input(fullName.GetBytes());
	JFileVersion vers;

	const FileStatus status = DefaultCanReadASCIIFile(input, kFileSignature, kCurrentFileVersion, &vers);
	if (status == kFileReadable)
	{
		*doc = jnew MenuDocument(fullName, true);
		(**doc).ReadFile(input);
		(**doc).Activate();
		return true;
	}
	else if (status == kNeedNewerVersion)
	{
		JGetUserNotification()->ReportError(JGetString("NeedNewerVersion::MenuDocument"));
		return false;
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("UnknownFile::MenuDocument"));
		return false;
	}
}

/******************************************************************************
 Constructor (protected)

 *****************************************************************************/

MenuDocument::MenuDocument
	(
	const JString&	fullName,
	const bool		onDisk
	)
	:
	JXFileDocument(JXGetApplication(), fullName, onDisk, false, ".jxm")
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MenuDocument::~MenuDocument()
{
	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (onDisk)
	{
		GetDocumentManager()->AddToFileHistoryMenu(fullName);
	}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "main_window_icon.xpm"
#include <jx-af/image/jx/jx_file_new.xpm>
#include <jx-af/image/jx/jx_file_open.xpm>
#include <jx-af/image/jx/jx_file_save.xpm>
#include <jx-af/image/jx/jx_file_revert_to_saved.xpm>
#include <jx-af/image/jx/jx_file_save_all.xpm>
#include <jx-af/image/jx/jx_edit_undo.xpm>
#include <jx-af/image/jx/jx_edit_redo.xpm>
#include <jx-af/image/jx/jx_edit_cut.xpm>
#include <jx-af/image/jx/jx_edit_copy.xpm>
#include <jx-af/image/jx/jx_edit_paste.xpm>
#include <jx-af/image/jx/jx_edit_clear.xpm>

void
MenuDocument::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,300, JString::empty);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kMenuDocToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,270);
	assert( itsToolBar != nullptr );

// end JXLayout

	JXWidget* toolBarEnclosure = itsToolBar->GetWidgetEnclosure();

// begin EditorLayout

	const JRect EditorLayout_Aperture = toolBarEnclosure->GetAperture();
	toolBarEnclosure->AdjustSize(500 - EditorLayout_Aperture.width(), 300 - EditorLayout_Aperture.height());

	auto* menuTitleLabel =
		jnew JXStaticText(JGetString("menuTitleLabel::MenuDocument::EditorLayout"), toolBarEnclosure,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,10, 70,20);
	assert( menuTitleLabel != nullptr );
	menuTitleLabel->SetToLabel();

	itsMenuTitleInput =
		jnew JXInputField(toolBarEnclosure,
					JXWidget::kHElastic, JXWidget::kFixedTop, 70,10, 240,20);
	assert( itsMenuTitleInput != nullptr );

	auto* windowsAltLabel =
		jnew JXStaticText(JGetString("windowsAltLabel::MenuDocument::EditorLayout"), toolBarEnclosure,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 320,10, 130,20);
	assert( windowsAltLabel != nullptr );
	windowsAltLabel->SetToLabel();

	itsWindowsKeyInput =
		jnew JXCharInput(toolBarEnclosure,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 450,10, 50,20);
	assert( itsWindowsKeyInput != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(toolBarEnclosure,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,40, 500,260);
	assert( scrollbarSet != nullptr );

	toolBarEnclosure->SetSize(EditorLayout_Aperture.width(), EditorLayout_Aperture.height());

// end EditorLayout

	AdjustWindowTitle();
	window->SetCloseAction(JXWindow::kCloseDirector);
	window->SetWMClass(GetWMClassInstance(), GetMenuDocumentClass());
	window->SetMinSize(200, 100);

	JXImage* image = jnew JXImage(GetDisplay(), main_window_icon);
	window->SetIcon(image);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "MenuDocument");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&MenuDocument::UpdateFileMenu,
		&MenuDocument::HandleFileMenu);

	itsFileMenu->SetItemImage(kNewCmd,     jx_file_new);
	itsFileMenu->SetItemImage(kOpenCmd,    jx_file_open);
	itsFileMenu->SetItemImage(kSaveCmd,    jx_file_save);
	itsFileMenu->SetItemImage(kRevertCmd,  jx_file_revert_to_saved);
	itsFileMenu->SetItemImage(kSaveAllCmd, jx_file_save_all);

	jnew FileHistoryMenu(itsFileMenu, kRecentMenuCmd, menuBar);

	JXTextMenu* editMenu = itsMenuTitleInput->AppendEditMenu(menuBar);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "MenuDocument");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandler(this, &MenuDocument::HandlePrefsMenu);

	JXTextMenu* helpMenu = GetApplication()->CreateHelpMenu(menuBar, "MenuDocument", "MainHelp");

	// table

	const JCoordinate kHeaderHeight = 20;

	itsTable =
		jnew MenuTable(menuBar, itsMenuTitleInput, editMenu,
					   scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	itsTable->FitToEnclosure();
	itsTable->Move(0, kHeaderHeight);
	itsTable->AdjustSize(0, -kHeaderHeight);

	auto* colHeader =
		jnew JXColHeaderWidget(itsTable, scrollbarSet,
							   scrollbarSet->GetScrollEnclosure(),
							   JXWidget::kHElastic, JXWidget::kFixedTop,
							   0,0, 10,kHeaderHeight);
	colHeader->FitToEnclosure(true, false);
	itsTable->SetColTitles(colHeader);
	colHeader->TurnOnColResizing(20);

	// must be done after creating widgets

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
	{
		JIndex undo, redo, cut, copy, paste;
		bool ok = itsMenuTitleInput->EditMenuCmdToIndex(JTextEditor::kUndoCmd, &undo);
		assert( ok );
		ok = itsMenuTitleInput->EditMenuCmdToIndex(JTextEditor::kRedoCmd, &redo);
		assert( ok );
		ok = itsMenuTitleInput->EditMenuCmdToIndex(JTextEditor::kCutCmd, &cut);
		assert( ok );
		ok = itsMenuTitleInput->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &copy);
		assert( ok );
		ok = itsMenuTitleInput->EditMenuCmdToIndex(JTextEditor::kPasteCmd, &paste);
		assert( ok );

		itsToolBar->AppendButton(itsFileMenu, kNewCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kSaveCmd);
		itsToolBar->AppendButton(itsFileMenu, kSaveAllCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(editMenu, undo);
		itsToolBar->AppendButton(editMenu, redo);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(editMenu, cut);
		itsToolBar->AppendButton(editMenu, copy);
		itsToolBar->AppendButton(editMenu, paste);

		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, helpMenu);
	}
}

/******************************************************************************
 GetName (virtual)

	Override of JXDocument::GetName().

 ******************************************************************************/

const JString&
MenuDocument::GetName()
	const
{
	JString root, suffix;
	JSplitRootAndSuffix(GetFileName(), &root, &suffix);
	itsDocName = root;

	return itsDocName;
}

/******************************************************************************
 ReadFile (private)

 ******************************************************************************/

void
MenuDocument::ReadFile
	(
	std::istream& input
	)
{
	input.ignore(strlen(kFileSignature));

	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentFileVersion );

	JString title;
	JUtf8Character key;
	input >> title >> std::ws >> key;

	itsMenuTitleInput->GetText()->SetText(title);
	itsWindowsKeyInput->SetCharacter(key);

	itsTable->ReadGeometry(input);
	itsTable->ReadMenuItems(input);
}

/******************************************************************************
 WriteTextFile (virtual protected)

	This must be overridden if WriteFile() is not overridden.

 ******************************************************************************/

void
MenuDocument::WriteTextFile
	(
	std::ostream&	output,
	const bool		safetySave
	)
	const
{
	output << kFileSignature << ' ' << kCurrentFileVersion << std::endl;

	output << itsMenuTitleInput->GetText()->GetText() << std::endl;
	output << itsWindowsKeyInput->GetCharacter() << std::endl;

	itsTable->WriteGeometry(output);
	output << std::endl;

	itsTable->WriteMenuItems(output);
}

/******************************************************************************
 DiscardChanges (virtual protected)

 ******************************************************************************/

void
MenuDocument::DiscardChanges()
{
	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	assert( onDisk );

	std::ifstream input(fullName.GetBytes());
	JFileVersion vers;
	const FileStatus status = DefaultCanReadASCIIFile(input, kFileSignature, kCurrentFileVersion, &vers);
	if (status == kFileReadable)
	{
		ReadFile(input);
		DataReverted();
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("UnknownFile::MenuDocument"));
	}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
MenuDocument::UpdateFileMenu()
{
	itsFileMenu->SetItemEnabled(kSaveCmd, NeedsSave());
	itsFileMenu->SetItemEnabled(kRevertCmd, CanRevert());
	itsFileMenu->SetItemEnabled(kSaveAllCmd, JXGetDocumentManager()->DocumentsNeedSave());

	itsFileMenu->SetItemEnabled(kShowInFileMgrCmd, ExistsOnDisk());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
MenuDocument::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kNewCmd)
	{
		MenuDocument* doc;
		Create(&doc);
	}
	else if (index == kOpenCmd)
	{
		MDIServer::ChooseFiles();
	}

	else if (index == kSaveCmd)
	{
		SaveInCurrentFile();
	}
	else if (index == kSaveAsCmd)
	{
		bool onDisk;
		const JString fullName = GetFullName(&onDisk);
		if (onDisk)
		{
			GetDocumentManager()->AddToFileHistoryMenu(fullName);
		}

		SaveInNewFile();
	}
	else if (index == kSaveCopyAsCmd)
	{
		JString fullName;
		if (SaveCopyInNewFile(JString::empty, &fullName))
		{
			GetDocumentManager()->AddToFileHistoryMenu(fullName);
		}
	}
	else if (index == kRevertCmd)
	{
		RevertToSaved();
	}
	else if (index == kSaveAllCmd)
	{
		JXGetDocumentManager()->SaveAllFileDocuments(true);
	}

	else if (index == kShowInFileMgrCmd)
	{
		bool onDisk;
		const JString fullName = GetFullName(&onDisk);
		assert( onDisk );
		JXGetWebBrowser()->ShowFileLocation(fullName);
	}

	else if (index == kCloseCmd)
	{
		Close();
	}

	else if (index == kQuitCmd)
	{
		GetApplication()->Quit();
	}
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
MenuDocument::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kEditToolBarCmd)
	{
		itsToolBar->Edit();
	}
	else if (index == kFilePrefsCmd)
	{
		JXGetWebBrowser()->EditPrefs();
	}
	else if (index == kEditMacWinPrefsCmd)
	{
		JXMacWinPrefsDialog::EditPrefs();
	}

	else if (index == kSaveLayoutPrefsCmd)
	{
		itsTable->JPrefObject::WritePrefs();
	}
}
