/******************************************************************************
 LayoutDocument.cpp

	BASE CLASS = public JXFileDocument

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#include "LayoutDocument.h"
#include "FileHistoryMenu.h"
#include "LayoutContainer.h"
#include "MDIServer.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXDocumentMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXGetStringDialog.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXMacWinPrefsDialog.h>
#include <jx-af/jcore/JUndoRedoChain.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jTextUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kFileSignature = "jx_layout_editor";
const JFileVersion kCurrentFileVersion = 0;

static const JUtf8Byte* kBeginCodeDelimiterPrefix = "// begin ";
static const JUtf8Byte* kEndCodeDelimiterPrefix   = "// end ";

/******************************************************************************
 Create (static)

	Create an empty layout.

 *****************************************************************************/

bool
LayoutDocument::Create
	(
	LayoutDocument** doc
	)
{
	auto* dlog = jnew JXGetStringDialog(
		JGetString("NewLayoutNameWindowTitle::LayoutDocument"),
		JGetString("EditLayoutNamePrompt::LayoutDocument"));

	dlog->GetInputField()->SetValidationPattern(
		jnew JRegex("^[_a-z][_a-z0-9]+$", "i"),
		"LayoutNameMustBeValidIdentifier::LayoutDocument");

	if (dlog->DoDialog())
	{
		*doc = jnew LayoutDocument(dlog->GetString(), false);
		(**doc).SetDataReverted();
		(**doc).Activate();
		return true;
	}

	*doc = nullptr;
	return false;
}

/******************************************************************************
 Create (static)

	Load layout(s) from a file.

 *****************************************************************************/

bool
LayoutDocument::Create
	(
	const JString& fullName
	)
{
	std::ifstream input(fullName.GetBytes());
	JFileVersion vers;

	FileStatus status = DefaultCanReadASCIIFile(input, "Magic:", 15000, &vers);
	if (status == kFileReadable)
	{
		ImportFDesignFile(input);
		return true;
	}
	else if (status == kNeedNewerVersion)
	{
		const JString v((JUInt64) vers);
		const JUtf8Byte* map[] =
		{
			"v", v.GetBytes()
		};
		const JString msg = JGetString("UnsupportedFDesignVersion::LayoutDocument", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		return false;
	}

	JXFileDocument* fdoc;
	if (JXGetDocumentManager()->FileDocumentIsOpen(fullName, &fdoc))
	{
		fdoc->Activate();
		return true;
	}

	status = DefaultCanReadASCIIFile(input, kFileSignature, kCurrentFileVersion, &vers);
	if (status == kFileReadable)
	{
		auto* doc = jnew LayoutDocument(fullName, true);
		doc->ReadFile(input);
		doc->SetDataReverted();
		doc->Activate();
		return true;
	}
	else if (status == kNeedNewerVersion)
	{
		JGetUserNotification()->ReportError(JGetString("NeedNewerVersion::LayoutDocument"));
		return false;
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("UnknownFile::LayoutDocument"));
		return false;
	}
}

/******************************************************************************
 Constructor (protected)

 *****************************************************************************/

LayoutDocument::LayoutDocument
	(
	const JString&	fullName,
	const bool		onDisk
	)
	:
	JXFileDocument(JXGetApplication(), fullName, onDisk, false, ".jxl"),
	itsLayout(nullptr)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

LayoutDocument::~LayoutDocument()
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
#include "LayoutDocument-File.h"
#include "LayoutDocument-Preferences.h"
#include "LayoutDocument-Grid.h"

void
LayoutDocument::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 601,300, JString::empty);

	itsMenuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 601,30);
	assert( itsMenuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kLayoutDocToolBarID, itsMenuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 601,270);
	assert( itsToolBar != nullptr );

