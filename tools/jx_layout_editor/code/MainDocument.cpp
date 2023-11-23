/******************************************************************************
 MainDocument.cpp

	BASE CLASS = public JXFileDocument

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#include "MainDocument.h"
#include "LayoutList.h"
#include "LayoutDirector.h"
#include "globals.h"
#include "actionDefs.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXCloseDirectorTask.h>
#include <jx-af/jx/JXGetStringDialog.h>
#include <jx-af/jx/JXMacWinPrefsDialog.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kFileSignature = "jx_layout_editor_data";
const JFileVersion kCurrentFileVersion = 0;

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    Save            %k Meta-S %i" kSaveLayoutAction
	"  | Save as...      %k Ctrl-S %i" kSaveLayoutAsAction
	"  | Revert to saved           %i" kRevertLayoutsAction
	"%l| Quit            %k Meta-Q %i" kJXQuitAction;

enum
{
	kSaveCmd = 1,
	kSaveAsCmd,
	kRevertCmd,
	kQuitCmd
};

// Layout menu

static const JUtf8Byte* kLayoutMenuStr =
	"    New layout...             %k Meta-N %i" kNewLayoutAction
	"%l| Rename selected layout...           %i" kEditLayoutNameAction;

enum
{
	kNewLayoutCmd = 1,
	kRenameSelectedLayoutCmd
};

// Preferences menu

static const JUtf8Byte* kPrefsMenuStr =
	"    Edit tool bar..."
	"  | Mac/Win/X emulation...";

enum
{
	kEditToolBarCmd = 1,
	kEditMacWinPrefsCmd
};

/******************************************************************************
 Create (static)

 *****************************************************************************/

bool
MainDocument::Create
	(
	const JString&	fullName,
	MainDocument**	doc
	)
{
	*doc = nullptr;

	if (!JFileExists(fullName))
	{
		*doc = jnew MainDocument(fullName, false);
		return true;
	}

	std::ifstream input(fullName.GetBytes());
	JFileVersion vers;

	FileStatus status =DefaultCanReadASCIIFile(input, "Magic:", 15000, &vers);
	if (status == kFileReadable)
	{
		JString path, name, root, suffix;
		JSplitPathAndName(fullName, &path, &name);
		JSplitRootAndSuffix(name, &root, &suffix);

		*doc = jnew MainDocument(root, false);
		(**doc).ImportFDesignFile(input);
		return true;
	}
	else if (status == kNeedNewerVersion)
	{
		const JString v((JUInt64) vers);
		const JUtf8Byte* map[] =
		{
			"v", v.GetBytes()
		};
		const JString msg = JGetString("UnsupportedFDesignVersion::MainDocument", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		return false;
	}

	status = DefaultCanReadASCIIFile(input, kFileSignature, kCurrentFileVersion, &vers);
	if (status == kFileReadable)
	{
		*doc = jnew MainDocument(fullName, true);
		(**doc).ReadFile(input);
		return true;
	}
	else if (status == kNeedNewerVersion)
	{
		JGetUserNotification()->ReportError(JGetString("NeedNewerVersion::MainDocument"));
		return false;
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("UnknownFile::MainDocument"));
		return false;
	}
}

/******************************************************************************
 Constructor (protected)

 *****************************************************************************/

MainDocument::MainDocument
	(
	const JString&	fullName,
	const bool		onDisk
	)
	:
	JXFileDocument(JXGetApplication(), fullName, onDisk, false, ".jxl")
{
	itsLayouts    = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
	itsLayoutDirs = jnew JStringPtrMap<LayoutDirector>(JPtrArrayT::kForgetAll);

	itsLayoutNames = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsLayoutNames->SetCompareFunction(JCompareStringsCaseInsensitive);

	BuildWindow();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MainDocument::~MainDocument()
{
	jdelete itsLayouts;
	jdelete itsLayoutNames;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "main_window_icon.xpm"
#include <jx-af/image/jx/jx_file_save.xpm>
#include <jx-af/image/jx/jx_file_revert_to_saved.xpm>

void
MainDocument::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,300, JString::empty);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kMainDocToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,270);
	assert( itsToolBar != nullptr );

