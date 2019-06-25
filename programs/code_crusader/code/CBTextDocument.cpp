/******************************************************************************
 CBTextDocument.cpp

	BASE CLASS = JXFileDocument, JPrefObject

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "CBEditTextPrefsDialog.h"
#include "CBTELineIndexInput.h"
#include "CBTEColIndexInput.h"
#include "CBFileDragSource.h"
#include "CBFileNameDisplay.h"
#include "CBFileHistoryMenu.h"
#include "CBTabWidthDialog.h"
#include "CBProjectDocument.h"
#include "CBDiffFileDialog.h"
#include "CBStylerBase.h"
#include "CBPTPrinter.h"
#include "CBPSPrinter.h"
#include "CBDocumentMenu.h"
#include "CBCommandMenu.h"
#include "cbGlobals.h"
#include "cbmUtil.h"
#include "cbActionDefs.h"
#include <JXDocumentManager.h>
#include <JXWebBrowser.h>
#include <JXWindow.h>
#include <JXDockWidget.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXMenuBar.h>
#include <JXToolBar.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXDownRect.h>
#include <JXColorManager.h>
#include <JXSharedPrefsManager.h>
#include <jXEventUtil.h>
#include <JFontManager.h>
#include <jFileUtil.h>
#include <jVCSUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 2;
const JCharacter kSetupDataEndDelimiter = '\1';

	// version 1 includes itsBreakCodeCROnlyFlag
	// version 2 removes itsBreakCodeCROnlyFlag

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
	"%l| Save                           %k Meta-S       %i" kCBSaveFileAction
	"  | Save as...                     %k Ctrl-S       %i" kCBSaveFileAsAction
	"  | Save a copy as...              %k Ctrl-Shift-S %i" kCBSaveCopyAsAction
	"  | Save as template...                            %i" kCBSaveAsTemplateAction
	"  | Revert to saved                                %i" kCBRevertAction
	"  | Save all                       %k Meta-Shift-S %i" kCBSaveAllTextFilesAction
	"%l| File format"
	"%l| Compare files...                               %i" kCBDiffFilesAction
	"  | Compare with...                                %i" kCBDiffSmartAction
	"  | Compare with version control                   %i" kCBDiffVCSAction
	"  | Compare"
	"%l| Show in file manager           %k Dbl-click icon in footer %i" kCBShowInFileMgrAction
	"%l| Page setup...                                  %i" kJXPageSetupAction
	"  | Print...                       %k Meta-P       %i" kJXPrintAction
	"%l| Page setup for styles...                       %i" kCBPageSetupStyledTextAction
	"  | Print with styles...           %k Meta-Shift-P %i" kCBPrintStyledTextAction
	"%l| Close                          %k Meta-W       %i" kJXCloseWindowAction
	"  | Close all                      %k Meta-Shift-W %i" kCBCloseAllTextFilesAction
	"%l| Quit                           %k Meta-Q       %i" kJXQuitAction;

enum
{
	kNewTextEditorCmd = 1, kNewTextTemplateCmd, kNewProjectCmd, kNewShellCmd,
	kOpenSomethingCmd, kRecentProjectMenuCmd, kRecentTextMenuCmd,
	kSaveFileCmd, kSaveFileAsCmd, kSaveCopyAsCmd, kSaveAsTemplateCmd,
	kRevertCmd, kSaveAllFilesCmd,
	kFileFormatIndex,
	kDiffFilesCmd, kDiffSmartCmd, kDiffVCSCmd, kDiffMenuIndex,
	kShowInFileMgrCmd,
	kPTPageSetupCmd, kPrintPTCmd,
	kPSPageSetupCmd, kPrintPSCmd,
	kCloseCmd, kCloseAllCmd,
	kQuitCmd
};

// File format menu

static const JCharacter* kFileFormatMenuStr =
	"  UNIX                 %r %i" kCBUNIXTextFormatAction
	"| Macintosh (pre OS X) %r %i" kCBMacTextFormatAction
	"| DOS / Windows        %r %i" kCBDOSTextFormatAction;

enum
{
	kUNIXFmtCmd = 1, kMacFmtCmd, kDOSFmtCmd
};

// Diff menu

static const JCharacter* kDiffMenuStr =
	"    Compare as file #1..."
	"  | Compare as file #2..."
	"%l| Compare with version control...";

enum
{
	kDiffAs1Cmd = 1,
	kDiffAs2Cmd,
	kDiffAsVCSCmd
};

// Windows menu

static const JCharacter* kFileListMenuTitleStr = "Windows";

// Preferences menu

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
	"    Editor..."
	"  | Toolbar buttons..."
	"  | External editors..."
	"  | File manager & web browser..."
	"  | Miscellaneous..."
	"%l| File types..."
	"  | Syntax highlighting..."
	"  | Macros..."
	"  | Clean paragraph margins rules..."
	"%l| Save window size as default";

enum
{
	kEditorPrefsCmd = 1, kToolBarPrefsCmd,
	kChooseExtEditorsCmd, kWWWPrefsCmd, kMiscPrefsCmd,
	kEditFileTypesCmd, kEditStylesSubmenuIndex,
	kEditMacrosCmd, kEditCRMRuleListsCmd,
	kSaveWindSizeCmd
};

// Syntax highlighting menu

static const JCharacter* kPrefsStylesMenuStr =
	"  Bourne shell"
	"| C shell"
	"| C / C++"
	"| C#"
	"| Eiffel"
	"| HTML / PHP / JSP / XML"
	"| INI"
	"| Java"
	"| Java properties"
	"| JavaScript"
	"| Perl"
	"| Python"
	"| Ruby"
	"| SQL"
	"| TCL";

static const CBLanguage kMenuItemToLang[] =
{
	kCBBourneShellLang,
	kCBCShellLang,
	kCBCLang,
	kCBCSharpLang,
	kCBEiffelLang,
	kCBHTMLLang,
	kCBINILang,
	kCBJavaLang,
	kCBPropertiesLang,
	kCBJavaScriptLang,
	kCBPerlLang,
	kCBPythonLang,
	kCBRubyLang,
	kCBSQLLang,
	kCBTCLLang
};

const JSize kMenuItemToLangCount = sizeof(kMenuItemToLang) / sizeof(CBLanguage);

// Settings popup menu

static const JCharacter* kSettingsMenuStr =
	"    Change spaces per tab..." 
	"%l| Auto-indent        %b"
	"  | Tab inserts spaces %b"
	"  | Show whitespace    %b"
	"  | Word wrap          %b"
	"  | Read only          %b";

enum
{
	kTabWidthCmd = 1,
	kToggleAutoIndentCmd,
	kToggleTabInsertsSpacesCmd,
	kToggleWhitespaceCmd,
	kToggleWordWrapCmd,
	kToggleReadOnlyCmd
};

// Action button

static const JCharacter* kHTMLButtonStr = "View";

// JBroadcaster message types

const JCharacter* CBTextDocument::kSaved = "Saved::CBTextDocument";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBTextDocument::CBTextDocument
	(
	const CBTextFileType	type,		// first to avoid conflict with fileName
	const JCharacter*		helpSectionName,
	const JBoolean			setWMClass,
	CBTextEditorCtorFn*		teCtorFn
	)
	:
	JXFileDocument(CBGetApplication(),
				   (CBGetDocumentManager())->GetNewFileName(),
				   kJFalse, kJTrue, ""),
	JPrefObject(CBGetPrefsManager(), kCBTextDocID),
	itsHelpSectionName(helpSectionName)
{
	CBTextDocumentX1(type);
	BuildWindow(setWMClass, teCtorFn);
	CBTextDocumentX2(kJTrue);
}

CBTextDocument::CBTextDocument
	(
	const JCharacter*		fileName,
	const CBTextFileType	type,
	const JBoolean			tmpl
	)
	:
	JXFileDocument(CBGetApplication(), fileName, !tmpl, kJTrue, ""),
	JPrefObject(CBGetPrefsManager(), kCBTextDocID),
	itsHelpSectionName("CBOverviewHelp")
{
	CBTextDocumentX1(type);
	BuildWindow(kJTrue, ConstructTextEditor);

	ReadFile(fileName, kJTrue);
	CBTextDocumentX2(kJTrue);

	if (tmpl)
		{
		JString path, name;
		JSplitPathAndName(fileName, &path, &name);
		FileChanged(name, kJFalse);
		}
}

CBTextDocument::CBTextDocument
	(
	std::istream&		input,
	const JFileVersion	vers,
	JBoolean*			keep
	)
	:
	JXFileDocument(CBGetApplication(), "", kJFalse, kJTrue, ""),
	JPrefObject(CBGetPrefsManager(), kCBTextDocID),
	itsHelpSectionName("CBOverviewHelp")
{
	CBTextDocumentX1(kCBUnknownFT);
	BuildWindow(kJTrue, ConstructTextEditor);
	*keep = ReadFromProject(input, vers);
	CBTextDocumentX2(kJFalse);

	JXDockWidget* dock;
	if (*keep && GetWindow()->GetDockWidget(&dock))
		{
		dock->Dock(GetWindow());	// sort to correct location
		}
}

// private

void
CBTextDocument::CBTextDocumentX1
	(
	const CBTextFileType type
	)
{
	ListenTo(this);

	itsOpenOverComplementFlag = kJFalse;
	itsFileFormat             = JTextEditor::kUNIXText;
	itsUpdateFileTypeFlag     = kJFalse;

	itsFileType    = type;
	itsActionMgr   = nullptr;
	itsMacroMgr    = nullptr;
	itsCRMRuleList = nullptr;

	itsTabWidthDialog = nullptr;

	for (JUnsignedOffset i=0; i<kSettingCount; i++)
		{
		itsOverrideFlag[i] = kJFalse;
		}

	(CBGetDocumentManager())->TextDocumentCreated(this);
}

void
CBTextDocument::CBTextDocumentX2
	(
	const JBoolean setWindowSize
	)
{
	itsTextEditor->SetPTPrinter(CBGetPTTextPrinter());
	itsTextEditor->SetPSPrinter(CBGetPSTextPrinter());

	// must do this after reading file

	itsUpdateFileTypeFlag = kJTrue;
	UpdateFileType(kJTrue);

	// must do this after reading prefs and calculating file type

	if (setWindowSize)
		{
		JPoint desktopLoc;
		JCoordinate w,h;
		if (GetWindowSize(&desktopLoc, &w, &h))
			{
			JXWindow* window = GetWindow();
			window->Place(desktopLoc.x, desktopLoc.y);
			window->SetSize(w,h);
			}
		}
}

// static private

CBTextEditor*
CBTextDocument::ConstructTextEditor
	(
	CBTextDocument*		document,
	const JCharacter*	fileName,
	JXMenuBar*			menuBar,
	CBTELineIndexInput*	lineInput,
	CBTEColIndexInput*	colInput,
	JXScrollbarSet*		scrollbarSet
	)
{
	CBTextEditor* te =
		jnew CBTextEditor(document, fileName, menuBar, lineInput, colInput,
						 scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( te != nullptr );

	return te;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBTextDocument::~CBTextDocument()
{
	(CBGetDocumentManager())->TextDocumentDeleted(this);
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
CBTextDocument::Activate()
{
	JXFileDocument::Activate();

	if (IsActive())
		{
		(CBGetDocumentManager())->SetActiveTextDocument(this);
		}
}

/******************************************************************************
 ConvertSelectionToFullPath (virtual)

	Check if the file is relative to our file's path.
	CBExecOutputDocument overrides this to try other tricks.

 ******************************************************************************/