// end JXLayout

	AdjustWindowTitle();
	window->SetCloseAction(JXWindow::kCloseDirector);
	window->SetWMClass(GetWMClassInstance(), GetLayoutDocumentClass());
	window->SetMinSize(200, 60);

	auto* image = jnew JXImage(GetDisplay(), main_window_icon);
	window->SetIcon(image);

	itsLayout =
		jnew LayoutContainer(this, itsMenuBar, itsToolBar->GetWidgetEnclosure(),
							 JXWidget::kHElastic,JXWidget::kVElastic,
							 0,0, 100,100);
	itsLayout->FitToEnclosure();

	GetWindow()->SetStepSize(itsLayout->GetGridSpacing(), itsLayout->GetGridSpacing());

	// menus

	itsFileMenu = itsMenuBar->PrependTextMenu(JGetString("MenuTitle::LayoutDocument_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&LayoutDocument::UpdateFileMenu,
		&LayoutDocument::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	jnew FileHistoryMenu(itsFileMenu, kRecentMenuCmd, itsMenuBar);

	auto* docMenu =
		jnew JXDocumentMenu(JGetString("WindowsMenuTitle::JXGlobal"), itsMenuBar,
							JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 10,10);
	itsMenuBar->AppendMenu(docMenu);
	if (JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle)
	{
		docMenu->SetShortcuts(JGetString("WindowsMenuShortcut::JXGlobal"));
	}

	itsPrefsMenu = itsMenuBar->AppendTextMenu(JGetString("MenuTitle::LayoutDocument_Preferences"));
	itsPrefsMenu->SetMenuItems(kPreferencesMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandler(this, &LayoutDocument::HandlePrefsMenu);
	ConfigurePreferencesMenu(itsPrefsMenu);

	itsGridMenu = jnew JXTextMenu(itsPrefsMenu, kGridMenuCmd, itsMenuBar);
	itsGridMenu->SetMenuItems(kGridMenuStr);
	itsGridMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsGridMenu->AttachHandlers(this,
		&LayoutDocument::UpdateGridMenu,
		&LayoutDocument::HandleGridMenu);
	ConfigureGridMenu(itsGridMenu);

	auto* helpMenu = GetApplication()->CreateHelpMenu(itsMenuBar, "MainHelp");

	// must be done after creating widgets

	itsToolBar->LoadPrefs(nullptr);
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kNewCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kSaveCmd);
		itsToolBar->AppendButton(itsFileMenu, kSaveAllCmd);

		itsLayout->AppendToToolBar(itsToolBar);
		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, helpMenu);
	}
}

/******************************************************************************
 GetName (virtual)

	Override of JXDocument::GetName().

 ******************************************************************************/

const JString&
LayoutDocument::GetName()
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
LayoutDocument::ReadFile
	(
	std::istream&	input,
	const bool		isUndoRedo
	)
{
	input.ignore(strlen(kFileSignature));

	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentFileVersion );

	itsLayout->Clear(isUndoRedo);

	JCoordinate w,h;
	input >> w >> h;
	SetLayoutSize(w,h);

	itsLayout->ReadConfig(input, vers);
}

/******************************************************************************
 WriteTextFile (virtual protected)

 ******************************************************************************/

void
LayoutDocument::WriteTextFile
	(
	std::ostream&	output,
	const bool		safetySave
	)
	const
{
	output << kFileSignature << ' ' << kCurrentFileVersion << std::endl;
	output << itsLayout->GetApertureWidth() << std::endl;
	output << itsLayout->GetApertureHeight() << std::endl;

	itsLayout->WriteConfig(output);

	if (!safetySave)
	{
		GenerateCode();
	}
}

/******************************************************************************
 DiscardChanges (virtual protected)

 ******************************************************************************/

void
LayoutDocument::DiscardChanges()
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
		SetDataReverted();
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("UnknownFile::LayoutDocument"));
	}
}

/******************************************************************************
 UpdateSaveState

 ******************************************************************************/

void
LayoutDocument::UpdateSaveState()
{
	if (itsLayout != nullptr && itsLayout->GetUndoRedoChain()->IsAtLastSaveLocation())
	{
		DataReverted(true);
	}
	else
	{
		DataModified();
	}
}

/******************************************************************************
 SetDataReverted

 ******************************************************************************/

void
LayoutDocument::SetDataReverted()
{
	itsLayout->GetUndoRedoChain()->ClearUndo();
	itsLayout->GetUndoRedoChain()->SetLastSaveLocation();
	DataReverted();
}

/******************************************************************************
 SetLayoutSize

 ******************************************************************************/

