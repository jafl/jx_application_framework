/******************************************************************************
 CMSourceDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "CMSourceDirector.h"
#include "CMLineNumberTable.h"
#include "CMLineAddressTable.h"
#include "CMSourceText.h"
#include "CMCommandDirector.h"
#include "CMFileDragSource.h"
#include "CBTextFileType.h"
#include "CBFnMenuUpdater.h"
#include "CBFunctionMenu.h"
#include "CMEditCommandsDialog.h"
#include "CMDisplaySourceForMain.h"
#include "CMClearSourceDisplayTask.h"
#include "CMGetAssembly.h"
#include "CMStackDir.h"
#include "cbmUtil.h"
#include "cmGlobals.h"
#include "cmActionDefs.h"

#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXScrollbarSet.h>
#include <JXFileNameDisplay.h>
#include <JXToolBar.h>
#include <JXHelpManager.h>
#include <JXWDManager.h>
#include <JXImage.h>
#include <JXColorManager.h>
#include <JXMacWinPrefsDialog.h>
#include <JXCloseDirectorTask.h>

#include <JFontManager.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jAssert.h>

// File menu

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"    Open source file...      %k Meta-O       %i" kCMOpenSourceFileAction
	"  | Open this file in editor %k Meta-Shift-O %i" kCMEditSourceFileAction
	"%l| Load configuration...    %k Ctrl-O       %i" kCMLoadConfigAction
	"  | Save configuration...    %k Ctrl-S       %i" kCMSaveConfigAction
	"%l| Page setup...                            %i" kJXPageSetupAction
	"  | Print...                 %k Meta-P       %i" kJXPrintAction
	"%l| Close                    %k Meta-W       %i" kJXCloseWindowAction
	"  | Quit                     %k Meta-Q       %i" kJXQuitAction;

enum
{
	kOpenCmd = 1,
	kEditCmd,
	kLoadConfigCmd,
	kSaveConfigCmd,
	kPageSetupCmd,
	kPrintCmd,
	kCloseCmd,
	kQuitCmd
};

// Prefs menu

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
	"    gdb %r"
	"  | lldb %r"
	"  | Java %r"
	"  | Xdebug %r"
	"%l| Preferences..."
	"  | Toolbar buttons..."
	"  | Custom commands..."
	"  | Mac/Win/X emulation..."
	"%l| Save window size as default";

enum
{
	kUseGDBCmd = 1,
	kUseLLDBCmd,
	kUseJavaCmd,
	kUseXdebugCmd,
	kEditPrefsCmd,
	kEditToolBarCmd,
	kEditCmdsCmd,
	kEditMacWinPrefsCmd,
	kWindowSizeCmd	// must be last item (see menu creation code)
};

static const JIndex kDebuggerTypeToMenuIndex[] =
{
	kUseGDBCmd,
	kUseXdebugCmd,
	kUseJavaCmd,
	kUseLLDBCmd
};

// Help menu

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"    About"
	"%l| Table of Contents       %i" kJXHelpTOCAction
	"  | Overview"
	"  | This window       %k F1 %i" kJXHelpSpecificAction
	"%l| Changes"
	"  | Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kOverviewCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

CMSourceDirector*
CMSourceDirector::Create
	(
	CMCommandDirector*	commandDir,
	const JCharacter*	fileOrFn,
	const Type			type
	)
{
	CMSourceDirector* dir = jnew CMSourceDirector(commandDir, fileOrFn, type);
	assert( dir != NULL );
	dir->CreateWindowsMenu();
	return dir;
}

/******************************************************************************
 Constructor

 ******************************************************************************/

