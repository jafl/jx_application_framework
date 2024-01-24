/******************************************************************************
 MenuDocument.cpp

	BASE CLASS = public JXFileDocument

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#include "MenuDocument.h"
#include "FileHistoryMenu.h"
#include "MenuTable.h"
#include "MDIServer.h"
#include "ImageCache.h"
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
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXMacWinPrefsDialog.h>
#include <jx-af/jx/JXUrgentFunctionTask.h>
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
	(**doc).DataReverted();
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

		MenuDocument* d = *doc;
		auto* task = jnew JXUrgentFunctionTask(d, std::function([d]()
		{
			d->DataReverted();
		}));
		task->Go();

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

	auto* window = jnew JXWindow(this, 920,300, JString::empty);
	window->SetMinSize(200, 100);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "jx_menu_editor_Layout");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 920,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kMenuDocToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 920,270);

	auto* classNameLabel =
		jnew JXStaticText(JGetString("classNameLabel::MenuDocument::JXLayout"), itsToolBar->GetWidgetEnclosure(),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 80,20);
	classNameLabel->SetToLabel(false);

	auto* menuTitleLabel =
		jnew JXStaticText(JGetString("menuTitleLabel::MenuDocument::JXLayout"), itsToolBar->GetWidgetEnclosure(),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,0, 70,20);
	menuTitleLabel->SetToLabel(false);

	auto* windowsAltLabel =
		jnew JXStaticText(JGetString("windowsAltLabel::MenuDocument::JXLayout"), itsToolBar->GetWidgetEnclosure(),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 440,0, 130,20);
	windowsAltLabel->SetToLabel(false);

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 920,240);
	assert( scrollbarSet != nullptr );

	itsClassNameInput =
		jnew JXInputField(itsToolBar->GetWidgetEnclosure(),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,0, 140,20);

	itsMenuTitleInput =
		jnew JXInputField(itsToolBar->GetWidgetEnclosure(),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 300,0, 130,20);

	itsWindowsKeyInput =
		jnew JXCharInput(itsToolBar->GetWidgetEnclosure(),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 570,0, 30,20);

// end JXLayout

	AdjustWindowTitle();

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

	ListenTo(this, std::function([this](const JXFileDocument::NameChanged&)
	{
		itsTable->RebuildIconMenu();
	}));

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("MenuTitle::MenuDocument_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&MenuDocument::UpdateFileMenu,
		&MenuDocument::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	jnew FileHistoryMenu(itsFileMenu, kRecentMenuCmd, menuBar);

	JXTextMenu* editMenu = itsClassNameInput->AppendEditMenu(menuBar);
	itsMenuTitleInput->ShareEditMenu(editMenu);
	itsWindowsKeyInput->ShareEditMenu(editMenu);

	auto* docMenu =
		jnew JXDocumentMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
							JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 10,10);
	menuBar->AppendMenu(docMenu);
	if (JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle)
	{
		docMenu->SetShortcuts(JGetString("WindowsMenuShortcut::JXGlobal"));
	}

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::MenuDocument_Preferences"));
	itsPrefsMenu->SetMenuItems(kPreferencesMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandler(this, &MenuDocument::HandlePrefsMenu);
	ConfigurePreferencesMenu(itsPrefsMenu);

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

	itsToolBar->LoadPrefs(nullptr);
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
 GetClassName

 ******************************************************************************/