void
LayoutDocument::SetLayoutSize
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	itsLayout->SetIgnoreResize(true);

	GetWindow()->AdjustSize(
		w - itsLayout->GetApertureWidth(),
		h - itsLayout->GetApertureHeight());

	itsLayout->SetIgnoreResize(false);
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
LayoutDocument::UpdateFileMenu()
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
LayoutDocument::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kNewCmd)
	{
		LayoutDocument* doc;
		Create(&doc);
	}
	else if (index == kOpenCmd)
	{
		MDIServer::ChooseFiles();
	}

	else if (index == kSaveCmd)
	{
		SaveInCurrentFile();
		itsLayout->GetUndoRedoChain()->DeactivateCurrentUndo();
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
		itsLayout->GetUndoRedoChain()->DeactivateCurrentUndo();
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
LayoutDocument::HandlePrefsMenu
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
}

/******************************************************************************
 UpdateGridMenu (private)

 ******************************************************************************/

void
LayoutDocument::UpdateGridMenu()
{
	JXWidget* widget = nullptr;
	const bool ok = GetWindow()->GetFocusWidget(&widget);
	assert( ok );
	LayoutContainer* layout = dynamic_cast<LayoutContainer*>(widget);
	assert( layout != nullptr );

	const JSize w = layout->GetGridSpacing();
	if (w == 5)
	{
		itsGridMenu->CheckItem(kGrid5Cmd);
	}
	else if (w == 10)
	{
		itsGridMenu->CheckItem(kGrid10Cmd);
	}
}

/******************************************************************************
 HandleGridMenu (private)

 ******************************************************************************/

void
LayoutDocument::HandleGridMenu
	(
	const JIndex index
	)
{
	JXWidget* widget = nullptr;
	const bool ok = GetWindow()->GetFocusWidget(&widget);
	assert( ok );
	LayoutContainer* layout = dynamic_cast<LayoutContainer*>(widget);
	assert( layout != nullptr );

	if (index == kGrid5Cmd)
	{
		layout->SetGridSpacing(5);
		if (layout == itsLayout)
		{
			GetWindow()->SetStepSize(5,5);
		}
	}
	if (index == kGrid10Cmd)
	{
		layout->SetGridSpacing(10);
		if (layout == itsLayout)
		{
			GetWindow()->SetStepSize(10,10);
		}
	}
}

/******************************************************************************
 GenerateCode (private)

 ******************************************************************************/

static const JUtf8Byte* kSourceFileSuffixList[] = { "cc", "cpp", "cxx", nullptr };
static const JUtf8Byte* kHeaderFileSuffixList[] = { "h", "hh", "hpp", nullptr };

