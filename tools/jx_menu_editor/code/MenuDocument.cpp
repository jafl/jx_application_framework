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
#include "fileVersions.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXDocumentMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXCharInput.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXGetStringDialog.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXMacWinPrefsDialog.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kFileSignature = "jx_menu_editor";

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
	*doc = jnew MenuDocument(GetDocumentManager()->GetNewFileName(), false);
	(**doc).Activate();
	return true;
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

	const FileStatus status = DefaultCanReadASCIIFile(input, kFileSignature, kCurrentMenuFileVersion, &vers);
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
#include "MenuDocument-File.h"
#include "MenuDocument-Preferences.h"

void
MenuDocument::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 670,300, JString::empty);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 670,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kMenuDocToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 670,270);
	assert( itsToolBar != nullptr );

// end JXLayout

	JXWidget* toolBarEnclosure = itsToolBar->GetWidgetEnclosure();

// begin EditorLayout

	const JRect EditorLayout_Aperture = toolBarEnclosure->GetAperture();
	toolBarEnclosure->AdjustSize(600 - EditorLayout_Aperture.width(), 300 - EditorLayout_Aperture.height());

	itsClassNameInput =
		jnew JXInputField(toolBarEnclosure,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,10, 140,20);
	assert( itsClassNameInput != nullptr );

	auto* menuTitleLabel =
		jnew JXStaticText(JGetString("menuTitleLabel::MenuDocument::EditorLayout"), toolBarEnclosure,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,10, 70,20);
	assert( menuTitleLabel != nullptr );
	menuTitleLabel->SetToLabel();

	itsMenuTitleInput =
		jnew JXInputField(toolBarEnclosure,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 300,10, 130,20);
	assert( itsMenuTitleInput != nullptr );

	auto* windowsAltLabel =
		jnew JXStaticText(JGetString("windowsAltLabel::MenuDocument::EditorLayout"), toolBarEnclosure,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 440,10, 130,20);
	assert( windowsAltLabel != nullptr );
	windowsAltLabel->SetToLabel();

	itsWindowsKeyInput =
		jnew JXCharInput(toolBarEnclosure,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 570,10, 30,20);
	assert( itsWindowsKeyInput != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(toolBarEnclosure,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,40, 600,260);
	assert( scrollbarSet != nullptr );

	auto* classNameLabel =
		jnew JXStaticText(JGetString("classNameLabel::MenuDocument::EditorLayout"), toolBarEnclosure,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,10, 80,20);
	assert( classNameLabel != nullptr );
	classNameLabel->SetToLabel();

	toolBarEnclosure->SetSize(EditorLayout_Aperture.width(), EditorLayout_Aperture.height());

// end EditorLayout

	AdjustWindowTitle();
	window->SetCloseAction(JXWindow::kCloseDirector);
	window->SetWMClass(GetWMClassInstance(), GetMenuDocumentClass());
	window->SetMinSize(200, 100);

	JXImage* image = jnew JXImage(GetDisplay(), main_window_icon);
	window->SetIcon(image);

	itsClassNameInput->SetIsRequired();
	itsMenuTitleInput->SetIsRequired();

	ListenTo(itsClassNameInput->GetText(), std::function([this](const JStyledText::TextChanged&)
	{
		UpdateUnsavedFileName();
		DataModified();
	}));

	ListenTo(itsMenuTitleInput->GetText(), std::function([this](const JStyledText::TextChanged&)
	{
		UpdateUnsavedFileName();
		DataModified();
	}));

	ListenTo(itsWindowsKeyInput->GetText(), std::function([this](const JStyledText::TextChanged&)
	{
		DataModified();
	}));

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("MenuTitle::MenuDocument_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&MenuDocument::UpdateFileMenu,
		&MenuDocument::HandleFileMenu);
	SetFileMenuIcons(itsFileMenu);

	jnew FileHistoryMenu(itsFileMenu, kRecentMenuCmd, menuBar);

	JXTextMenu* editMenu = itsClassNameInput->AppendEditMenu(menuBar);
	itsMenuTitleInput->ShareEditMenu(editMenu);
	itsWindowsKeyInput->ShareEditMenu(editMenu);

	menuBar->AppendMenu(
		jnew JXDocumentMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
							JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 10,10));

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::MenuDocument_Preferences"));
	itsPrefsMenu->SetMenuItems(kPreferencesMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandler(this, &MenuDocument::HandlePrefsMenu);

	JXTextMenu* helpMenu = GetApplication()->CreateHelpMenu(menuBar, "MainHelp");

	// table

	const JCoordinate kHeaderHeight = 20;

	itsTable =
		jnew MenuTable(this, menuBar, itsMenuTitleInput, editMenu,
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
		itsToolBar->AppendButton(itsFileMenu, kNewCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kSaveCmd);
		itsToolBar->AppendButton(itsFileMenu, kSaveAllCmd);

		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, helpMenu);
	}
}