CMSourceDirector::CMSourceDirector
	(
	CMCommandDirector*	commandDir,
	const Type			type
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsType(type),
	itsSrcMainCmd(NULL),
	itsGetAssemblyCmd(NULL)
{
	assert( IsMainSourceWindow() );
	CMSourceViewDirectorX(commandDir);

	if (itsType == kMainSourceType)
		{
		itsSrcMainCmd = itsLink->CreateDisplaySourceForMain(this);
		}
	else if (itsType == kMainAsmType)
		{
		UpdateFileType();	// only need to do this once
		}
}

// protected

CMSourceDirector::CMSourceDirector
	(
	CMCommandDirector*	commandDir,
	const JCharacter*	fileOrFn,
	const Type			type
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsType(type),
	itsSrcMainCmd(NULL),
	itsGetAssemblyCmd(NULL)
{
	CMSourceViewDirectorX(commandDir);

	if (itsType == kAsmType && !JString::IsEmpty(fileOrFn))
		{
		CMLocation loc;
		loc.SetFunctionName(fileOrFn);
		loc.SetMemoryAddress("0x0");	// not allowed to be null
		DisplayDisassembly(loc);
		}
	else if (itsType == kSourceType && JFileReadable(fileOrFn))
		{
		DisplayFile(fileOrFn);
		}
}

void
CMSourceDirector::CMSourceViewDirectorX
	(
	CMCommandDirector* commandDir
	)
{
	itsLink = CMGetLink();
	ListenTo(itsLink);

	itsCommandDir = commandDir;
	ListenTo(itsCommandDir);
	BuildWindow();

	ListenTo(CMGetPrefsManager());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMSourceDirector::~CMSourceDirector()
{
	if (itsType == kMainSourceType)
		{
		CMGetPrefsManager()->SaveWindowSize(kMainCodeWindSizeID, GetWindow());
		}
	else if (itsType == kMainAsmType)
		{
		CMGetPrefsManager()->SaveWindowSize(kMainAsmWindSizeID, GetWindow());
		}
	else
		{
		itsCommandDir->SourceWindowClosed(this);
		}

	jdelete itsSrcMainCmd;
}

/******************************************************************************
 GetCurrentExecLine

 ******************************************************************************/

JIndex
CMSourceDirector::GetCurrentExecLine()
	const
{
	return itsTable->GetCurrentLine();
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

#include "medic_current_source_window.xpm"
#include "medic_current_asm_window.xpm"

#include <jx_file_open.xpm>
#include <jx_file_print.xpm>
#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

void
CMSourceDirector::BuildWindow()
{
	const JIndex prefID = itsType == kMainSourceType ? kMainSrcToolBarID  :
						  itsType == kMainAsmType    ? kMainAsmToolBarID  :
						  itsType == kAsmType        ? kOtherAsmToolBarID :
						  kOtherSrcToolBarID;

// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 600,550, "");
	assert( window != NULL );

	itsMenuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 600,30);
	assert( itsMenuBar != NULL );

	itsToolBar =
		jnew JXToolBar(CMGetPrefsManager(), prefID, itsMenuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 600,500);
	assert( itsToolBar != NULL );

	itsFileDisplay =
		jnew JXFileNameDisplay("", window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,530, 580,20);
	assert( itsFileDisplay != NULL );

	CMFileDragSource* dragSource =
		jnew CMFileDragSource(this, window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 0,530, 20,20);
	assert( dragSource != NULL );

// end JXLayout

	window->SetTitle(JGetString(
		itsType == kMainAsmType ? "InitAsmWindowTitle::CMSourceDirector" :
								  "InitSourceWindowTitle::CMSourceDirector"));

	window->SetMinSize(300, 200);

	if (itsType == kMainSourceType)
		{
		window->SetCloseAction(JXWindow::kDeactivateDirector);

		window->SetWMClass(CMGetWMClassInstance(), CMGetMainSourceWindowClass());
		CMGetPrefsManager()->GetWindowSize(kMainCodeWindSizeID, window);

		JXDisplay* display = GetDisplay();
		JXImage* icon      = jnew JXImage(display, medic_current_source_window);
		assert( icon != NULL );
		window->SetIcon(icon);
		}
	else if (itsType == kMainAsmType)
		{
		window->SetCloseAction(JXWindow::kDeactivateDirector);

		window->SetWMClass(CMGetWMClassInstance(), CMGetMainAsmWindowClass());
		CMGetPrefsManager()->GetWindowSize(kMainAsmWindSizeID, window);

		JXDisplay* display = GetDisplay();
		JXImage* icon      = jnew JXImage(display, medic_current_asm_window);
		assert( icon != NULL );
		window->SetIcon(icon);
		}
	else if (itsType == kAsmType)
		{
		window->SetWMClass(CMGetWMClassInstance(), CMGetAsmViewWindowClass());
		CMGetPrefsManager()->GetWindowSize(kAsmWindSizeID, window, kJTrue);
		}
	else
		{
		window->SetWMClass(CMGetWMClassInstance(), CMGetSourceViewWindowClass());
		CMGetPrefsManager()->GetWindowSize(kCodeWindSizeID, window, kJTrue);
		}

	JCoordinate w = window->GetFrameWidth();

	itsFileMenu = itsMenuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "CMSourceDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd,  jx_file_open);
	itsFileMenu->SetItemImage(kPrintCmd, jx_file_print);

	// appends Edit & Search menus

	const JCoordinate kInitTableWidth = 50;

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != NULL );
	scrollbarSet->FitToEnclosure();

	JXContainer* encl = scrollbarSet->GetScrollEnclosure();

	itsText =
		jnew CMSourceText(this, itsCommandDir, itsMenuBar, scrollbarSet, encl,
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 kInitTableWidth, 0,
						 encl->GetApertureWidth()-kInitTableWidth, 100);
	assert( itsText != NULL );
	itsText->FitToEnclosure(kJFalse, kJTrue);
	ListenTo(itsText);

	if (itsType == kMainAsmType || itsType == kAsmType)
		{
		itsTable =
			jnew CMLineAddressTable(this, itsText, scrollbarSet, encl,
								   JXWidget::kFixedLeft, JXWidget::kVElastic,
								   0, 0, kInitTableWidth, 100);
		}
	else
		{
		itsTable =
			jnew CMLineNumberTable(this, itsText, scrollbarSet, encl,
								  JXWidget::kFixedLeft, JXWidget::kVElastic,
								  0, 0, kInitTableWidth, 100);
		}
	assert( itsTable != NULL );
	itsTable->FitToEnclosure(kJFalse, kJTrue);

	// requires itsText

	itsDebugMenu = itsCommandDir->CreateDebugMenu(itsMenuBar);
	ListenTo(itsDebugMenu);

	itsFnMenu =
		jnew CBFunctionMenu(NULL, kCBUnknownFT, itsText, itsMenuBar,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop,
						   0,0, 10,10);
	assert( itsFnMenu != NULL );
	itsFnMenu->Hide();

	itsPrefsMenu = itsMenuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "CMSourceDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	if (IsMainSourceWindow())
		{
		itsPrefsMenu->RemoveItem(kWindowSizeCmd);
		}
	ListenTo(itsPrefsMenu);

	itsHelpMenu = itsMenuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "CMSourceDirector");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);

	if (!IsMainSourceWindow())
		{
		(GetDisplay()->GetWDManager())->DirectorCreated(this);
		}
}