bool
LayoutDocument::GenerateCode()
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
		JGetUserNotification()->ReportError(JGetString("NoProjectRoot::LayoutDocument"));
		return false;
	}

	JString sourceFullName = JCombinePathAndName(path, root);
	if (!FindInputFile(&sourceFullName, kSourceFileSuffixList))
	{
		JGetUserNotification()->ReportError(JGetString("NoSourceFile::LayoutDocument"));
		return false;
	}

	JString headerFullName = JCombinePathAndName(path, root);
	if (!FindInputFile(&headerFullName, kHeaderFileSuffixList))
	{
		JGetUserNotification()->ReportError(JGetString("NoHeaderFile::LayoutDocument"));
		return false;
	}

	bool changed = false;

	//
	// source
	//

	JString tempSourceFullName;
	JError err = JCreateTempFile(&path, nullptr, &tempSourceFullName);
	if (!err.OK())
	{
		const JUtf8Byte* map[] =
		{
			"path", path.GetBytes(),
			"err",  err.GetMessage().GetBytes()
		};
		JGetUserNotification()->ReportError(JGetString("CodeDirectoryNotWritable::LayoutDocument", map, sizeof(map)));
		return false;
	}
	std::ifstream sourceInput(sourceFullName.GetBytes());
	std::ofstream sourceOutput(tempSourceFullName.GetBytes());
	sourceOutput << std::boolalpha;

	JString indent;
	if (!CopyBeforeCodeDelimiter(sourceInput, sourceOutput, &indent))
	{
		JString p,n;
		JSplitPathAndName(sourceFullName, &p, &n);
		const JUtf8Byte* map[] =
		{
			"f", n.GetBytes()
		};
		JGetUserNotification()->ReportError(JGetString("NoStartDelimiter::LayoutDocument", map, sizeof(map)));
		sourceOutput.close();
		JRemoveFile(tempSourceFullName);
		return false;
	}

	JPtrArray<JString> objTypes(JPtrArrayT::kDeleteAll),
					   objNames(JPtrArrayT::kDeleteAll);
	JStringManager stringdb;
	itsLayout->GenerateCode(sourceOutput, indent, &objTypes, &objNames, &stringdb);

	bool done = CopyAfterCodeDelimiter(sourceInput, sourceOutput);
	sourceInput.close();
	sourceOutput.close();

	if (!done)
	{
		JString p,n;
		JSplitPathAndName(sourceFullName, &p, &n);
		const JUtf8Byte* map[] =
		{
			"f", n.GetBytes()
		};
		JGetUserNotification()->ReportError(JGetString("NoEndDelimiter::LayoutDocument", map, sizeof(map)));
		JRemoveFile(tempSourceFullName);
		return false;
	}

	// check if source file actually changed

	JString origText, newText;
	JReadFile(sourceFullName, &origText);
	JReadFile(tempSourceFullName, &newText);
	if (newText != origText)
	{
		JEditVCS(sourceFullName);
		JRenameFile(tempSourceFullName, sourceFullName, true);
		changed = true;
	}
	else
	{
		JRemoveFile(tempSourceFullName);
	}

	//
	// header
	//

	JString tempHeaderFullName;
	err = JCreateTempFile(&path, nullptr, &tempHeaderFullName);
	if (!err.OK())
	{
		const JUtf8Byte* map[] =
		{
			"path", path.GetBytes(),
			"err",  err.GetMessage().GetBytes()
		};
		JGetUserNotification()->ReportError(JGetString("CodeDirectoryNotWritable::LayoutDocument", map, sizeof(map)));
		return false;
	}
	std::ifstream headerInput(headerFullName.GetBytes());
	std::ofstream headerOutput(tempHeaderFullName.GetBytes());

	if (!CopyBeforeCodeDelimiter(headerInput, headerOutput, &indent))
	{
		JString p,n;
		JSplitPathAndName(headerFullName, &p, &n);
		const JUtf8Byte* map[] =
		{
			"f", n.GetBytes()
		};
		JGetUserNotification()->ReportError(JGetString("NoStartDelimiter::LayoutDocument", map, sizeof(map)));
		headerOutput.close();
		JRemoveFile(tempHeaderFullName);
		return false;
	}

	GenerateHeader(headerOutput, objTypes, objNames, indent);

	done = CopyAfterCodeDelimiter(headerInput, headerOutput);
	headerInput.close();
	headerOutput.close();

	if (!done)
	{
		JString p,n;
		JSplitPathAndName(headerFullName, &p, &n);
		const JUtf8Byte* map[] =
		{
			"f", n.GetBytes()
		};
		JGetUserNotification()->ReportError(JGetString("NoEndDelimiter::LayoutDocument", map, sizeof(map)));
		JRemoveFile(tempHeaderFullName);
		return false;
	}

	// check if header file actually changed

	JReadFile(headerFullName, &origText);
	JReadFile(tempHeaderFullName, &newText);
	if (newText != origText)
	{
		JEditVCS(headerFullName);
		JRenameFile(tempHeaderFullName, headerFullName, true);
		changed = true;
	}
	else
	{
		JRemoveFile(tempHeaderFullName);
	}

	//
	// string database
	//

	JString stringsFullName = JCombinePathAndName(projRoot, "strings");
	stringsFullName         = JCombinePathAndName(stringsFullName, root);
	stringsFullName        += "_layout";

	if (stringdb.GetItemCount() > 0)
	{
		JEditVCS(stringsFullName);
		std::ofstream output(stringsFullName.GetBytes());
		stringdb.WriteFile(output);
		JGetString("StringsFooter::LayoutDocument").Print(output);
	}
	else
	{
		JRemoveVCS(stringsFullName);
		JRemoveFile(stringsFullName);
	}

	if (changed && JProgramAvailable(CODE_CRUSADER_BINARY))
	{
		const JError err = JExecute(CODE_CRUSADER_BINARY " --force-reload-open", nullptr);
		err.ReportIfError();
	}

	return true;
}

/******************************************************************************
 FindProjectRoot (static)

	Search directory tree up to root.

 ******************************************************************************/