// end JXLayout

	AdjustWindowTitle();
	window->SetCloseAction(JXWindow::kQuitApp);
	window->SetWMClass(GetWMClassInstance(), GetMainDocumentClass());
	window->SetMinSize(200, 200);

	JXImage* image = jnew JXImage(GetDisplay(), main_window_icon);
	window->SetIcon(image);

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic,JXWidget::kVElastic,
						   0,0, 100,100);
	scrollbarSet->FitToEnclosure();

	itsLayoutNameTable =
		jnew LayoutList(this, menuBar, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic,
						0,0, 100,100);
	itsLayoutNameTable->FitToEnclosure();
	itsLayoutNameTable->SetStringList(itsLayoutNames);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "MainDocument");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&MainDocument::UpdateFileMenu,
		&MainDocument::HandleFileMenu);

	itsFileMenu->SetItemImage(kSaveCmd,   jx_file_save);
	itsFileMenu->SetItemImage(kRevertCmd, jx_file_revert_to_saved);

	itsLayoutMenu = menuBar->AppendTextMenu(JGetString("LayoutMenuTitle::MainDocument"));
	itsLayoutMenu->SetMenuItems(kLayoutMenuStr, "MainDocument");
	itsLayoutMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsLayoutMenu->AttachHandlers(this,
		&MainDocument::UpdateLayoutMenu,
		&MainDocument::HandleLayoutMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "MainDocument");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandler(this, &MainDocument::HandlePrefsMenu);

	JXTextMenu* helpMenu = GetApplication()->CreateHelpMenu(menuBar, "MainDocument", "MainHelp");

	// must be done after creating widgets

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kSaveCmd);
		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, helpMenu);
	}
}

/******************************************************************************
 GetName (virtual)

	Override of JXDocument::GetName().

 ******************************************************************************/

const JString&
MainDocument::GetName()
	const
{
	JString root, suffix;
	JSplitRootAndSuffix(GetFileName(), &root, &suffix);
	itsDocName = root;

	return itsDocName;
}

/******************************************************************************
 OpenLayout

 ******************************************************************************/

void
MainDocument::OpenLayout
	(
	const JString& name
	)
{
	LayoutDirector* dir;
	JString* data;
	if (itsLayoutDirs->GetItem(name, &dir))
	{
		dir->Activate();
	}
	else if (itsLayouts->GetItem(name, &data))
	{
		std::istringstream input(data->GetBytes());
		auto* dir = jnew LayoutDirector(this, name, input);

		const bool ok = itsLayoutDirs->SetNewItem(name, dir);
		assert( ok );

		dir->Activate();
	}
}

/******************************************************************************
 SaveLayout

 ******************************************************************************/

void
MainDocument::SaveLayout
	(
	LayoutDirector* dir
	)
{
	std::ostringstream data;
	dir->WriteLayout(data);
	itsLayouts->SetItem(dir->GetLayoutName(), JString(data.str()), JPtrArrayT::kDelete);
}

/******************************************************************************
 EditLayoutName

 ******************************************************************************/

void
MainDocument::EditLayoutName
	(
	const JString& oldName
	)
{
	auto* dlog = jnew JXGetStringDialog(
		JGetString("EditLayoutNameWindowTitle::LayoutDirector"),
		JGetString("EditLayoutNamePrompt::LayoutDirector"),
		oldName);

	if (dlog->DoDialog())
	{
		const JString& newName = dlog->GetString();

		JString* data;
		bool ok = itsLayouts->GetItem(oldName, &data);
		assert( ok );
		ok = itsLayouts->SetNewItem(newName, data);
		if (ok)
		{
			itsLayouts->RemoveItem(oldName);

			LayoutDirector* dir;
			ok = itsLayoutDirs->GetItem(oldName, &dir);
			assert( ok );
			ok = itsLayoutDirs->SetNewItem(newName, dir);
			assert( ok );
			itsLayoutDirs->RemoveItem(oldName);

			JIndex i;
			ok = itsLayoutNames->SearchSorted(const_cast<JString*>(&oldName), JListT::kAnyMatch, &i);
			assert( ok );
			itsLayoutNames->DeleteItem(i);
			ok = itsLayoutNames->InsertSorted(jnew JString(newName), false, &i);
			assert( ok );

			dir->SetLayoutName(newName);
			DataModified();
		}
		else
		{
			JGetUserNotification()->ReportError(JGetString("DuplicateLayoutName::MainDocument"));
		}
	}
}

/*****************************************************************************
 DirectorClosed (virtual protected)

 ******************************************************************************/

void
MainDocument::DirectorClosed
	(
	JXDirector* dir
	)
{
	LayoutDirector* layoutDir = dynamic_cast<LayoutDirector*>(dir);
	if (layoutDir != nullptr)
	{
		itsLayoutDirs->RemoveItem(layoutDir->GetLayoutName());
	}

	JXWindowDirector::DirectorClosed(dir);
}

/******************************************************************************
 ReadFile (private)

 ******************************************************************************/