/******************************************************************************
 UpdateUnsavedFileName (private)

 ******************************************************************************/

void
MenuDocument::UpdateUnsavedFileName()
{
	if (!ExistsOnDisk())
	{
		FileChanged(
			itsClassNameInput->GetText()->GetText() + "-" + itsMenuTitleInput->GetText()->GetText(),
			false);
	}
}

/******************************************************************************
 GetName (virtual)

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
	assert( vers <= kCurrentMenuFileVersion );

	JCoordinate w,h;
	input >> w >> h;

	GetWindow()->SetSize(w, h);

	JString className, title;
	input >> className >> title;

	JUtf8Character key;
	JReadUntil(input, kCharacterMarker);
	input >> key;

	itsClassNameInput->GetText()->SetText(className);
	itsMenuTitleInput->GetText()->SetText(title);
	itsWindowsKeyInput->SetCharacter(key);

	itsTable->ReadGeometry(input);
	itsTable->ReadMenuItems(input);
}

/******************************************************************************
 WriteFile (virtual protected)

 ******************************************************************************/

JError
MenuDocument::WriteFile
	(
	const JString&	fullName,
	const bool		safetySave
	)
	const
{
	if (!safetySave)
	{
		itsTable->FillInItemIDs(itsClassNameInput->GetText()->GetText());
	}
	return JXFileDocument::WriteFile(fullName, safetySave);
}

/******************************************************************************
 WriteTextFile (virtual protected)

 ******************************************************************************/

void
MenuDocument::WriteTextFile
	(
	std::ostream&	output,
	const bool		safetySave
	)
	const
{
	output << kFileSignature << ' ' << kCurrentMenuFileVersion << std::endl;

	const JRect boundsG = GetWindow()->GetBoundsGlobal();
	output << boundsG.width() << ' ' << boundsG.height() << std::endl;

	output << itsClassNameInput->GetText()->GetText() << std::endl;
	output << itsMenuTitleInput->GetText()->GetText() << std::endl;
	output << kCharacterMarker << itsWindowsKeyInput->GetCharacter() << std::endl;

	itsTable->WriteGeometry(output);
	output << std::endl;

	itsTable->WriteMenuItems(output);

	if (!safetySave)
	{
		GenerateCode();
	}
}

/******************************************************************************
 GenerateCode (private)

 ******************************************************************************/

static const JRegex invalidCharPattern = "[^_a-zA-Z0-9]+";