bool
LayoutDocument::FindProjectRoot
	(
	const JString&	path,
	JString*		root
	)
{
	JString p = path, n;
	do
	{
		n = JCombinePathAndName(p, "Makefile");
		if (JFileExists(n))
		{
			*root = p;
			return true;
		}

		n = JCombinePathAndName(p, "Make.header");
		if (JFileExists(n))
		{
			*root = p;
			return true;
		}

		n = JCombinePathAndName(p, "CMakeLists.txt");
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
 FindInputFile (static private)

 ******************************************************************************/

bool
LayoutDocument::FindInputFile
	(
	JString*			root,
	const JUtf8Byte*	suffixList[]
	)
{
	const JUtf8Byte** s = suffixList;
	JString fullName;
	while (*s != nullptr)
	{
		fullName = JCombineRootAndSuffix(*root,  *s);
		if (JFileReadable(fullName))
		{
			root->Set(fullName);
			return true;
		}
		s++;
	}

	return false;
}

/******************************************************************************
 CopyBeforeCodeDelimiter (private)

 ******************************************************************************/

bool
LayoutDocument::CopyBeforeCodeDelimiter
	(
	std::istream&	input,
	std::ostream&	output,
	JString*		indent
	)
	const
{
	JString buffer;

	const JString delim = kBeginCodeDelimiterPrefix + itsLayout->GetCodeTag();
	while (!input.eof() && !input.fail())
	{
		const JString line = JReadLine(input);
		line.Print(output);
		output << '\n';
		if (line == delim)
		{
			output << '\n';
			break;
		}

		buffer += line;
		buffer += "\n";
	}

	bool useSpaces, isMixed;
	JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	*indent = useSpaces ? "    " : "\t";

	return !input.eof() && !input.fail();
}

/******************************************************************************
 CopyAfterCodeDelimiter (private)

	Skips everything before end delimiter and then copies the rest.

 ******************************************************************************/

bool
LayoutDocument::CopyAfterCodeDelimiter
	(
	std::istream& input,
	std::ostream& output
	)
	const
{
	const JString delim = kEndCodeDelimiterPrefix + itsLayout->GetCodeTag();

	// skip lines before end delimiter

	while (!input.eof() && !input.fail())
	{
		const JString line = JReadLine(input);
		if (line == delim)
		{
			break;
		}
	}

	if (input.eof() || input.fail())
	{
		return false;
	}

	// include end delimiter

	delim.Print(output);
	output << std::endl;

	// copy lines after end delimiter

	while (true)
	{
		const JString line = JReadLine(input);
		if ((input.eof() || input.fail()) && line.IsEmpty())
		{
			break;	// avoid creating extra empty lines
		}
		line.Print(output);
		output << std::endl;
	}

	return true;
}

/******************************************************************************
 GenerateHeader

 ******************************************************************************/

void
LayoutDocument::GenerateHeader
	(
	std::ostream&				output,
	const JPtrArray<JString>&	objTypes,
	const JPtrArray<JString>&	objNames,
	const JString&				indent
	)
	const
{
	const JSize count = objTypes.GetItemCount();
	assert( count == objNames.GetItemCount() );

	// get width of longest type

	JSize maxLen = 0;
	for (const auto* type : objTypes)
	{
		maxLen = JMax(maxLen, type->GetCharacterCount());
	}

	// declare each object

	for (JIndex i=1; i<=count; i++)
	{
		indent.Print(output);

		const JString* type = objTypes.GetItem(i);
		type->Print(output);
		output << '*';

		const JSize len = type->GetCharacterCount();
		for (JIndex j=len+1; j<=maxLen+1; j++)
		{
			output << ' ';
		}
		objNames.GetItem(i)->Print(output);
		output << ';' << std::endl;
	}

	// need blank line to conform to expectations of CopyAfterCodeDelimiter

	output << std::endl;
}

/******************************************************************************
 ImportFDesignFile (static private)

 ******************************************************************************/

void
LayoutDocument::ImportFDesignFile
	(
	std::istream& input
	)
{
	while (!input.eof() && !input.fail())
	{
		auto* doc = jnew LayoutDocument(JString::empty, false);
		if (doc->ImportFDesignLayout(input))
		{
			doc->SetDataReverted();
			doc->Activate();
		}
		else
		{
			doc->DataReverted();
			doc->Close();
		}
	}
}

/******************************************************************************
 ImportFDesignLayout

 ******************************************************************************/

#include "CustomWidget.h"
#include "InputField.h"
#include "StaticText.h"
#include "TextButton.h"
#include "TextCheckbox.h"
#include "WidgetSet.h"

// Form fields

static const JUtf8Byte* kBeginFormLine      = "=============== FORM ===============";
static const JUtf8Byte* kFormNameMarker     = "Name:";
static const JUtf8Byte* kFormWidthMarker    = "Width:";
static const JUtf8Byte* kFormHeightMarker   = "Height:";
static const JUtf8Byte* kFormObjCountMarker = "Number of Objects:";

// Object fields

static const JUtf8Byte* kBeginObjLine      = "--------------------";
static const JUtf8Byte* kObjClassMarker    = "class:";
static const JUtf8Byte* kObjTypeMarker     = "type:";
static const JUtf8Byte* kObjRectMarker     = "box:";
static const JUtf8Byte* kObjBoxTypeMarker  = "boxtype:";
static const JUtf8Byte* kObjColorsMarker   = "colors:";
static const JUtf8Byte* kObjLAlignMarker   = "alignment:";
static const JUtf8Byte* kObjLStyleMarker   = "style:";
static const JUtf8Byte* kObjLSizeMarker    = "size:";
static const JUtf8Byte* kObjLColorMarker   = "lcol:";
static const JUtf8Byte* kObjLabelMarker    = "label:";
static const JUtf8Byte* kObjShortcutMarker = "shortcut:";
static const JUtf8Byte* kObjGravityMarker  = "gravity:";
static const JUtf8Byte* kObjNameMarker     = "name:";
static const JUtf8Byte* kObjCBArgMarker    = "argument:";

bool
LayoutDocument::ImportFDesignLayout
	(
	std::istream& input
	)
{
	JString s;
	do
	{
		s = JReadLine(input);
	}
		while (s != kBeginFormLine && !input.eof() && !input.fail());

	if (input.eof() || input.fail())
	{
		return false;
	}

	// name

	s = ReadFDesignString(input, kFormNameMarker);
	FileChanged(s, false);

	// window size

	SetLayoutSize(
		ReadFDesignNumber(input, kFormWidthMarker),
		ReadFDesignNumber(input, kFormHeightMarker));

	// object count (marker contains whitespace)

	JString flClass, flType, boxType, color1, color2,
		label, labelAlign, labelStyle, labelSize, labelColor,
		shortcuts, gravity, varName, argument, className, argList;
	JArray<JRect> rectList;
	JPtrArray<JString> objNames(JPtrArrayT::kDeleteAll), tmp(JPtrArrayT::kDeleteAll);
	JPtrArray<BaseWidget> widgetList(JPtrArrayT::kForgetAll);

	const JSize itemCount = ReadFDesignNumber(input, kFormObjCountMarker);
	for (JIndex i=1; i<=itemCount; i++)
	{
		// check for start-of-object

		input >> std::ws;
		s = JReadLine(input);
		assert( s == kBeginObjLine );

		flClass = ReadFDesignString(input, kObjClassMarker);
		flType  = ReadFDesignString(input, kObjTypeMarker);

		JCoordinate x = ReadFDesignNumber(input, kObjRectMarker);
		JCoordinate y,w,h;
		input >> y >> w >> h;

		boxType = ReadFDesignString(input, kObjBoxTypeMarker);
		if (flClass == "FL_BOX")	// for actual boxes, use boxType instead of type
		{
			flType = boxType;
		}

		s = ReadFDesignString(input, kObjColorsMarker);
		s.Split(" ", &tmp);
		assert( tmp.GetItemCount() == 2 );
		color1 = *tmp.GetItem(1);
		color2 = *tmp.GetItem(2);

		labelAlign = ReadFDesignString(input, kObjLAlignMarker);
		labelStyle = ReadFDesignString(input, kObjLStyleMarker);
		labelSize  = ReadFDesignString(input, kObjLSizeMarker);
		labelSize  = ReadFDesignString(input, kObjLColorMarker);
		label      = ReadFDesignString(input, kObjLabelMarker);

		shortcuts = ReadFDesignString(input, kObjShortcutMarker);

		JIgnoreLine(input);		// resizing

		s = ReadFDesignString(input, kObjGravityMarker);
		s.Split(" ", &tmp);
		gravity = *tmp.GetItem(1);

		varName = ReadFDesignString(input, kObjNameMarker);

		JIgnoreLine(input);		// callback

		argument = ReadFDesignString(input, kObjCBArgMarker);

		do
		{
			s = JReadLine(input);
			s.TrimWhitespace();
		}
			while (!s.IsEmpty());

		// don't bother to generate code for initial box
		// if it is FL_BOX, FL_FLAT_BOX, FL_COL1

		if (i == 1 && flClass == "FL_BOX" && flType == "FL_FLAT_BOX" && color1 == "FL_COL1")
		{
			continue;
		}

		bool varIsInstance = false;
		if (varName.IsEmpty())
		{
			varName = GetTempFDesignVarName(objNames);
		}
		else if (varName.GetFirstCharacter() == '(' &&
				  varName.GetLastCharacter()  == ')')
		{
			JStringIterator iter(&varName);
			iter.SkipNext();
			iter.RemoveAllPrev();
			iter.MoveTo(JStringIterator::kStartAtEnd, 0);
			iter.SkipPrev();
			iter.RemoveAllNext();
		}
		else
		{
			varIsInstance = true;
		}

		JRect frame(y, x, y+h, x+w);
		JIndex enclIndex;
		LayoutContainer* enclosure;
		JRect localFrame = frame;
		if (GetFDesignEnclosure(frame, rectList, &enclIndex))
		{
			const bool ok = widgetList.GetItem(enclIndex)->GetLayoutContainer(&enclosure);
			if (!ok)
			{
				return false;
			}

			const JRect enclFrame = rectList.GetItem(enclIndex);
			localFrame.Shift(-enclFrame.topLeft());
		}
		else
		{
			enclosure = itsLayout;
		}

		JXWidget::HSizingOption hS;
		JXWidget::VSizingOption vS;
		ParseFDesignGravity(gravity, &hS, &vS);

		x = localFrame.left;
		y = localFrame.top;
		w = localFrame.width();
		h = localFrame.height();

		BaseWidget* widget;
		if (flClass == "FL_INPUT" && flType == "FL_NORMAL_INPUT")
		{
			widget = jnew InputField(enclosure, hS,vS, x,y,w,h);
		}
		else if (flClass == "FL_TEXT" && flType == "FL_NORMAL_TEXT")
		{
			widget = jnew StaticText(label, enclosure, hS,vS, x,y,w,h);
		}
		else if (flClass == "FL_BUTTON")
		{
			widget = jnew TextButton(label, shortcuts, enclosure, hS,vS, x,y,w,h);
		}
		else if (flClass == "FL_CHECKBUTTON")
		{
			widget = jnew TextCheckbox(label, shortcuts, enclosure, hS,vS, x,y,w,h);
		}
		else if (flClass == "FL_BOX" && flType == "FL_SHADOW_BOX")
		{
			widget = jnew WidgetSet(enclosure, hS,vS, x,y,w,h);
		}
		else // if (flClass == "FL_BOX" && flType == "FL_NO_BOX" && !label->IsEmpty())
		{
			SplitFDesignClassNameAndArgs(label, &className, &argList);
			if (argList.IsEmpty())
			{
				argList = argument;
			}

			widget = jnew CustomWidget(className, argList, false,
										enclosure, hS,vS, x,y,w,h);
		}

		if (widget != nullptr)
		{
			widget->SetVarName(varName, varIsInstance);

			rectList.AppendItem(frame);
			objNames.Append(varName);
			widgetList.Append(widget);
		}
	}

	return true;
}

/******************************************************************************
 ReadFDesignString (static private)

 ******************************************************************************/

JString
LayoutDocument::ReadFDesignString
	(
	std::istream&		input,
	const JUtf8Byte*	marker
	)
{
	JString s;
	input >> std::ws;
	s = JReadUntil(input, ':') + ":";
	assert( s == marker );

	s = JReadLine(input);
	s.TrimWhitespace();
	return s;
}

/******************************************************************************
 ReadFDesignNumber (static private)

 ******************************************************************************/

JCoordinate
LayoutDocument::ReadFDesignNumber
	(
	std::istream&		input,
	const JUtf8Byte*	marker
	)
{
	JString s;
	input >> std::ws;
	s = JReadUntil(input, ':') + ":";
	assert( s == marker );

	JCoordinate v;
	input >> v;
	return v;
}

/******************************************************************************
 ParseFDesignGravity (static private)

 ******************************************************************************/

struct GravityMap
{
	const JUtf8Byte* gravity;
	const JXWidget::HSizingOption hSizing;
	const JXWidget::VSizingOption vSizing;
};

static const GravityMap kGravityMap[] =
{
	{ "FL_NoGravity", JXWidget::kHElastic,   JXWidget::kVElastic    },	// default
	{ "FL_North",     JXWidget::kHElastic,   JXWidget::kFixedTop    },
	{ "FL_NorthEast", JXWidget::kFixedRight, JXWidget::kFixedTop    },
	{ "FL_East",      JXWidget::kFixedRight, JXWidget::kVElastic    },
	{ "FL_SouthEast", JXWidget::kFixedRight, JXWidget::kFixedBottom },
	{ "FL_South",     JXWidget::kHElastic,   JXWidget::kFixedBottom },
	{ "FL_SouthWest", JXWidget::kFixedLeft,  JXWidget::kFixedBottom },
	{ "FL_West",      JXWidget::kFixedLeft,  JXWidget::kVElastic    },
	{ "FL_NorthWest", JXWidget::kFixedLeft,  JXWidget::kFixedTop    }
};

const JSize kGravityCount = sizeof(kGravityMap) / sizeof(GravityMap);

void
LayoutDocument::ParseFDesignGravity
	(
	const JString&				gravity,
	JXWidget::HSizingOption*	hSizing,
	JXWidget::VSizingOption*	vSizing
	)
{
	for (JUnsignedOffset i=0; i<kGravityCount; i++)
	{
		if (gravity == kGravityMap[i].gravity)
		{
			*hSizing = kGravityMap[i].hSizing;
			*vSizing = kGravityMap[i].vSizing;
			return;
		}
	}

	*hSizing = kGravityMap[0].hSizing;
	*vSizing = kGravityMap[0].vSizing;
}

/******************************************************************************
 GetTempFDesignVarName (static private)

	Return a variable name that is not in the given list.

	We ignore the possibility of not finding a valid name because the
	code we are writing will run out of memory long before we run out
	of possibilities.

 ******************************************************************************/

JString
LayoutDocument::GetTempFDesignVarName
	(
	const JPtrArray<JString>& objNames
	)
{
	const JString& base = JGetString("VarNameBase::LayoutContainer");

	JString varName;
	for (JIndex i=1; i<=INT_MAX; i++)
	{
		varName = base + JString((JUInt64) i);

		bool unique = true;
		for (const auto* usedName : objNames)
		{
			if (varName == *usedName)
			{
				unique = false;
				break;
			}
		}
		if (unique)
		{
			break;
		}
	}

	return varName;
}

/******************************************************************************
 GetFDesignEnclosure

	Returns true if it finds a rectangle that encloses the rectangle
	at the specified index.  If it finds more than one enclosing rectangle,
	it returns the smallest one.

	If no enclosure is found, returns false, and sets *enclIndex to zero.

 ******************************************************************************/

bool
LayoutDocument::GetFDesignEnclosure
	(
	const JRect&			frame,
	const JArray<JRect>&	rectList,
	JIndex*					enclIndex
	)
{
	bool found    = false;
	*enclIndex    = 0;
	JSize minArea = 0;

	const JSize count = rectList.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JRect r = rectList.GetItem(i);
		const JSize a = r.area();
		if (r.Contains(frame) && (a < minArea || minArea == 0))
		{
			minArea    = a;
			found      = true;
			*enclIndex = i;
		}
	}

	return found;
}

/******************************************************************************
 SplitFDesignClassNameAndArgs (static private)

	Returns false if there is no class name.

 ******************************************************************************/

void
LayoutDocument::SplitFDesignClassNameAndArgs
	(
	const JString&	str,
	JString*		name,
	JString*		args
	)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	str.Split("(", &list, 2);

	const bool hasArgs = list.GetItemCount() > 1;
	if (hasArgs &&
		!list.GetFirstItem()->IsEmpty() &&
		!list.GetLastItem()->IsEmpty())
	{
		*name = *list.GetFirstItem();
		*args = *list.GetLastItem();

		name->TrimWhitespace();
		args->TrimWhitespace();
		return;
	}

	if (hasArgs)
	{
		*name = *list.GetFirstItem();
	}
	else
	{
		*name = str;
	}
	name->TrimWhitespace();
	args->Clear();
}