void
MainDocument::ReadFile
	(
	std::istream& input
	)
{
	input.ignore(strlen(kFileSignature));

	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentFileVersion );

	itsLayouts->CleanOut();
	while (true)
	{
		bool keepGoing;
		input >> keepGoing;
		if (!keepGoing)
		{
			break;
		}

		JString name, data;
		input >> name >> data;
		itsLayouts->SetNewItem(name, data);

		auto* n = jnew JString(name);
		JIndex i;
		const bool ok = itsLayoutNames->InsertSorted(n, false, &i);
		assert( ok );
	}
}

/******************************************************************************
 WriteTextFile (virtual protected)

	This must be overridden if WriteFile() is not overridden.

 ******************************************************************************/

void
MainDocument::WriteTextFile
	(
	std::ostream&	output,
	const bool		safetySave
	)
	const
{
	output << kFileSignature << ' ' << kCurrentFileVersion << std::endl;

	JStringPtrMapCursor<JString> cursor(itsLayouts);
	while (cursor.Next())
	{
		output << true << std::endl;
		output << cursor.GetKey() << std::endl;
		output << *cursor.GetValue() << std::endl;
	}

	output << false << std::endl;
}

/******************************************************************************
 ImportFDesignFile (private)

 ******************************************************************************/

void
MainDocument::ImportFDesignFile
	(
	std::istream& input
	)
{
	while (!input.eof() && !input.fail())
	{
		auto* dir = jnew LayoutDirector(this, JString::empty);
		if (dir->ImportFDesignLayout(input))
		{
			SaveLayout(dir);

			auto* n = jnew JString(dir->GetLayoutName());
			JIndex i;
			itsLayoutNames->InsertSorted(n, true, &i);
		}

		dir->Close();
	}
}

/******************************************************************************
 DiscardChanges (virtual protected)

 ******************************************************************************/

void
MainDocument::DiscardChanges()
{
	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (onDisk)
	{
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
			JGetUserNotification()->ReportError(JGetString("UnknownFile::MainDocument"));
		}
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("FileMissing::MainDocument"));
	}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
MainDocument::UpdateFileMenu()
{
	itsFileMenu->SetItemEnabled(kSaveCmd, NeedsSave());
	itsFileMenu->SetItemEnabled(kRevertCmd, CanRevert());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
MainDocument::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kSaveCmd)
	{
		SaveInCurrentFile();
	}
	else if (index == kSaveAsCmd)
	{
		SaveInNewFile();
	}
	else if (index == kRevertCmd)
	{
		RevertToSaved();
	}

	if (index == kQuitCmd)
	{
		GetApplication()->Quit();
	}
}

/******************************************************************************
 UpdateLayoutMenu (private)

 ******************************************************************************/

void
MainDocument::UpdateLayoutMenu()
{
	JPoint cell;
	itsLayoutMenu->SetItemEnabled(kRenameSelectedLayoutCmd,
		itsLayoutNameTable->GetTableSelection().GetSingleSelectedCell(&cell));
}

/******************************************************************************
 HandleLayoutMenu (private)

 ******************************************************************************/

void
MainDocument::HandleLayoutMenu
	(
	const JIndex index
	)
{
	if (index == kNewLayoutCmd)
	{
		auto* dlog = jnew JXGetStringDialog(
			JGetString("NewLayoutNameWindowTitle::LayoutDirector"),
			JGetString("EditLayoutNamePrompt::LayoutDirector"));

		if (dlog->DoDialog())
		{
			const JString& name = dlog->GetString();

			auto* dir = jnew LayoutDirector(this, name);

			std::ostringstream data;
			dir->WriteLayout(data);
			bool ok = itsLayouts->SetNewItem(name, JString(data.str()));
			if (ok)
			{
				JIndex i;
				ok = itsLayoutNames->InsertSorted(jnew JString(name), false, &i);
				assert(ok);

				ok = itsLayoutDirs->SetNewItem(name, dir);
				assert( ok );

				dir->Activate();
				DataModified();
			}
			else
			{
				dir->Close();
				JGetUserNotification()->ReportError(JGetString("DuplicateLayoutName::MainDocument"));
			}
		}
	}

	else if (index == kRenameSelectedLayoutCmd)
	{
		JPoint cell;
		bool ok = itsLayoutNameTable->GetTableSelection().GetSingleSelectedCell(&cell);
		assert( ok );

		const JString oldName = *itsLayoutNames->GetItem(cell.y);	// copy to avoid data corruption
		EditLayoutName(oldName);
	}
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
MainDocument::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kEditToolBarCmd)
	{
		itsToolBar->Edit();
	}
	else if (index == kEditMacWinPrefsCmd)
	{
		JXMacWinPrefsDialog::EditPrefs();
	}
}