/******************************************************************************
 CreateWindowsMenu

	Call this after all directors have been constructed.

 ******************************************************************************/

void
CMSourceDirector::CreateWindowsMenu()
{
	itsCommandDir->CreateWindowsMenuAndToolBar(
			itsMenuBar, itsToolBar,
			JI2B( itsType == kMainAsmType || itsType == kAsmType ),
			kJTrue, JI2B( itsType != kMainSourceType ),
			itsDebugMenu, itsPrefsMenu, itsHelpMenu, kTOCCmd, kThisWindowCmd);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
CMSourceDirector::GetName()
	const
{
	if (itsType == kMainSourceType)
		{
		return JGetString("MainSourceName::CMSourceDirector");
		}
	else if (itsType == kMainAsmType)
		{
		return JGetString("MainAsmName::CMSourceDirector");
		}
	else
		{
		return JXWindowDirector::GetName();
		}
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

JBoolean
CMSourceDirector::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	if (itsType == kMainSourceType)
		{
		*icon = CMGetCurrentSourceIcon();
		}
	else if (itsType == kMainAsmType)
		{
		*icon = CMGetCurrentAsmIcon();
		}
	else if (itsType == kAsmType)
		{
		*icon = CMGetAsmSourceIcon();
		}
	else
		{
		*icon = CMGetSourceFileIcon();
		}
	return kJTrue;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMSourceDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (IsMainSourceWindow() &&
		sender == itsLink && message.Is(CMLink::kDebuggerStarted))
		{
		ClearDisplay();
		}
	else if (sender == itsLink && message.Is(CMLink::kPrepareToLoadSymbols))
		{
		if (!itsCurrentFile.IsEmpty())		// reload file, in case it changed
			{
			const JString fileName = itsCurrentFile;
			const JIndex lineIndex = itsText->HasSelection() ?
									 itsText->GetLineForChar(itsText->GetInsertionIndex()) :
									 itsTable->GetCurrentLine();
			itsCurrentFile.Clear();			// force reload
			DisplayFile(fileName, lineIndex);
			}
		}
	else if (IsMainSourceWindow() &&
			 sender == itsLink && message.Is(CMLink::kSymbolsLoaded))
		{
		const CMLink::SymbolsLoaded* info =
			dynamic_cast<const CMLink::SymbolsLoaded*>(&message);
		assert( info != NULL );
		UpdateWindowTitle(info->GetProgramName());
		}
	else if (IsMainSourceWindow() &&
			 sender == itsLink && message.Is(CMLink::kProgramStopped))
		{
		const CMLink::ProgramStopped* info =
			dynamic_cast<const CMLink::ProgramStopped*>(&message);
		assert( info != NULL);
		const CMLocation* loc;
		const JBoolean hasFile = info->GetLocation(&loc);
		if (itsType == kMainSourceType && hasFile)
			{
			DisplayFile(loc->GetFileName(), loc->GetLineNumber());
			}
		else if (itsType == kMainAsmType &&
				 !loc->GetFunctionName().IsEmpty() &&
				 !loc->GetMemoryAddress().IsEmpty())
			{
			DisplayDisassembly(*loc);
			}
		else if (itsType == kMainAsmType)
			{
			// wait for kProgramStopped2
			}
		else
			{
			#ifdef _J_OLD_OSX
			itsTable->SetCurrentLine(0);	// we get blank location the first time
			#else
			ClearDisplay();
			(CMGetCommandDirector())->GetStackDir()->Activate();
			#endif
			}
		}
	else if (itsType == kMainAsmType && sender == itsLink &&
			 message.Is(CMLink::kProgramStopped2))
		{
		const CMLink::ProgramStopped2* info =
			dynamic_cast<const CMLink::ProgramStopped2*>(&message);
		assert( info != NULL);
		const CMLocation* loc;
		info->GetLocation(&loc);
		if (!loc->GetFunctionName().IsEmpty() &&
			!loc->GetMemoryAddress().IsEmpty())
			{
			DisplayDisassembly(*loc);
			}
		else
			{
			ClearDisplay();
			}
		}

	else if (sender == itsLink &&
			 (message.Is(CMLink::kProgramFinished) ||
			  message.Is(CMLink::kCoreCleared)     ||
			  message.Is(CMLink::kDetachedFromProcess)))
		{
		if (itsSrcMainCmd != NULL)
			{
			itsSrcMainCmd->CMCommand::Send();
			}
		else if (itsType == kMainAsmType)
			{
			ClearDisplay();
			}
		}

	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
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

	else if (sender == itsDebugMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		itsCommandDir->UpdateDebugMenu(itsDebugMenu, itsText, NULL);
		}
	else if (sender == itsDebugMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsCommandDir->HandleDebugMenu(itsDebugMenu, selection->GetIndex(), itsText, NULL);
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

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}

	else if (sender == CMGetPrefsManager() && message.Is(CMPrefsManager::kFileTypesChanged))
		{
		UpdateFileType();
		}
	else if (sender == CMGetPrefsManager() && message.Is(CMPrefsManager::kCustomCommandsChanged))
		{
		itsCommandDir->AdjustDebugMenu(itsDebugMenu);
		}

	else
		{
		JXWindowDirector::Receive(sender,message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
CMSourceDirector::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !CMIsShuttingDown() && IsMainSourceWindow())
		{
		itsLink = CMGetLink();
		ListenTo(itsLink);

		if (itsType == kMainSourceType)
			{
			jdelete itsSrcMainCmd;
			itsSrcMainCmd = itsLink->CreateDisplaySourceForMain(this);
			}
		else if (itsType == kMainAsmType)
			{
			jdelete itsGetAssemblyCmd;
			itsGetAssemblyCmd = NULL;
			}

		CMClearSourceDisplayTask* task = jnew CMClearSourceDisplayTask(this);
		assert( task != NULL );
		task->Go();
		}
	else if (sender == itsLink && !CMIsShuttingDown())
		{
		JXCloseDirectorTask::Close(this);
		}
	else
		{
		JXWindowDirector::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 DisplayFile

 ******************************************************************************/

void
CMSourceDirector::DisplayFile
	(
	const JCharacter*	fileName,
	const JIndex		lineNumber,
	const JBoolean		markLine
	)
{
	assert( !JString::IsEmpty(fileName) );

	if (!itsCurrentFile.IsEmpty() && JSameDirEntry(fileName, itsCurrentFile))
		{
		DisplayLine(lineNumber, markLine);
		UpdateWindowTitle(NULL);
		}
	else if (JFileReadable(fileName))
		{
		itsCurrentFile = fileName;

		JTextEditor::PlainTextFormat format;
		itsText->ReadPlainText(itsCurrentFile, &format);

		// Assembly must not end with newline, so we have to enforce this for
		// source code, too.

		const JString& text = itsText->GetText();
		JIndex i = text.GetLength();
		while (text.GetCharacter(i) == '\n')
			{
			i--;
			}
		if (i < text.GetLength())
			{
			itsText->SetSelection(i+1, text.GetLength());
			itsText->DeleteSelection();
			}
		itsText->SetCaretLocation(1);

		DisplayLine(lineNumber, markLine);
		itsFileDisplay->SetText(itsCurrentFile);

		UpdateFileType();
		UpdateWindowTitle(NULL);
		}
}

/******************************************************************************
 DisplayDisassembly

 ******************************************************************************/

void
CMSourceDirector::DisplayDisassembly
	(
	const CMLocation& loc
	)
{
	const JString& fnName = loc.GetFunctionName();
	const JString& addr   = loc.GetMemoryAddress();

	assert( !JString::IsEmpty(fnName) );
	assert( !JString::IsEmpty(addr) );

	if (fnName == itsCurrentFn)
		{
		JIndex i;
		if (dynamic_cast<CMLineAddressTable*>(itsTable)->FindAddressLineNumber(addr, &i))
			{
			DisplayLine(i);
			}
		else
			{
			itsTable->SetCurrentLine(0);
			}

		UpdateWindowTitle(NULL);
		}
	else
		{
		itsCurrentFn = fnName;

		if (itsGetAssemblyCmd == NULL)
			{
			itsGetAssemblyCmd = (CMGetLink())->CreateGetAssembly(this);
			}

		if (itsGetAssemblyCmd != NULL)
			{
			itsAsmLocation = loc;
			itsGetAssemblyCmd->Send();
			}

		UpdateWindowTitle(NULL);
		}
}

/******************************************************************************
 DisplayDisassembly

 ******************************************************************************/

void
CMSourceDirector::DisplayDisassembly
	(
	JPtrArray<JString>*	addrList,
	const JCharacter*	instText
	)
{
	CMLineAddressTable* table = dynamic_cast<CMLineAddressTable*>(itsTable);
	table->SetLineNumbers(addrList);
	itsText->SetText(instText);

	JIndex i;
	if (table->FindAddressLineNumber(itsAsmLocation.GetMemoryAddress(), &i))
		{
		DisplayLine(i);
		}
	else
		{
		table->SetCurrentLine(0);
		}
}

/******************************************************************************
 DisplayLine

 ******************************************************************************/

void
CMSourceDirector::DisplayLine
	(
	const JSize		lineNumber,
	const JBoolean	markLine
	)
{
	if (lineNumber > 0)
		{
		const JIndex i = itsText->CRLineIndexToVisualLineIndex(lineNumber);

		itsText->GoToLine(i);
		if (IsMainSourceWindow() && markLine)
			{
			itsTable->SetCurrentLine(i);
			}
		else
			{
			itsText->SelectLine(i);
			}
		}
}

/******************************************************************************
 ClearDisplay

 ******************************************************************************/

void
CMSourceDirector::ClearDisplay()
{
	CMLineAddressTable* table = dynamic_cast<CMLineAddressTable*>(itsTable);
	if (table != NULL)
		{
		table->ClearLineNumbers();
		}
	itsText->SetText("");
	itsFileDisplay->SetText("");
	itsCurrentFile.Clear();
	itsCurrentFn.Clear();
	UpdateWindowTitle(NULL);

	UpdateFileType();
}

/******************************************************************************
 UpdateFileType

 ******************************************************************************/

void
CMSourceDirector::UpdateFileType()
{
	const CBTextFileType fileType =
		itsType == kMainAsmType || itsType == kAsmType ? kCBAssemblyFT :
		itsCurrentFile.IsEmpty() ? kCBUnknownFT :
			CMGetPrefsManager()->GetFileType(itsCurrentFile);

	itsText->SetFileType(fileType);

	if (!itsCurrentFile.IsEmpty() && (CMGetFnMenuUpdater())->CanCreateMenu(fileType))
		{
		itsMenuBar->InsertMenuAfter(itsDebugMenu, itsFnMenu);
		itsFnMenu->TextChanged(fileType, itsCurrentFile);
		}
	else
		{
		itsMenuBar->RemoveMenu(itsFnMenu);
		}

	if (!itsCurrentFile.IsEmpty())
		{
		JBoolean setTabWidth, setTabMode, tabInsertsSpaces, setAutoIndent, autoIndent;
		JSize tabWidth;
		CBMParseEditorOptions(itsText->GetText(), &setTabWidth, &tabWidth,
							  &setTabMode, &tabInsertsSpaces, &setAutoIndent, &autoIndent);
		if (setTabWidth)
			{
			itsText->SetCRMTabCharCount(tabWidth);
			itsText->SetDefaultTabWidth(
				tabWidth * itsText->GetDefaultFont().GetCharWidth(' '));
			}
		}
}

/******************************************************************************
 UpdateWindowTitle (private)

	binaryName can be NULL

 ******************************************************************************/

void
CMSourceDirector::UpdateWindowTitle
	(
	const JCharacter* binaryName
	)
{
	JString path, title, prefix;
	if (itsType == kMainAsmType || itsType == kAsmType)
		{
		prefix = JGetString("WindowTitlePrefixMainAsm::CMSourceDirector");
		title  = itsCurrentFn.IsEmpty() ? JGetString("WindowTitleNoAsm::CMSourceDirector") : itsCurrentFn;
		}
	else
		{
		prefix = JGetString("WindowTitlePrefixMainSrc::CMSourceDirector");

		if (itsCurrentFile.IsEmpty())
			{
			title = JGetString("WindowTitleNoSrc::CMSourceDirector");
			}
		else
			{
			JSplitPathAndName(itsCurrentFile, &path, &title);
			}
		}

	if (IsMainSourceWindow())
		{
		// Save prefix with binary name since we will be called when file
		// name changes, too.

		if (binaryName != NULL)
			{
			itsWindowTitlePrefix  = binaryName;
			itsWindowTitlePrefix += prefix;
			}
		else if (itsWindowTitlePrefix.IsEmpty())
			{
			itsWindowTitlePrefix = prefix;
			}

		title.Prepend(itsWindowTitlePrefix);
		}

	GetWindow()->SetTitle(title);
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
CMSourceDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kEditCmd, !itsCurrentFile.IsEmpty());
	itsFileMenu->SetItemEnable(kLoadConfigCmd, itsLink->HasProgram());
	itsFileMenu->SetItemEnable(kCloseCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu

 ******************************************************************************/

void
CMSourceDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenCmd)
		{
		itsCommandDir->OpenSourceFiles();
		}
	else if (index == kEditCmd)
		{
		const JIndex visualIndex =
			itsText->HasSelection() ?
				itsText->GetLineForChar(itsText->GetInsertionIndex()) :
				itsTable->GetCurrentLine();

		(CMGetApplication())->
			EditFile(itsCurrentFile,
				visualIndex > 0 ?
					itsText->VisualLineIndexToCRLineIndex(visualIndex) :
					visualIndex);
		}

	else if (index == kLoadConfigCmd)
		{
		itsCommandDir->LoadConfig();
		}
	else if (index == kSaveConfigCmd)
		{
		itsCommandDir->SaveConfig();
		}

	else if (index == kPageSetupCmd)
		{
		itsText->HandlePTPageSetup();
		}
	else if (index == kPrintCmd)
		{
		itsText->PrintPT();
		}

	else if (index == kCloseCmd)
		{
		if (IsMainSourceWindow())
			{
			Deactivate();
			}
		else
			{
			Close();
			}
		}
	else if (index == kQuitCmd)
		{
		JXGetApplication()->Quit();
		}
}

/******************************************************************************
 UpdatePrefsMenu

 ******************************************************************************/

void
CMSourceDirector::UpdatePrefsMenu()
{
	CMPrefsManager::DebuggerType type = CMGetPrefsManager()->GetDebuggerType();
	itsPrefsMenu->CheckItem(kDebuggerTypeToMenuIndex[ type ]);

	itsPrefsMenu->DisableItem(kUseJavaCmd);
}

/******************************************************************************
 HandlePrefsMenu

 ******************************************************************************/

void
CMSourceDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kUseGDBCmd)
		{
		CMGetPrefsManager()->SetDebuggerType(CMPrefsManager::kGDBType);
		}
	else if (index == kUseLLDBCmd)
		{
		CMGetPrefsManager()->SetDebuggerType(CMPrefsManager::kLLDBType);
		}
	else if (index == kUseJavaCmd)
		{
		CMGetPrefsManager()->SetDebuggerType(CMPrefsManager::kJavaType);
		}
	else if (index == kUseXdebugCmd)
		{
		CMGetPrefsManager()->SetDebuggerType(CMPrefsManager::kXdebugType);
		}

	else if (index == kEditPrefsCmd)
		{
		CMGetPrefsManager()->EditPrefs();
		}
	else if (index == kEditToolBarCmd)
		{
		itsToolBar->Edit();
		}
	else if (index == kEditCmdsCmd)
		{
		CMEditCommandsDialog* dlog = jnew CMEditCommandsDialog;
		assert(dlog != NULL);
		dlog->BeginDialog();
		}
	else if (index == kEditMacWinPrefsCmd)
		{
		JXMacWinPrefsDialog::EditPrefs();
		}

	else if (index == kWindowSizeCmd)
		{
		if (itsType == kSourceType)
			{
			CMGetPrefsManager()->SaveWindowSize(kCodeWindSizeID, GetWindow());
			}
		else if (itsType == kAsmType)
			{
			CMGetPrefsManager()->SaveWindowSize(kAsmWindSizeID, GetWindow());
			}
		}
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
CMSourceDirector::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		(CMGetApplication())->DisplayAbout();
		}
	else if (index == kTOCCmd)
		{
		(JXGetHelpManager())->ShowTOC();
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection("CMOverviewHelp");
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection("CMSourceWindowHelp");
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