void
CBTextDocument::ConvertSelectionToFullPath
	(
	JString* fileName
	)
	const
{
	if (JIsAbsolutePath(*fileName))
		{
		return;
		}

	if (ExistsOnDisk())
		{
		const JString testName = JCombinePathAndName(GetFilePath(), *fileName);
		if (JFileExists(testName))
			{
			*fileName = testName;
			}
		}
}

/******************************************************************************
 GetMenuIcon (virtual)

	Override of JXDocument::GetMenuIcon().

 ******************************************************************************/

JBoolean
CBTextDocument::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	CBExecOutputDocument* doc;
	*icon = CBGetTextFileIcon(JI2B((CBGetDocumentManager())->GetActiveListDocument(&doc) &&
								   doc == const_cast<CBTextDocument*>(this)));
	return kJTrue;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include <jx_file_new.xpm>
#include <jx_file_open.xpm>
#include <jx_file_save.xpm>
#include <jx_file_save_all.xpm>
#include <jx_file_revert_to_saved.xpm>
#include <jx_file_print.xpm>
#include <jx_edit_read_only.xpm>
#include <jx_show_whitespace.xpm>

#include "jcc_file_print_with_styles.xpm"
#include "jcc_file_save_copy_as.xpm"
#include "jcc_unix_format.xpm"
#include "jcc_mac_format.xpm"
#include "jcc_dos_format.xpm"
#include "jcc_compare_files.xpm"
#include "jcc_compare_files_1.xpm"
#include "jcc_compare_files_2.xpm"
#include "jcc_compare_backup.xpm"
#include "jcc_compare_vcs.xpm"
#include "jcc_compare_vcs_as.xpm"
#include "jcc_word_wrap.xpm"
#include "jcc_auto_indent.xpm"
#include "jcc_tab_inserts_spaces.xpm"