const JString&
MenuDocument::GetClassName()
	const
{
	return itsClassNameInput->GetText()->GetText();
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

static const JRegex invalidCharPattern("[^_a-z0-9]+", "i");

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
	if (!ImageCache::FindProjectRoot(path, &projRoot))
	{
		JGetUserNotification()->ReportError(JGetString("NoProjectRoot::MenuDocument"));
		return;
	}

	// headers

	JString headerFullName = JCombinePathAndName(path, root);
	headerFullName         = JCombineRootAndSuffix(headerFullName, "h");

	JString tempHeaderFullName;
	JError err = JCreateTempFile(&path, nullptr, &tempHeaderFullName);
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
	std::ofstream headerOutput(tempHeaderFullName.GetBytes());

	name = root;
	JStringIterator nameIter(&name);
	while (nameIter.Next(invalidCharPattern))
	{
		nameIter.ReplaceLastMatch("_");
	}
	nameIter.Invalidate();

	JString title = itsMenuTitleInput->GetText()->GetText();
	JStringIterator titleIter(&title);
	JUtf8Character c;
	while (titleIter.Next(invalidCharPattern))
	{
		titleIter.RemoveLastMatch();
		if (titleIter.Next(&c, JStringIterator::kStay))
		{
			titleIter.SetNext(c.ToUpper(), JStringIterator::kStay);
		}
	}
	titleIter.Invalidate();

	const JUtf8Byte* headerMap[] =
	{
		"name",  name.GetBytes(),
		"title", title.GetBytes()
	};
	JGetString("CodeHeader::MenuDocument", headerMap, sizeof(headerMap)).Print(headerOutput);

	JString enumFullName = JCombinePathAndName(path, root + "-enum");
	enumFullName         = JCombineRootAndSuffix(enumFullName, "h");

	JString tempEnumFullName;
	err = JCreateTempFile(&path, nullptr, &tempEnumFullName);
	assert( err.OK() );
	std::ofstream enumOutput(tempEnumFullName.GetBytes());

	const JUtf8Byte* enumMap[] =
	{
		"name",  name.GetBytes(),
		"title", title.GetBytes()
	};
	JGetString("EnumHeader::MenuDocument", enumMap, sizeof(enumMap)).Print(enumOutput);

	JString enumFileName;
	JSplitPathAndName(enumFullName, &path, &enumFileName);

	itsTable->GenerateCode(headerOutput, itsClassNameInput->GetText()->GetText(),
						   title, itsWindowsKeyInput->GetText()->GetText(),
						   enumOutput, enumFileName);

	JGetString("CodeFooter::MenuDocument").Print(headerOutput);
	headerOutput.close();

	JGetString("CodeFooter::MenuDocument").Print(enumOutput);
	enumOutput.close();

	// check if header files actually changed

	JString origText, newText;
	JReadFile(headerFullName, &origText);
	JReadFile(tempHeaderFullName, &newText);
	if (newText != origText)
	{
		JEditVCS(headerFullName);
		JRenameFile(tempHeaderFullName, headerFullName, true);
	}
	else
	{
		JRemoveFile(tempHeaderFullName);
	}

	JReadFile(enumFullName, &origText);
	JReadFile(tempEnumFullName, &newText);
	if (newText != origText)
	{
		JEditVCS(enumFullName);
		JRenameFile(tempEnumFullName, enumFullName, true);
	}
	else
	{
		JRemoveFile(tempEnumFullName);
	}

	// strings

	JString stringsFullName = JCombinePathAndName(projRoot, "strings");
	stringsFullName         = JCombinePathAndName(stringsFullName, root);

	JEditVCS(stringsFullName);
	std::ofstream stringsOutput(stringsFullName.GetBytes());

	const JUtf8Byte* stringsMap[] =
	{
		"name", name.GetBytes()
	};
	JGetString("StringsHeader::MenuDocument", stringsMap, sizeof(stringsMap)).Print(stringsOutput);

	stringsOutput << itsMenuTitleInput->GetText()->GetText() << std::endl << std::endl;

	itsTable->GenerateStrings(stringsOutput, itsClassNameInput->GetText()->GetText());
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
		if (itsTable->EndEditing() && itsTable->ValidateWindowsKeys())
		{
			SaveInCurrentFile();
		}
	}
	else if (index == kSaveAsCmd)
	{
		if (itsTable->EndEditing() && itsTable->ValidateWindowsKeys())
		{
			bool onDisk;
			const JString fullName = GetFullName(&onDisk);
			if (onDisk)
			{
				GetDocumentManager()->AddToFileHistoryMenu(fullName);
			}

			SaveInNewFile();
		}
	}
	else if (index == kSaveCopyAsCmd)
	{
		JString fullName;
		if (itsTable->EndEditing() && itsTable->ValidateWindowsKeys() &&
			SaveCopyInNewFile(JString::empty, &fullName))
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