void
MenuDocument::GenerateCode()
	const
{
	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	assert( onDisk );

	JString path, name, root, suffix;
	JSplitPathAndName(fullName, &path, &name);
	JSplitRootAndSuffix(name, &root, &suffix);

	JString projRoot;
	if (!FindProjectRoot(path, &projRoot))
	{
		JGetUserNotification()->ReportError(JGetString(""));
		return;
	}

	// header

	JString headerFileName = JCombinePathAndName(path, root);
	headerFileName         = JCombineRootAndSuffix(headerFileName, "h");

	JString tempHeaderFileName;
	const JError err = JCreateTempFile(&path, nullptr, &tempHeaderFileName);
	if (!err.OK())
	{
		const JUtf8Byte* map[] =
		{
			"path", path.GetBytes(),
			"err",  err.GetMessage().GetBytes()
		};
		JGetUserNotification()->ReportError(JGetString("CodeDirectoryNotWritable::MenuDocument", map, sizeof(map)));
		return;
	}
	std::ofstream headerOutput(tempHeaderFileName.GetBytes());

	name = root;
	JStringIterator nameIter(&name);
	while (nameIter.Next(invalidCharPattern))
	{
		nameIter.ReplaceLastMatch("_");
	}
	nameIter.Invalidate();

	JString title = itsMenuTitleInput->GetText()->GetText();
	JStringIterator titleIter(&title);
	while (titleIter.Next(invalidCharPattern))
	{
		titleIter.RemoveLastMatch();
	}
	titleIter.Invalidate();

	const JUtf8Byte* headerMap[] =
	{
		"name",  name.GetBytes(),
		"title", title.GetBytes()
	};
	JGetString("CodeHeader::MenuDocument", headerMap, sizeof(headerMap)).Print(headerOutput);

	itsTable->GenerateCode(headerOutput, itsClassNameInput->GetText()->GetText(), title);

	JGetString("CodeFooter::MenuDocument").Print(headerOutput);
	headerOutput.close();

	// check if header file actually changed

	JString origHeaderText, newHeaderText;
	JReadFile(headerFileName, &origHeaderText);
	JReadFile(tempHeaderFileName, &newHeaderText);
	if (newHeaderText != origHeaderText)
	{
		JEditVCS(headerFileName);
		JRenameFile(tempHeaderFileName, headerFileName, true);
	}
	else
	{
		JRemoveFile(tempHeaderFileName);
	}

	// strings

	JString stringsFile = JCombinePathAndName(projRoot, JString("strings", JString::kNoCopy));
	stringsFile         = JCombinePathAndName(stringsFile, root);
	std::ofstream stringsOutput(stringsFile.GetBytes());

	const JUtf8Byte* stringsMap[] =
	{
		"name", name.GetBytes()
	};
	JGetString("StringsHeader::MenuDocument", stringsMap, sizeof(stringsMap)).Print(stringsOutput);

	stringsOutput << itsMenuTitleInput->GetText()->GetText() << std::endl << std::endl;

	itsTable->GenerateStrings(stringsOutput, itsClassNameInput->GetText()->GetText());
}

/******************************************************************************
 FindProjectRoot (static)

	Search directory tree up to root.

 ******************************************************************************/

bool
MenuDocument::FindProjectRoot
	(
	const JString&	path,
	JString*		root
	)
{
	JString p = path, n;
	do
	{
		n = JCombinePathAndName(p, JString("Makefile", JString::kNoCopy));
		if (JFileExists(n))
		{
			*root = p;
			return true;
		}

		n = JCombinePathAndName(p, JString("Make.header", JString::kNoCopy));
		if (JFileExists(n))
		{
			*root = p;
			return true;
		}

		n = JCombinePathAndName(p, JString("CMakeLists.txt", JString::kNoCopy));
		if (JFileExists(n))
		{
			*root = p;
			return true;
		}

		JSplitPathAndName(p, &p, &n);
	}
	while (!JIsRootDirectory(p));

	root->Clear();
	return false;
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
	const FileStatus status = DefaultCanReadASCIIFile(input, kFileSignature, kCurrentMenuFileVersion, &vers);
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
		if (itsTable->EndEditing())
		{
			SaveInCurrentFile();
		}
	}
	else if (index == kSaveAsCmd)
	{
		bool onDisk;
		const JString fullName = GetFullName(&onDisk);
		if (onDisk)
		{
			GetDocumentManager()->AddToFileHistoryMenu(fullName);
		}

		if (itsTable->EndEditing())
		{
			SaveInNewFile();
		}
	}
	else if (index == kSaveCopyAsCmd)
	{
		JString fullName;
		if (itsTable->EndEditing() && SaveCopyInNewFile(JString::empty, &fullName))
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