void
CBTextDocument::BuildWindow
	(
	const JBoolean		setWMClass,
	CBTextEditorCtorFn*	teCtorFn
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 550,550, JString::empty);
	assert( window != nullptr );

	itsFileDragSource =
		jnew CBFileDragSource(this, window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 150,530, 20,20);
	assert( itsFileDragSource != nullptr );

	itsMenuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 480,30);
	assert( itsMenuBar != nullptr );

	itsActionButton =
		jnew JXTextButton(JGetString("itsActionButton::CBTextDocument::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,0, 70,30);
	assert( itsActionButton != nullptr );

	itsToolBar =
		jnew JXToolBar(CBGetPrefsManager(), kCBTEToolBarID, itsMenuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 550,500);
	assert( itsToolBar != nullptr );

	JXDownRect* lineBorder =
		jnew JXDownRect(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 0,530, 80,20);
	assert( lineBorder != nullptr );

	JXStaticText* lineLabel =
		jnew JXStaticText(JGetString("lineLabel::CBTextDocument::JXLayout"), lineBorder,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 28,16);
	assert( lineLabel != nullptr );
	lineLabel->SetToLabel();

	CBTELineIndexInput* lineInput =
		jnew CBTELineIndexInput(lineLabel, lineBorder,
					JXWidget::kHElastic, JXWidget::kFixedTop, 28,0, 48,16);
	assert( lineInput != nullptr );

	JXDownRect* colBorder =
		jnew JXDownRect(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 80,530, 70,20);
	assert( colBorder != nullptr );

	JXStaticText* colLabel =
		jnew JXStaticText(JGetString("colLabel::CBTextDocument::JXLayout"), colBorder,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 28,16);
	assert( colLabel != nullptr );
	colLabel->SetToLabel();

	CBTEColIndexInput* colInput =
		jnew CBTEColIndexInput(lineInput, colLabel, colBorder,
					JXWidget::kHElastic, JXWidget::kFixedTop, 28,0, 38,16);
	assert( colInput != nullptr );

	itsFileDisplay =
		jnew CBFileNameDisplay(this, itsFileDragSource, window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 170,530, 295,20);
	assert( itsFileDisplay != nullptr );

	itsSettingsMenu =
		jnew JXTextMenu(JGetString("itsSettingsMenu::CBTextDocument::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 465,530, 85,20);
	assert( itsSettingsMenu != nullptr );

// end JXLayout

	AdjustWindowTitle();
	window->SetMinSize(300, 100);
	if (setWMClass)
		{
		window->SetWMClass(CBGetWMClassInstance(), CBGetEditorWindowClass());
		}
	window->ShouldFocusWhenShow(kJTrue);	// necessary for click-to-focus

	JPoint p = itsSettingsMenu->GetTitlePadding();
	p.y      = 0;
	itsSettingsMenu->SetTitlePadding(p);

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure();

	JBoolean onDisk;
	const JString fullName = GetFullName(&onDisk);
	DisplayFileName(fullName);

	itsTextEditor = teCtorFn(this, fullName, itsMenuBar, lineInput, colInput, scrollbarSet);
	itsTextEditor->FitToEnclosure();

	UpdateReadOnlyDisplay();

	ListenTo(itsTextEditor);
	ListenTo(itsActionButton);
	itsActionButton->SetHint(JGetString("ConfigButtonHint::CBTextDocument"));

	itsFileDisplay->SetTE(itsTextEditor);
	itsFileDisplay->ShareEditMenu(itsTextEditor);

	itsSettingsMenu->SetPopupArrowDirection(JXMenu::kArrowPointsUp);
	itsSettingsMenu->AdjustSize(
		window->GetFrameGlobal().right - itsSettingsMenu->GetFrameGlobal().right, 0);
	itsSettingsMenu->SetMenuItems(kSettingsMenuStr, "CBTextDocument");
	itsSettingsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsSettingsMenu);

	itsSettingsMenu->SetItemImage(kToggleAutoIndentCmd,       jcc_auto_indent);
	itsSettingsMenu->SetItemImage(kToggleTabInsertsSpacesCmd, jcc_tab_inserts_spaces);
	itsSettingsMenu->SetItemImage(kToggleWhitespaceCmd,       jx_show_whitespace);
	itsSettingsMenu->SetItemImage(kToggleWordWrapCmd,         jcc_word_wrap);
	itsSettingsMenu->SetItemImage(kToggleReadOnlyCmd,         jx_edit_read_only);

	itsFileMenu = itsMenuBar->PrependTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "CBTextDocument");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kNewTextEditorCmd, jx_file_new);
	itsFileMenu->SetItemImage(kOpenSomethingCmd, jx_file_open);
	itsFileMenu->SetItemImage(kSaveFileCmd,      jx_file_save);
	itsFileMenu->SetItemImage(kSaveCopyAsCmd,    jcc_file_save_copy_as);
	itsFileMenu->SetItemImage(kRevertCmd,        jx_file_revert_to_saved);
	itsFileMenu->SetItemImage(kSaveAllFilesCmd,  jx_file_save_all);
	itsFileMenu->SetItemImage(kDiffFilesCmd,     jcc_compare_files);
	itsFileMenu->SetItemImage(kDiffSmartCmd,     jcc_compare_backup);
	itsFileMenu->SetItemImage(kDiffVCSCmd,       jcc_compare_vcs);
	itsFileMenu->SetItemImage(kPrintPTCmd,       jx_file_print);
	itsFileMenu->SetItemImage(kPrintPSCmd,       jcc_file_print_with_styles);

	CBFileHistoryMenu* recentProjectMenu =
		jnew CBFileHistoryMenu(CBDocumentManager::kProjectFileHistory,
							  itsFileMenu, kRecentProjectMenuCmd, itsMenuBar);
	assert( recentProjectMenu != nullptr );

	CBFileHistoryMenu* recentTextMenu =
		jnew CBFileHistoryMenu(CBDocumentManager::kTextFileHistory,
							  itsFileMenu, kRecentTextMenuCmd, itsMenuBar);
	assert( recentTextMenu != nullptr );

	itsFileFormatMenu = jnew JXTextMenu(itsFileMenu, kFileFormatIndex, itsMenuBar);
	assert( itsFileFormatMenu != nullptr );
	itsFileFormatMenu->SetMenuItems(kFileFormatMenuStr, "CBTextDocument");
	itsFileFormatMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileFormatMenu);

	itsFileFormatMenu->SetItemImage(kUNIXFmtCmd, jcc_unix_format);
	itsFileFormatMenu->SetItemImage(kMacFmtCmd,  jcc_mac_format);
	itsFileFormatMenu->SetItemImage(kDOSFmtCmd,  jcc_dos_format);

	itsDiffMenu = jnew JXTextMenu(itsFileMenu, kDiffMenuIndex, itsMenuBar);
	assert( itsDiffMenu != nullptr );
	itsDiffMenu->SetMenuItems(kDiffMenuStr, "CBTextDocument");
	ListenTo(itsDiffMenu);

	itsDiffMenu->SetItemImage(kDiffAs1Cmd,   jcc_compare_files_1);
	itsDiffMenu->SetItemImage(kDiffAs2Cmd,   jcc_compare_files_2);
	itsDiffMenu->SetItemImage(kDiffAsVCSCmd, jcc_compare_vcs_as);

	itsCmdMenu =
		jnew CBCommandMenu(nullptr, this, itsMenuBar,
						  JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsCmdMenu != nullptr );
	itsMenuBar->AppendMenu(itsCmdMenu);

	itsWindowMenu =
		jnew CBDocumentMenu(kFileListMenuTitleStr, itsMenuBar,
						   JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsWindowMenu != nullptr );
	itsMenuBar->AppendMenu(itsWindowMenu);

	itsPrefsMenu = itsMenuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "CBTextDocument");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsPrefsStylesMenu = jnew JXTextMenu(itsPrefsMenu, kEditStylesSubmenuIndex, itsMenuBar);
	assert( itsPrefsStylesMenu != nullptr );
	itsPrefsStylesMenu->SetMenuItems(kPrefsStylesMenuStr, "CBTextDocument");
	itsPrefsStylesMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsStylesMenu);

	itsHelpMenu = (CBGetApplication())->CreateHelpMenu(itsMenuBar, "CBTextDocument");
	ListenTo(itsHelpMenu);

	// must do this after creating widgets

	JPrefObject::ReadPrefs();

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
		{
		JXTextMenu* editMenu;
		JBoolean ok = itsTextEditor->GetEditMenu(&editMenu);
		assert( ok );

		JIndex undo, redo, cut, copy, paste, left, right;
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kUndoCmd, &undo);
		assert( ok );
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kRedoCmd, &redo);
		assert( ok );
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kCutCmd, &cut);
		assert( ok );
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &copy);
		assert( ok );
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kPasteCmd, &paste);
		assert( ok );
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kShiftSelLeftCmd, &left);
		assert( ok );
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kShiftSelRightCmd, &right);
		assert( ok );

		itsToolBar->AppendButton(itsFileMenu, kNewTextEditorCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenSomethingCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kSaveFileCmd);
		itsToolBar->AppendButton(itsFileMenu, kSaveCopyAsCmd);
		itsToolBar->AppendButton(itsFileMenu, kSaveAllFilesCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kPrintPTCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(editMenu, undo);
		itsToolBar->AppendButton(editMenu, redo);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(editMenu, cut);
		itsToolBar->AppendButton(editMenu, copy);
		itsToolBar->AppendButton(editMenu, paste);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(editMenu, left);
		itsToolBar->AppendButton(editMenu, right);

		(CBGetApplication())->AppendHelpMenuToToolBar(itsToolBar, itsHelpMenu);
		}
}

/******************************************************************************
 InsertTextMenu

 ******************************************************************************/

JXTextMenu*
CBTextDocument::InsertTextMenu
	(
	const JCharacter* title
	)
{
	JIndex i;
	const JBoolean found = itsMenuBar->FindMenu(itsWindowMenu, &i);
	assert( found );
	return itsMenuBar->InsertTextMenu(i, title);
}

/******************************************************************************
 GetFileDisplayInfo

 ******************************************************************************/

JRect
CBTextDocument::GetFileDisplayInfo
	(
	JXWidget::HSizingOption* hSizing,
	JXWidget::VSizingOption* vSizing
	)
	const
{
	*hSizing = itsFileDisplay->GetHSizing();
	*vSizing = itsFileDisplay->GetVSizing();

	return JCovering(itsFileDragSource->GetFrame(), itsFileDisplay->GetFrame());
}

/******************************************************************************
 SetFileDisplayVisible

 ******************************************************************************/

void
CBTextDocument::SetFileDisplayVisible
	(
	const JBoolean show
	)
{
	itsFileDragSource->SetVisible(show);
	itsFileDisplay->SetVisible(show);
}

/******************************************************************************
 DisplayFileName (private)

	The file name is displayed at the bottom of the window.

 ******************************************************************************/

void
CBTextDocument::DisplayFileName
	(
	const JCharacter* name
	)
{
	if (ExistsOnDisk())
		{
		itsFileDisplay->SetText(name);
		}
	else
		{
		itsFileDisplay->SetText("");
		}

	itsFileDisplay->DiskCopyIsModified(kJFalse);
}

/******************************************************************************
 HandleFileModifiedByOthers (virtual protected)

	Display the file name in red.

 ******************************************************************************/

void
CBTextDocument::HandleFileModifiedByOthers
	(
	const JBoolean modTimeChanged,
	const JBoolean permsChanged
	)
{
	JXFileDocument::HandleFileModifiedByOthers(modTimeChanged, permsChanged);

	if (modTimeChanged)
		{
		itsFileDisplay->DiskCopyIsModified(kJTrue);
		}

	// if file becomes writable, update "Read only" setting
	// (Never automatically go the other way, because that is annoying!)

	JBoolean onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (onDisk && JFileWritable(fullName))
		{
		itsTextEditor->UpdateWritable(fullName);
		}
}

/******************************************************************************
 RefreshVCSStatus

 ******************************************************************************/

void
CBTextDocument::RefreshVCSStatus()
{
	JBoolean onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (onDisk)
		{
		itsTextEditor->UpdateWritable(fullName);
		}
}

/******************************************************************************
 Receive (virtual protected)

	Listen for menu update requests and menu selections.

 ******************************************************************************/

void
CBTextDocument::Receive
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
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsFileFormatMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileFormatMenu();
		}
	else if (sender == itsFileFormatMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileFormatMenu(selection->GetIndex());
		}

	else if (sender == itsDiffMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateDiffMenu();
		}
	else if (sender == itsDiffMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleDiffMenu(selection->GetIndex());
		}

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrefsMenu();
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandlePrefsMenu(selection->GetIndex());
		}

	else if (sender == itsPrefsStylesMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrefsStylesMenu();
		}
	else if (sender == itsPrefsStylesMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandlePrefsStylesMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		(CBGetApplication())->UpdateHelpMenu(itsHelpMenu);
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		(CBGetApplication())->HandleHelpMenu(itsHelpMenu, itsHelpSectionName,
											 selection->GetIndex());
		}

	else if (sender == itsSettingsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateSettingsMenu();
		}
	else if (sender == itsSettingsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleSettingsMenu(selection->GetIndex());
		}

	else if (sender == itsActionButton && message.Is(JXButton::kPushed))
		{
		HandleActionButton();
		}

	else if (sender == itsTextEditor && message.Is(JTextEditor::kTextChanged))
		{
		if (itsTextEditor->IsAtLastSaveLocation())
			{
			DataReverted(kJTrue);
			}
		else
			{
			DataModified();
			}
		(CBGetDocumentManager())->TextDocumentNeedsSave();
		}

	else if (sender == itsTextEditor && message.Is(JTextEditor::kTypeChanged))
		{
		UpdateReadOnlyDisplay();
		}

	else if (sender == itsTabWidthDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			OverrideTabWidth(itsTabWidthDialog->GetTabCharCount());
			}
		itsTabWidthDialog = nullptr;
		}

	else
		{
		if (sender == this && message.Is(JXFileDocument::kNameChanged))
			{
			const JXFileDocument::NameChanged* info =
				dynamic_cast<const JXFileDocument::NameChanged*>(&message);
			assert( info != nullptr );
			DisplayFileName(info->GetFullName());
			UpdateFileType();
			}

		JXFileDocument::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
CBTextDocument::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kSaveFileCmd, NeedsSave());
	itsFileMenu->SetItemEnable(kRevertCmd, CanRevert());
	itsFileMenu->SetItemEnable(kSaveAllFilesCmd,
							   (CBGetDocumentManager())->TextDocumentsNeedSave());

	JBoolean enable, onDisk;
	const JString fullName = GetFullName(&onDisk);
	itsFileMenu->SetItemText(kDiffSmartCmd,
		(CBGetDiffFileDialog())->GetSmartDiffItemText(onDisk, fullName, &enable));
	itsFileMenu->SetItemEnable(kDiffSmartCmd, enable);

	enable = kJFalse;
	const JString* s;
	if (onDisk)
		{
		const JVCSType type = JGetVCSType(GetFilePath());
		if (type == kJCVSType)
			{
			enable = kJTrue;
			s = & JGetString("DiffCVSMenuText::CBTextDocument");
			}
		else if (type == kJSVNType)
			{
			enable = kJTrue;
			s = & JGetString("DiffSVNMenuText::CBTextDocument");
			}
		else if (type == kJGitType)
			{
			enable = kJTrue;
			s = & JGetString("DiffGitMenuText::CBTextDocument");
			}
		else
			{
			enable = kJTrue;
			s = & JGetString("DiffVCSMenuText::CBTextDocument");
			}
		}
	else
		{
		s = & JGetString("DiffVCSMenuText::CBTextDocument");
		}
	itsFileMenu->SetItemText(kDiffVCSCmd, *s);
	itsFileMenu->SetItemEnable(kDiffVCSCmd, enable);

	const JBoolean hasText = !(itsTextEditor->GetText()).IsEmpty();
	itsFileMenu->SetItemEnable(kPrintPTCmd, hasText);

	const JBoolean isStyled = JI2B((itsTextEditor->GetStyles()).GetRunCount() > 1);
	itsFileMenu->SetItemEnable(kPSPageSetupCmd, isStyled);
	itsFileMenu->SetItemEnable(kPrintPSCmd, JI2B(hasText && isStyled));
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
CBTextDocument::HandleFileMenu
	(
	const JIndex index
	)
{
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
		if (OpenSomething())
			{
			return;		// we closed ourselves
			}
		}
	else if (index == kSaveFileCmd)
		{
		Save();
		}
	else if (index == kSaveFileAsCmd)
		{
		JBoolean onDisk;
		const JString fullName = GetFullName(&onDisk);
		(CBGetDocumentManager())->
			AddToFileHistoryMenu(CBDocumentManager::kTextFileHistory, fullName);

		SaveInNewFile();
		}
	else if (index == kSaveCopyAsCmd)
		{
		JString fullName;
		if (SaveCopyInNewFile(nullptr, &fullName))
			{
			(CBGetDocumentManager())->
				AddToFileHistoryMenu(CBDocumentManager::kTextFileHistory, fullName);
			}
		}
	else if (index == kSaveAsTemplateCmd)
		{
		JString origName;
		if ((CBGetDocumentManager())->GetTextTemplateDirectory(kJTrue, &origName))
			{
			origName = JCombinePathAndName(origName, GetFileName());
			SaveCopyInNewFile(origName);
			}
		}
	else if (index == kRevertCmd)
		{
		RevertToSaved();
		}
	else if (index == kSaveAllFilesCmd)
		{
		(CBGetDocumentManager())->SaveTextDocuments(kJTrue);
		}

	else if (index == kDiffFilesCmd)
		{
		(CBGetDiffFileDialog())->Activate();
		}
	else if (index == kDiffSmartCmd)
		{
		JBoolean onDisk;
		const JString fullName = GetFullName(&onDisk);
		if (onDisk)
			{
			Save();
			(CBGetDiffFileDialog())->ViewDiffs(onDisk, fullName);
			}
		}
	else if (index == kDiffVCSCmd)
		{
		JBoolean onDisk;
		const JString fullName = GetFullName(&onDisk);
		if (onDisk)
			{
			Save();
			(CBGetDiffFileDialog())->ViewVCSDiffs(fullName);
			}
		}

	else if (index == kShowInFileMgrCmd)
		{
		JBoolean onDisk;
		const JString fullName = GetFullName(&onDisk);
		if (onDisk)
			{
			(JXGetWebBrowser())->ShowFileLocation(fullName);
			}
		}

	else if (index == kPTPageSetupCmd)
		{
		itsTextEditor->HandlePTPageSetup();
		}
	else if (index == kPrintPTCmd)
		{
		JBoolean onDisk;
		const JString fullName = GetFullName(&onDisk);
		CBPTPrinter* p = CBGetPTTextPrinter();
		p->SetHeaderName(fullName);
		p->SetTabWidth(itsTextEditor->GetTabCharCount());
		itsTextEditor->PrintPT();
		}

	else if (index == kPSPageSetupCmd)
		{
		itsTextEditor->HandlePSPageSetup();
		}
	else if (index == kPrintPSCmd)
		{
		JBoolean onDisk;
		const JString fullName = GetFullName(&onDisk);
		(CBGetPSTextPrinter())->SetPrintInfo(itsTextEditor, fullName);
		itsTextEditor->PrintPS();
		}

	else if (index == kCloseCmd)
		{
		Close();
		}
	else if (index == kCloseAllCmd)
		{
		(CBGetDocumentManager())->CloseTextDocuments();
		}

	else if (index == kQuitCmd)
		{
		JXGetApplication()->Quit();
		}
}

/******************************************************************************
 OpenSomething (private)

	If "Open..." is invoked from an empty, unmodified document that is
	not on disk, and the user opens a text document, then we close ourselves.

	Returns kJTrue if we closed ourselves.

 ******************************************************************************/

JBoolean
CBTextDocument::OpenSomething()
{
	CBDocumentManager* docMgr = CBGetDocumentManager();
	const JSize textDocCount  = docMgr->GetTextDocumentCount();

	docMgr->OpenSomething();

	if (docMgr->GetTextDocumentCount() > textDocCount &&
		(itsTextEditor->GetText()).IsEmpty() &&
		!NeedsSave() && !ExistsOnDisk())
		{
		Close();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 DiscardChanges (virtual protected)

 ******************************************************************************/

void
CBTextDocument::DiscardChanges()
{
	JBoolean onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (onDisk)
		{
		// save location of caret and scrollbars

		JIndex caretIndex = itsTextEditor->GetInsertionIndex();
		const JRect ap    = itsTextEditor->GetAperture();

		// re-read file

		DataReverted();			// must come first since file may need cleaning
		ReadFile(fullName, kJFalse);
		itsTextEditor->UpdateWritable(fullName);

		// restore location of caret and scrollbars

		itsTextEditor->ScrollTo(ap.topLeft());	// this first to avoid 2 redraws

		const JSize textLength = itsTextEditor->GetTextLength();
		if (caretIndex > textLength + 1)
			{
			caretIndex = textLength + 1;
			}
		itsTextEditor->SetCaretLocation(caretIndex);
		}
	else
		{
		itsTextEditor->SetText("");
		DataReverted();
		}

	itsFileDisplay->DiskCopyIsModified(kJFalse);
}

/******************************************************************************
 OpenAsBinaryFile (static)

	Returns kJTrue if the file contains illegal characters and the user
	wants to open it as a binary file.

 ******************************************************************************/

JBoolean
CBTextDocument::OpenAsBinaryFile
	(
	const JCharacter* fileName
	)
{
	JString s;
	JReadFile(fileName, &s);

	if (CBTextEditor::ContainsIllegalChars(s))
		{
		const JCharacter* map[] =
			{
			"name", fileName
			};
		const JString msg = JGetString("OpenBinaryFileMessage::CBTextDocument", map, sizeof(map));
		return !JGetUserNotification()->AskUserNo(msg);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ReadFile (protected)

 ******************************************************************************/

void
CBTextDocument::ReadFile
	(
	const JCharacter*	fileName,
	const JBoolean		firstTime
	)
{
	if (!itsTextEditor->ReadPlainText(fileName, &itsFileFormat))
		{
		// Don't let them overwrite it without thinking about it.

		const JString name = JGetString("BinaryFilePrefix::CBTextDocument") + GetFileName();
		FileChanged(name, kJFalse);

		// Display the full path of the original file.
		// (FileChanged() changes the display.)

		JString fullName;
		const JBoolean ok = JGetTrueName(fileName, &fullName);
		assert( ok );
		DisplayFileName(fullName);
		itsTextEditor->UpdateWritable(fullName);
		}

	JBoolean setTabWidth, setTabMode, tabInsertsSpaces, setAutoIndent, autoIndent;
	JSize tabWidth;
	CBMParseEditorOptions(fileName, itsTextEditor->GetText(), &setTabWidth, &tabWidth,
						  &setTabMode, &tabInsertsSpaces, &setAutoIndent, &autoIndent);

	if (setTabWidth)
		{
		OverrideTabWidth(tabWidth);
		}
	if (setTabMode)
		{
		OverrideTabInsertsSpaces(tabInsertsSpaces);
		}
	if (setAutoIndent)
		{
		OverrideAutoIndent(autoIndent);
		}

	if (firstTime)
		{
		const JBoolean spaces = itsTextEditor->TabInsertsSpaces();

		JSize tabWidth = itsTextEditor->GetTabCharCount();
		itsTextEditor->AnalyzeWhitespace(&tabWidth);

		if (setTabMode)
			{
			itsTextEditor->TabShouldInsertSpaces(spaces);
			}

		if (!setTabWidth)
			{
			itsTextEditor->SetTabCharCount(tabWidth);
			}

		JPtrArray<JString> safetyFilesToOpen(JPtrArrayT::kDeleteAll);
		if (CheckForSafetySaveFiles(fileName, &safetyFilesToOpen))
			{
			const JSize count = safetyFilesToOpen.GetElementCount();
			for (JIndex i=1; i<=count; i++)
				{
				(CBGetDocumentManager())->
					OpenTextDocument(*(safetyFilesToOpen.GetElement(i)));
				}
			}
		}

	UpdateFileType();
	itsTextEditor->SetLastSaveLocation();
}

/******************************************************************************
 WriteTextFile (virtual protected)

 ******************************************************************************/

void
CBTextDocument::WriteTextFile
	(
	std::ostream&		output,
	const JBoolean	safetySave
	)
	const
{
	itsTextEditor->WritePlainText(output, itsFileFormat);
	if (!safetySave)
		{
		itsTextEditor->DeactivateCurrentUndo();

		if (output.good())
			{
			itsTextEditor->SetLastSaveLocation();

			const_cast<CBTextDocument*>(this)->UpdateFileType();

			itsFileDisplay->DiskCopyIsModified(kJFalse);

			const_cast<CBTextDocument*>(this)->Broadcast(Saved());
			}
		}
}

/******************************************************************************
 ReadFromProject (private)

	Returns kJFalse if it cannot find the file.

 ******************************************************************************/

JBoolean
CBTextDocument::ReadFromProject
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	JBoolean onDisk;
	input >> onDisk;
	if (!onDisk)
		{
		return kJFalse;
		}

	JString fullName, ptPrintName, psPrintName;
	input >> fullName;
	if (vers >= 29)
		{
		input >> ptPrintName >> psPrintName;
		}

	GetWindow()->ReadGeometry(input);

	JIndex insertionIndex;
	input >> insertionIndex;

	JBoolean autoIndent, tabInsertsSpaces, showWS, wordWrap, readOnly;
	JSize tabCharCount;
	if (vers >= 68)
		{
		input >> itsOverrideFlag[ kAutoIndentIndex ]     >> autoIndent;
		input >> itsOverrideFlag[ kShowWhitespaceIndex ] >> showWS;
		input >> itsOverrideFlag[ kWordWrapIndex ]       >> wordWrap;
		input >> itsOverrideFlag[ kReadOnlyIndex ]       >> readOnly;
		}
	if (vers >= 69)
		{
		input >> itsOverrideFlag[ kTabWidthIndex ] >> tabCharCount;
		}
	if (70 <= vers && vers < 84)
		{
		JBoolean override;
		JString charSet;
		input >> override >> charSet;
		}
	if (vers >= 82)
		{
		input >> itsOverrideFlag[ kTabInsertsSpacesIndex ] >> tabInsertsSpaces;
		}

	JXFileDocument* existingDoc;
	if (JFileReadable(fullName) &&
		!(JXGetDocumentManager())->FileDocumentIsOpen(fullName, &existingDoc))
		{
		FileChanged(fullName, kJTrue);
		ReadFile(fullName, kJTrue);

		itsTextEditor->SetPTPrintFileName(ptPrintName);
		itsTextEditor->SetPSPrintFileName(psPrintName);

		if (itsOverrideFlag[ kAutoIndentIndex ])
			{
			itsTextEditor->ShouldAutoIndent(autoIndent);
			}
		if (itsOverrideFlag[ kTabInsertsSpacesIndex ])
			{
			itsTextEditor->TabShouldInsertSpaces(tabInsertsSpaces);
			}
		if (itsOverrideFlag[ kShowWhitespaceIndex ])
			{
			itsTextEditor->ShouldShowWhitespace(showWS);
			}
		if (itsOverrideFlag[ kWordWrapIndex ])
			{
			itsTextEditor->SetBreakCROnly(wordWrap);
			}
		if (itsOverrideFlag[ kReadOnlyIndex ])
			{
			itsTextEditor->SetWritable(!readOnly);
			}
		if (itsOverrideFlag[ kTabWidthIndex ])
			{
			itsTextEditor->SetTabCharCount(tabCharCount);
			}

		if (vers >= 22)
			{
			itsTextEditor->ReadScrollSetup(input);
			}
		else if (vers >= 18)
			{
			JXScrollbar *hScrollbar, *vScrollbar;
			const JBoolean ok = itsTextEditor->GetScrollbars(&hScrollbar, &vScrollbar);
			assert( ok );
			vScrollbar->ReadSetup(input);
			}

		const JIndex lineIndex = itsTextEditor->GetLineForChar(insertionIndex);
		if (lineIndex > 1)
			{
			itsTextEditor->GoToLine(lineIndex);
			}
		return kJTrue;
		}
	else
		{
		if (vers >= 22)
			{
			JXScrollableWidget::SkipScrollSetup(input);
			}
		else if (vers >= 18)
			{
			JXScrollbar::SkipSetup(input);
			}

		return kJFalse;
		}
}

/******************************************************************************
 WriteForProject

 ******************************************************************************/

void
CBTextDocument::WriteForProject
	(
	std::ostream& output
	)
	const
{
	JBoolean onDisk;
	const JString fullName = GetFullName(&onDisk);
	output << onDisk;

	if (onDisk)
		{
		output << ' ' << fullName;
		output << ' ' << itsTextEditor->GetPTPrintFileName();
		output << ' ' << itsTextEditor->GetPSPrintFileName();

		output << ' ';
		GetWindow()->WriteGeometry(output);

		output << ' ' << itsTextEditor->GetInsertionIndex();

		output << ' ' << itsOverrideFlag[ kAutoIndentIndex ];
		output << ' ' << itsTextEditor->WillAutoIndent();

		output << ' ' << itsOverrideFlag[ kShowWhitespaceIndex ];
		output << ' ' << itsTextEditor->WillShowWhitespace();

		output << ' ' << itsOverrideFlag[ kWordWrapIndex ];
		output << ' ' << itsTextEditor->WillBreakCROnly();

		output << ' ' << itsOverrideFlag[ kReadOnlyIndex ];
		output << ' ' << itsTextEditor->IsReadOnly();

		output << ' ' << itsOverrideFlag[ kTabWidthIndex ];
		output << ' ' << itsTextEditor->GetTabCharCount();

		output << ' ' << itsOverrideFlag[ kTabInsertsSpacesIndex ];
		output << ' ' << itsTextEditor->TabInsertsSpaces();

		output << ' ';
		itsTextEditor->WriteScrollSetup(output);
		}
}

/******************************************************************************
 ReadStaticGlobalPrefs (static)

 ******************************************************************************/

void
CBTextDocument::ReadStaticGlobalPrefs
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	if (vers < 25)
		{
		JBoolean copyWhenSelect;
		input >> copyWhenSelect;

		if ((JXGetSharedPrefsManager())->WasNew())
			{
			JTextEditor::ShouldCopyWhenSelect(copyWhenSelect);
			}
		}

	if (21 <= vers && vers < 25)
		{
		JBoolean windowsHomeEnd, scrollCaret;
		input >> windowsHomeEnd >> scrollCaret;

		if ((JXGetSharedPrefsManager())->WasNew())
			{
			JXTEBase::ShouldUseWindowsHomeEnd(windowsHomeEnd);
			JXTEBase::CaretShouldFollowScroll(scrollCaret);
			}
		}

	if (vers >= 17)
		{
		JBoolean askOKToClose;
		input >> askOKToClose;
		JXFileDocument::ShouldAskOKToClose(askOKToClose);
		}
}

/******************************************************************************
 WriteStaticGlobalPrefs (static)

 ******************************************************************************/

void
CBTextDocument::WriteStaticGlobalPrefs
	(
	std::ostream& output
	)
{
	output << ' ' << JXFileDocument::WillAskOKToClose();
}

/******************************************************************************
 UpdateFileFormatMenu (private)

 ******************************************************************************/

void
CBTextDocument::UpdateFileFormatMenu()
{
	if (itsFileFormat == JTextEditor::kUNIXText)
		{
		itsFileFormatMenu->CheckItem(kUNIXFmtCmd);
		}
	else if (itsFileFormat == JTextEditor::kMacintoshText)
		{
		itsFileFormatMenu->CheckItem(kMacFmtCmd);
		}
	else if (itsFileFormat == JTextEditor::kDOSText)
		{
		itsFileFormatMenu->CheckItem(kDOSFmtCmd);
		}
}

/******************************************************************************
 HandleFileFormatMenu (private)

 ******************************************************************************/

void
CBTextDocument::HandleFileFormatMenu
	(
	const JIndex index
	)
{
	const JTextEditor::PlainTextFormat origFormat = itsFileFormat;

	if (index == kUNIXFmtCmd)
		{
		itsFileFormat = JTextEditor::kUNIXText;
		}
	else if (index == kMacFmtCmd)
		{
		itsFileFormat = JTextEditor::kMacintoshText;
		}
	else if (index == kDOSFmtCmd)
		{
		itsFileFormat = JTextEditor::kDOSText;
		}

	if (itsFileFormat != origFormat)
		{
		itsTextEditor->ClearLastSaveLocation();
		DataModified();
		}
}

/******************************************************************************
 UpdateDiffMenu (private)

 ******************************************************************************/

void
CBTextDocument::UpdateDiffMenu()
{
	JBoolean onDisk;
	const JString fullName = GetFullName(&onDisk);

	JBoolean enable = kJFalse;
	JString s       = "...";
	if (onDisk)
		{
		itsDiffMenu->EnableItem(kDiffAs1Cmd);
		itsDiffMenu->EnableItem(kDiffAs2Cmd);

		const JVCSType type = JGetVCSType(GetFilePath());
		if (type == kJCVSType)
			{
			enable = kJTrue;
			s.Prepend(JGetString("DiffCVSMenuText::CBTextDocument"));
			}
		else if (type == kJSVNType)
			{
			enable = kJTrue;
			s.Prepend(JGetString("DiffSVNMenuText::CBTextDocument"));
			}
		else if (type == kJGitType)
			{
			enable = kJTrue;
			s.Prepend(JGetString("DiffGitMenuText::CBTextDocument"));
			}
		else
			{
			s.Prepend(JGetString("DiffVCSMenuText::CBTextDocument"));
			}
		}
	else
		{
		s.Prepend(JGetString("DiffVCSMenuText::CBTextDocument"));
		}

	itsDiffMenu->SetItemText(kDiffAsVCSCmd, s);
	itsDiffMenu->SetItemEnable(kDiffAsVCSCmd, enable);
}

/******************************************************************************
 HandleDiffMenu (private)

 ******************************************************************************/

void
CBTextDocument::HandleDiffMenu
	(
	const JIndex index
	)
{
	JBoolean onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (!onDisk)
		{
		return;
		}

	Save();

	if (index == kDiffAs1Cmd)
		{
		(CBGetDiffFileDialog())->SetFile1(fullName);
		}
	else if (index == kDiffAs2Cmd)
		{
		(CBGetDiffFileDialog())->SetFile2(fullName);
		}

	else if (index == kDiffAsVCSCmd)
		{
		const JVCSType type = JGetVCSType(GetFilePath());
		if (type == kJCVSType)
			{
			(CBGetDiffFileDialog())->SetCVSFile(fullName);
			}
		else if (type == kJSVNType)
			{
			(CBGetDiffFileDialog())->SetSVNFile(fullName);
			}
		else if (type == kJGitType)
			{
			(CBGetDiffFileDialog())->SetGitFile(fullName);
			}
		}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
CBTextDocument::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
CBTextDocument::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kEditorPrefsCmd)
		{
		EditPrefs();
		}
	else if (index == kToolBarPrefsCmd)
		{
		itsToolBar->Edit();
		}
	else if (index == kChooseExtEditorsCmd)
		{
		(CBGetDocumentManager())->ChooseEditors();
		}
	else if (index == kWWWPrefsCmd)
		{
		(JXGetWebBrowser())->EditPrefs();
		}
	else if (index == kMiscPrefsCmd)
		{
		(CBGetApplication())->EditMiscPrefs();
		}

	else if (index == kEditFileTypesCmd)
		{
		CBGetPrefsManager()->EditFileTypes();
		}
	else if (index == kEditMacrosCmd)
		{
		CBGetPrefsManager()->EditMacros(itsMacroMgr);
		}
	else if (index == kEditCRMRuleListsCmd)
		{
		CBGetPrefsManager()->EditCRMRuleLists(itsCRMRuleList);
		}

	else if (index == kSaveWindSizeCmd)
		{
		SaveWindowSize();
		}
}

/******************************************************************************
 UpdatePrefsStylesMenu (private)

 ******************************************************************************/

void
CBTextDocument::UpdatePrefsStylesMenu()
{
}

/******************************************************************************
 HandlePrefsStylesMenu (private)

 ******************************************************************************/

void
CBTextDocument::HandlePrefsStylesMenu
	(
	const JIndex index
	)
{
	CBStylerBase* styler;
	if (CBGetStyler(kMenuItemToLang[index-1], &styler))
		{
		styler->EditStyles();
		}
}

/******************************************************************************
 UpdateSettingsMenu (private)

 ******************************************************************************/

void
CBTextDocument::UpdateSettingsMenu()
{
	const JString tabWidth(itsTextEditor->GetTabCharCount(), JString::kBase10);

	const JCharacter* map[] =
	{
		"n", tabWidth.GetCString()
	};
	const JString text = JGetString("ChangeSpacesPerTabMenuItem::CBTextDocument", map, sizeof(map));
	itsSettingsMenu->SetItemText(kTabWidthCmd, text);

	if (itsTextEditor->WillAutoIndent())
		{
		itsSettingsMenu->CheckItem(kToggleAutoIndentCmd);
		}

	if (itsTextEditor->TabInsertsSpaces())
		{
		itsSettingsMenu->CheckItem(kToggleTabInsertsSpacesCmd);
		}

	if (itsTextEditor->WillShowWhitespace())
		{
		itsSettingsMenu->CheckItem(kToggleWhitespaceCmd);
		}

	if (!itsTextEditor->WillBreakCROnly())
		{
		itsSettingsMenu->CheckItem(kToggleWordWrapCmd);
		}

	if (itsTextEditor->IsReadOnly())
		{
		itsSettingsMenu->CheckItem(kToggleReadOnlyCmd);
		}
}

/******************************************************************************
 HandleSettingsMenu (private)

 ******************************************************************************/

void
CBTextDocument::HandleSettingsMenu
	(
	const JIndex index
	)
{
	if (index == kTabWidthCmd)
		{
		assert( itsTabWidthDialog == nullptr );
		itsTabWidthDialog = jnew CBTabWidthDialog(this, itsTextEditor->GetTabCharCount());
		assert( itsTabWidthDialog != nullptr );
		itsTabWidthDialog->BeginDialog();
		ListenTo(itsTabWidthDialog);
		}

	else if (index == kToggleAutoIndentCmd)
		{
		OverrideAutoIndent( !itsTextEditor->WillAutoIndent() );
		}
	else if (index == kToggleTabInsertsSpacesCmd)
		{
		OverrideTabInsertsSpaces( !itsTextEditor->TabInsertsSpaces() );
		}
	else if (index == kToggleWhitespaceCmd)
		{
		OverrideShowWhitespace( !itsTextEditor->WillShowWhitespace() );
		}
	else if (index == kToggleWordWrapCmd)
		{
		OverrideBreakCROnly( !itsTextEditor->WillBreakCROnly() );
		}
	else if (index == kToggleReadOnlyCmd)
		{
		OverrideReadOnly( !itsTextEditor->IsReadOnly() );
		}
}

/******************************************************************************
 OverrideTabWidth

 ******************************************************************************/

void
CBTextDocument::OverrideTabWidth
	(
	const JSize tabWidth
	)
{
	itsOverrideFlag[ kTabWidthIndex ] = kJTrue;
	itsTextEditor->SetTabCharCount(tabWidth);
}

/******************************************************************************
 OverrideAutoIndent

 ******************************************************************************/

void
CBTextDocument::OverrideAutoIndent
	(
	const JBoolean autoIndent
	)
{
	itsOverrideFlag[ kAutoIndentIndex ] = kJTrue;
	itsTextEditor->ShouldAutoIndent(autoIndent);
}

/******************************************************************************
 OverrideTabInsertsSpaces

 ******************************************************************************/

void
CBTextDocument::OverrideTabInsertsSpaces
	(
	const JBoolean insertSpaces
	)
{
	itsOverrideFlag[ kTabInsertsSpacesIndex ] = kJTrue;
	itsTextEditor->TabShouldInsertSpaces(insertSpaces);
}

/******************************************************************************
 OverrideShowWhitespace

 ******************************************************************************/

void
CBTextDocument::OverrideShowWhitespace
	(
	const JBoolean showWhitespace
	)
{
	itsOverrideFlag[ kShowWhitespaceIndex ] = kJTrue;
	itsTextEditor->ShouldShowWhitespace(showWhitespace);
}

/******************************************************************************
 OverrideBreakCROnly

 ******************************************************************************/

void
CBTextDocument::OverrideBreakCROnly
	(
	const JBoolean breakCROnly
	)
{
	itsOverrideFlag[ kWordWrapIndex ] = kJTrue;
	itsTextEditor->SetBreakCROnly(breakCROnly);
}

/******************************************************************************
 OverrideReadOnly

 ******************************************************************************/

void
CBTextDocument::OverrideReadOnly
	(
	const JBoolean readOnly
	)
{
	itsOverrideFlag[ kReadOnlyIndex ] = kJTrue;
	itsTextEditor->SetWritable(!readOnly);
}

/******************************************************************************
 GoToLine

 ******************************************************************************/

void
CBTextDocument::GoToLine
	(
	const JIndex lineIndex
	)
	const
{
	itsTextEditor->GoToLine(lineIndex);
}

/******************************************************************************
 SelectLines

 ******************************************************************************/

void
CBTextDocument::SelectLines
	(
	const JIndexRange& range
	)
	const
{
	CBMSelectLines(itsTextEditor, range);
}

/******************************************************************************
 UpdateFileType

 ******************************************************************************/

void
CBTextDocument::UpdateFileType
	(
	const JBoolean init
	)
{
	if (!itsUpdateFileTypeFlag)
		{
		return;
		}

	const CBTextFileType origType = itsFileType;

	CBStylerBase* origStyler;
	CBGetStyler(itsFileType, &origStyler);

	JString scriptPath;
	JBoolean wordWrap;
	itsFileType =
		CBGetPrefsManager()->GetFileType(*this, &itsActionMgr, &itsMacroMgr,
										 &itsCRMRuleList, &scriptPath, &wordWrap);

	if ((!init && itsFileType != origType) || !itsOverrideFlag[ kWordWrapIndex ])
		{
		itsTextEditor->SetBreakCROnly(!wordWrap);
		}
	itsTextEditor->SetCRMRuleList(itsCRMRuleList, kJFalse);
	itsTextEditor->SetScriptPath(scriptPath);

	CBStylerBase* newStyler;
	CBGetStyler(itsFileType, &newStyler);
	if (newStyler != origStyler)
		{
		itsTextEditor->RecalcStyles();
		}

	itsTextEditor->FileTypeChanged(itsFileType);

	// action button

	if (CBHasComplementType(itsFileType) ||
		itsFileType == kCBHTMLFT         ||
		itsFileType == kCBXMLFT)
		{
		if (!itsActionButton->WouldBeVisible())
			{
			itsActionButton->Show();
			itsMenuBar->AdjustSize(-itsActionButton->GetFrameWidth(), 0);
			}

		if (itsFileType == kCBHTMLFT || itsFileType == kCBXMLFT)
			{
			itsActionButton->SetLabel(kHTMLButtonStr);
			}
		else
			{
			itsActionButton->SetLabel(CBGetComplementFileTypeName(itsFileType));
			}
		}

	else if (itsActionButton->WouldBeVisible())
		{
		itsActionButton->Hide();
		itsMenuBar->AdjustSize(itsActionButton->GetFrameWidth(), 0);
		}
}

/******************************************************************************
 StylerChanged

 ******************************************************************************/

void
CBTextDocument::StylerChanged
	(
	JTEStyler* styler
	)
{
	CBStylerBase* myStyler;
	CBGetStyler(itsFileType, &myStyler);
	if (styler == myStyler)
		{
		itsTextEditor->RecalcStyles();
		}
}

/******************************************************************************
 GetStyler

	Returns the styler to be used for this document, if any.  Not inline
	to avoid including JTEStyler.h.

 ******************************************************************************/

JBoolean
CBTextDocument::GetStyler
	(
	CBStylerBase** styler
	)
	const
{
	return JI2B( CBGetStyler(itsFileType, styler) && (**styler).IsActive() );
}

/******************************************************************************
 UpdateReadOnlyDisplay (private)

 ******************************************************************************/

void
CBTextDocument::UpdateReadOnlyDisplay()
{
	itsFileDragSource->SetBackColor(
		itsTextEditor->GetType() == JTextEditor::kFullEditor ?
		GetColormap()->GetDefaultBackColor() :
		GetColormap()->GetDarkRedColor());
}

/******************************************************************************
 HandleActionButton

 ******************************************************************************/

void
CBTextDocument::HandleActionButton()
{
	if (itsActionButton->IsVisible())
		{
		JBoolean onDisk;
		const JString fullName = GetFullName(&onDisk);

		if ((itsFileType == kCBHTMLFT || itsFileType == kCBXMLFT) && Save())
			{
			(JXGetWebBrowser())->ShowFileContent(fullName);
			}
		else
			{
			(CBGetDocumentManager())->OpenComplementFile(fullName, itsFileType);
			}
		}
}

/******************************************************************************
 GetWindowSize

	This cannot be virtual because it is called by the constructor.

 ******************************************************************************/

JBoolean
CBTextDocument::GetWindowSize
	(
	JPoint*			desktopLoc,
	JCoordinate*	width,
	JCoordinate*	height
	)
	const
{
	CBPrefsManager* prefsMgr = CBGetPrefsManager();
	if (itsFileType == kCBExecOutputFT || itsFileType == kCBShellOutputFT)
		{
		return prefsMgr->GetWindowSize(kCBExecOutputWindSizeID, desktopLoc, width, height);
		}
	else if (itsFileType == kCBSearchOutputFT)
		{
		return prefsMgr->GetWindowSize(kCBSearchOutputWindSizeID, desktopLoc, width, height);
		}

	JBoolean onDisk;
	const JString thisFile = GetFullName(&onDisk);

	CBDocumentManager* docMgr = CBGetDocumentManager();

	JXFileDocument* doc;
	if (itsOpenOverComplementFlag &&
		docMgr->GetOpenComplementFile(thisFile, itsFileType, &doc))
		{
		JXWindow* window = doc->GetWindow();
		*desktopLoc      = window->GetDesktopLocation();
		*width           = window->GetFrameWidth();
		*height          = window->GetFrameHeight();
		return kJTrue;
		}

	else if (CBUseCSourceWindowSize(itsFileType))
		{
		return prefsMgr->GetWindowSize(kCBSourceWindSizeID, desktopLoc, width, height);
		}
	else if (CBIsHeaderType(itsFileType))
		{
		return prefsMgr->GetWindowSize(kCBHeaderWindSizeID, desktopLoc, width, height);
		}
	else
		{
		return prefsMgr->GetWindowSize(kCBOtherTextWindSizeID, desktopLoc, width, height);
		}
}

/******************************************************************************
 SaveWindowSize

 ******************************************************************************/

void
CBTextDocument::SaveWindowSize()
	const
{
	JXWindow* window = GetWindow();
	assert( window != nullptr );

	CBPrefsManager* prefsMgr = CBGetPrefsManager();

	if (itsFileType == kCBExecOutputFT || itsFileType == kCBShellOutputFT)
		{
		prefsMgr->SaveWindowSize(kCBExecOutputWindSizeID, window);
		}
	else if (itsFileType == kCBSearchOutputFT)
		{
		prefsMgr->SaveWindowSize(kCBSearchOutputWindSizeID, window);
		}
	else if (CBUseCSourceWindowSize(itsFileType))
		{
		prefsMgr->SaveWindowSize(kCBSourceWindSizeID, window);
		}
	else if (CBIsHeaderType(itsFileType))
		{
		prefsMgr->SaveWindowSize(kCBHeaderWindSizeID, window);
		}
	else
		{
		prefsMgr->SaveWindowSize(kCBOtherTextWindSizeID, window);
		}
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
CBTextDocument::EditPrefs()
{
	CBEditTextPrefsDialog* dlog = jnew CBEditTextPrefsDialog(this);
	assert( dlog != nullptr );
	dlog->BeginDialog();
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
CBTextDocument::ReadPrefs
	(
	std::istream& input
	)
{
	assert( itsTextEditor != nullptr );

	JFileVersion vers;
	input >> vers;

	if (vers == 0)
		{
		JBoolean openOverComplement;
		input >> JBoolFromString(openOverComplement);
		ShouldOpenComplFileOnTop(openOverComplement);
		}
	else if (vers == 1)
		{
		JBoolean breakCodeCROnly, openOverComplement;
		input >> JBoolFromString(breakCodeCROnly) >> JBoolFromString(openOverComplement);
		ShouldOpenComplFileOnTop(openOverComplement);
		}
	else if (vers <= kCurrentSetupVersion)
		{
		JBoolean openOverComplement;
		input >> JBoolFromString(openOverComplement);
		ShouldOpenComplFileOnTop(openOverComplement);
		}

	JIgnoreUntil(input, kSetupDataEndDelimiter);

	ReadJXFDSetup(input);
	itsTextEditor->ReadSetup(input);
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
CBTextDocument::WritePrefs
	(
	std::ostream& output
	)
	const
{
	assert( itsTextEditor != nullptr );

	output << kCurrentSetupVersion;
	output << ' ' << JBoolToString(itsOpenOverComplementFlag);
	output << kSetupDataEndDelimiter;

	WriteJXFDSetup(output);
	itsTextEditor->WriteSetup(output);
}
