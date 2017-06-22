/******************************************************************************
 CMCommandDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997-2004 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CMCommandDirector.h"
#include "CMSourceDirector.h"
#include "CMThreadsDir.h"
#include "CMStackDir.h"
#include "CMBreakpointsDir.h"
#include "CMVarTreeDir.h"
#include "CMArray1DDir.h"
#include "CMArray2DDir.h"
#include "CMPlot2DDir.h"
#include "CMMemoryDir.h"
#include "CMLocalVarsDir.h"
#include "CMRegistersDir.h"
#include "CMFileListDir.h"
#include "CMDebugDir.h"

#include "CMMDIServer.h"
#include "CMCommandOutputDisplay.h"
#include "CMCommandInput.h"
#include "CMGetCompletions.h"
#include "CMEditCommandsDialog.h"
#include "CMChooseProcessDialog.h"
#include "CMGetFullPath.h"
#include "CMGetInitArgs.h"
#include "CMRunProgramTask.h"

#include "CMMemoryDir.h"
#include "CMBreakpointManager.h"
#include "cmGlobals.h"
#include "cmFileVersions.h"
#include "cmActionDefs.h"

#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXToolBar.h>
#include <JXScrollbarSet.h>
#include <JXDownRect.h>
#include <JXStaticText.h>
#include <JXDockWidget.h>
#include <JXStringHistoryMenu.h>
#include <JXHelpManager.h>
#include <JXWDManager.h>
#include <JXWDMenu.h>
#include <JXChooseSaveFile.h>
#include <JXImage.h>
#include <JXColormap.h>
#include <JXMacWinPrefsDialog.h>

#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

static const JCharacter* kWindowTitleSuffix = " Command Line";

const JSize kCmdHistoryLength = 100;

// File menu

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"    Open source file...   %k Meta-O %i" kCMOpenSourceFileAction
	"%l| Load configuration... %k Ctrl-O %i" kCMLoadConfigAction
	"  | Save configuration... %k Ctrl-S %i" kCMSaveConfigAction
	"%l| Save history                    %i" kCMSaveHistoryAction
	"  | Save history as...              %i" kCMSaveHistoryAsAction
	"%l| Page setup...                   %i" kJXPageSetupAction
	"  | Print...              %k Meta-P %i" kJXPrintAction
	"%l| Close                 %k Meta-W %i" kJXCloseWindowAction
	"  | Quit                  %k Meta-Q %i" kJXQuitAction;

enum
{
	kOpenCmd = 1,
	kLoadConfigCmd,
	kSaveConfigCmd,
	kSaveCmd,
	kSaveAsCmd,
	kPageSetupCmd,
	kPrintCmd,
	kCloseCmd,
	kQuitCmd
};

// Debug menu

static const JCharacter* kDebugMenuTitleStr = "Debug";
static const JCharacter* kDebugMenuStr =
	"    Choose program...      %k Meta-Shift-P %i" kCMChooseBinaryAction
	"  | Reload program                         %i" kCMReloadBinaryAction
	"  | Choose core...         %k Meta-Shift-C %i" kCMChooseCoreAction
	"  | Choose process...                      %i" kCMChooseProcessAction
	"  | Set arguments                          %i" kCMSetArgumentsAction
	"%l| Restart debugger                       %i" kCMRestartDebuggerAction
	"%l| Display as variable    %k Meta-D       %i" kCMDisplayVariableAction
	"  | Display as 1D array    %k Meta-Shift-A %i" kCMDisplay1DArrayAction
	"  | Plot as 1D array                       %i" kCMPlot1DArrayAction
	"  | Display as 2D array                    %i" kCMDisplay2DArrayAction
	"%l| Watch expression                       %i" kCMWatchVarValueAction
	"  | Watch location                         %i" kCMWatchVarLocationAction
	"%l| Examine memory                         %i" kCMExamineMemoryAction
	"  | Disassemble memory                     %i" kCMDisasmMemoryAction
	"  | Disassemble function                   %i" kCMDisasmFunctionAction
	"%l| Run program            %k Meta-R       %i" kCMRunProgramAction
	"  | Pause execution        %k Ctrl-C       %i" kCMStopProgramAction
	"  | Kill process                           %i" kCMKillProgramAction
	"%l| Next statement         %k Meta-N       %i" kCMNextInstrAction
	"  | Step into              %k Meta-J       %i" kCMStepIntoAction
	"  | Finish subroutine      %k Meta-K       %i" kCMFinishSubAction
	"  | Continue execution     %k Meta-Shift-N %i" kCMContinueRunAction
	"%l| Next instruction       %k Meta-U       %i" kCMNextAsmInstrAction
	"  | Step into (instruction)%k Meta-I       %i" kCMStepIntoAsmAction
	"%l| Previous statement                     %i" kCMPrevInstrAction
	"  | Backup into                            %i" kCMReverseStepIntoAction
	"  | Reverse finish                         %i" kCMReverseFinishSubAction
	"  | Reverse continue                       %i" kCMReverseContinueRunAction
	"%l| Remove all breakpoints %k Ctrl-X       %i" kCMClearAllBreakpointsAction
	"%l";

enum
{
	kSelectBinCmd = 1,
	kReloadBinCmd,
	kSelectCoreCmd,
	kSelectProcessCmd,
	kSetArgsCmd,
	kRestartDebuggerCmd,
	kDisplayVarCmd,
	kDisplay1DArrayCmd,
	kPlot1DArrayCmd,
	kDisplay2DArrayCmd,
	kWatchVarCmd,
	kWatchLocCmd,
	kExamineMemCmd,
	kDisassembleMemCmd,
	kDisassembleFnCmd,
	kRunCmd,
	kStopCmd,
	kKillCmd,
	kNextCmd,
	kStepCmd,
	kFinishCmd,
	kContCmd,
	kNextAsmCmd,
	kStepAsmCmd,
	kPrevCmd,
	kReverseStepCmd,
	kReverseFinishCmd,
	kReverseContCmd,
	kRemoveAllBreakpointsCmd,

	kFirstCustomDebugCmd
};

// Windows menu

static const JCharacter* kWindowsMenuTitleStr = "Windows";

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
	"  | Mac/Win/X emulation...";

enum
{
	kUseGDBCmd = 1,
	kUseLLDBCmd,
	kUseJavaCmd,
	kUseXdebugCmd,
	kEditPrefsCmd,
	kEditToolBarCmd,
	kEditCmdsCmd,
	kEditMacWinPrefsCmd
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
 Constructor

 ******************************************************************************/

CMCommandDirector::CMCommandDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	itsLink = CMGetLink();
	ListenTo(itsLink);

	JXMenuBar* menuBar = BuildWindow();

	itsCurrentSourceDir = jnew CMSourceDirector(this, CMSourceDirector::kMainSourceType);
	assert( itsCurrentSourceDir != NULL );
	itsCurrentSourceDir->Activate();

	itsCurrentAsmDir = jnew CMSourceDirector(this, CMSourceDirector::kMainAsmType);
	assert( itsCurrentAsmDir != NULL );

	itsThreadsDir = jnew CMThreadsDir(this);
	assert( itsThreadsDir != NULL );

	itsStackDir = jnew CMStackDir(this);
	assert(itsStackDir != NULL);

	itsBreakpointsDir = jnew CMBreakpointsDir(this);
	assert(itsBreakpointsDir != NULL);

	itsVarTreeDir = jnew CMVarTreeDir(this);
	assert(itsVarTreeDir != NULL);

	itsLocalVarsDir = jnew CMLocalVarsDir(this);
	assert(itsLocalVarsDir != NULL);

	itsRegistersDir = jnew CMRegistersDir(this);
	assert(itsRegistersDir != NULL);

	itsFileListDir = jnew CMFileListDir(this);
	assert(itsFileListDir != NULL);

	itsDebugDir = jnew CMDebugDir(this);
	assert(itsDebugDir!=NULL);

	JXWDManager* wdMgr = GetDisplay()->GetWDManager();
	wdMgr->PermanentDirectorCreated(this,                NULL,           kCMShowCommandLineAction);
	wdMgr->PermanentDirectorCreated(itsCurrentSourceDir, NULL,           kCMShowCurrentSourceAction);
	wdMgr->PermanentDirectorCreated(itsThreadsDir,       "Meta-Shift-T", kCMShowThreadsAction);
	wdMgr->PermanentDirectorCreated(itsStackDir,         "Meta-Shift-S", kCMShowStackTraceAction);
	wdMgr->PermanentDirectorCreated(itsBreakpointsDir,   "Meta-Shift-B", kCMShowBreakpointsAction);
	wdMgr->PermanentDirectorCreated(itsVarTreeDir,       "Meta-Shift-V", kCMShowVariablesAction);
	wdMgr->PermanentDirectorCreated(itsLocalVarsDir,     "Meta-Shift-L", kCMShowLocalVariablesAction);
	wdMgr->PermanentDirectorCreated(itsCurrentAsmDir,    "Meta-Shift-Y", kCMShowCurrentAsmAction);
	wdMgr->PermanentDirectorCreated(itsRegistersDir,     "Meta-Shift-R", kCMShowRegistersAction);
	wdMgr->PermanentDirectorCreated(itsFileListDir,      "Meta-Shift-F", kCMShowFileListAction);
	wdMgr->PermanentDirectorCreated(itsDebugDir,         "",             kCMShowDebugInfoAction);

	CreateWindowsMenuAndToolBar(menuBar, itsToolBar, kJFalse, kJFalse, kJTrue,
								itsDebugMenu, itsPrefsMenu,
								itsHelpMenu, kTOCCmd, kThisWindowCmd);
	itsCurrentSourceDir->CreateWindowsMenu();
	itsCurrentAsmDir->CreateWindowsMenu();

	itsSourceDirs = jnew JPtrArray<CMSourceDirector>(JPtrArrayT::kForgetAll);
	assert( itsSourceDirs != NULL );

	itsAsmDirs = jnew JPtrArray<CMSourceDirector>(JPtrArrayT::kForgetAll);
	assert( itsAsmDirs != NULL );

	itsArray1DDirs = jnew JPtrArray<CMArray1DDir>(JPtrArrayT::kForgetAll);
	assert( itsArray1DDirs != NULL );

	itsArray2DDirs = jnew JPtrArray<CMArray2DDir>(JPtrArrayT::kForgetAll);
	assert( itsArray2DDirs != NULL );

	itsPlot2DDirs = jnew JPtrArray<CMPlot2DDir>(JPtrArrayT::kForgetAll);
	assert( itsPlot2DDirs != NULL );

	itsMemoryDirs = jnew JPtrArray<CMMemoryDir>(JPtrArrayT::kForgetAll);
	assert( itsMemoryDirs != NULL );

	itsHistoryIndex     = 0;
	itsWaitingToRunFlag = kJFalse;

	InitializeCommandOutput();

	itsGetArgsCmd = itsLink->CreateGetInitArgs(itsArgInput);

	ListenTo(CMGetPrefsManager());

	itsFakePrompt->SetText(itsLink->GetPrompt());
	itsFakePrompt->SetFontStyle(
		CMGetPrefsManager()->GetColor(CMPrefsManager::kRightMarginColorIndex));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMCommandDirector::~CMCommandDirector()
{
	CloseDynamicDirectors();

	CMGetPrefsManager()->SaveWindowSize(kCmdWindSizeID, GetWindow());

	jdelete itsSourceDirs;
	jdelete itsAsmDirs;
	jdelete itsArray1DDirs;
	jdelete itsArray2DDirs;
	jdelete itsPlot2DDirs;
	jdelete itsMemoryDirs;
//	jdelete itsISOStyler;
	jdelete itsGetArgsCmd;
}

/******************************************************************************
 Close (virtual)

 ******************************************************************************/

JBoolean
CMCommandDirector::Close()
{
	CloseDynamicDirectors();

	CMGetPrefsManager()->WriteHistoryMenuSetup(itsHistoryMenu);
	return JXWindowDirector::Close();		// deletes us if successful
}

/******************************************************************************
 CloseDynamicDirectors

 ******************************************************************************/

void
CMCommandDirector::CloseDynamicDirectors()
{
	JSize count = itsSourceDirs->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		(itsSourceDirs->GetElement(i))->Close();
		}

	count = itsAsmDirs->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		(itsAsmDirs->GetElement(i))->Close();
		}

	count = itsArray1DDirs->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		(itsArray1DDirs->GetElement(i))->Close();
		}

	count = itsArray2DDirs->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		(itsArray2DDirs->GetElement(i))->Close();
		}

	count = itsPlot2DDirs->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		(itsPlot2DDirs->GetElement(i))->Close();
		}

	count = itsMemoryDirs->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		(itsMemoryDirs->GetElement(i))->Close();
		}
}

/******************************************************************************
 TransferKeyPressToInput

 ******************************************************************************/

void
CMCommandDirector::TransferKeyPressToInput
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsCommandInput->Focus())
		{
		itsCommandInput->HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 InitializeCommandOutput

 ******************************************************************************/

void
CMCommandDirector::InitializeCommandOutput()
{
	itsCommandOutput->SetText("");

	const JCharacter* map[] =
		{
		"vers", CMGetVersionNumberStr()
		};
	const JString welcome = JGetString("Welcome::CMCommandDirector", map, sizeof(map));
	itsCommandOutput->SetText(welcome);
}

/******************************************************************************
 PrepareCommand

 ******************************************************************************/

void
CMCommandDirector::PrepareCommand
	(
	const JCharacter* cmd
	)
{
	itsCommandInput->SetText(cmd);
	itsCommandInput->Focus();
	itsCommandInput->SetCaretLocation(itsCommandInput->GetTextLength()+1);
	Activate();
	GetWindow()->RequestFocus();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "medic_command_window.xpm"

#include <jx_file_open.xpm>
#include <jx_file_print.xpm>
#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

JXMenuBar*
CMCommandDirector::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 500,550, "");
	assert( window != NULL );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != NULL );

	itsToolBar =
		jnew JXToolBar(CMGetPrefsManager(), kCmdWindowToolBarID, menuBar, 300, 200, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,440);
	assert( itsToolBar != NULL );

	itsProgramButton =
		jnew JXTextButton(JGetString("itsProgramButton::CMCommandDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 0,530, 150,20);
	assert( itsProgramButton != NULL );

	itsDownRect =
		jnew JXDownRect(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 0,470, 500,60);
	assert( itsDownRect != NULL );

	itsFakePrompt =
		jnew JXStaticText(JGetString("itsFakePrompt::CMCommandDirector::JXLayout"), itsDownRect,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,0, 38,20);
	assert( itsFakePrompt != NULL );
	itsFakePrompt->SetToLabel();

	itsCommandInput =
		jnew CMCommandInput(itsDownRect,
					JXWidget::kHElastic, JXWidget::kFixedTop, 53,0, 445,56);
	assert( itsCommandInput != NULL );

	itsHistoryMenu =
		jnew JXStringHistoryMenu(kCmdHistoryLength, "", itsDownRect,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 30,20);
	assert( itsHistoryMenu != NULL );

	itsArgInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 150,530, 350,20);
	assert( itsArgInput != NULL );

// end JXLayout

	window->SetTitle(kWindowTitleSuffix);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->ShouldFocusWhenShow(kJTrue);
	window->SetWMClass(CMGetWMClassInstance(), CMGetCommandWindowClass());
	CMGetPrefsManager()->GetWindowSize(kCmdWindSizeID, window);

	JXDisplay* display = GetDisplay();
	JXImage* icon      = jnew JXImage(display, medic_command_window);
	assert( icon != NULL );
	window->SetIcon(icon);

	itsDownRect->SetBackColor(itsCommandInput->GetCurrBackColor());
	itsFakePrompt->SetBackColor(itsCommandInput->GetCurrBackColor());
	CMTextDisplayBase::AdjustFont(itsFakePrompt);
	ListenTo(itsCommandInput);

	CMGetPrefsManager()->ReadHistoryMenuSetup(itsHistoryMenu);
	ListenTo(itsHistoryMenu);

	itsProgramButton->SetFontName(JGetMonospaceFontName());
	itsProgramButton->SetFontSize(kJDefaultMonoFontSize);
	itsProgramButton->SetActive(itsLink->GetFeature(CMLink::kSetProgram));
	ListenTo(itsProgramButton);

	CMTextDisplayBase::AdjustFont(itsArgInput);
	itsArgInput->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

	// menus

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr, "CMCommandDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd,  jx_file_open);
	itsFileMenu->SetItemImage(kPrintCmd, jx_file_print);

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != NULL );
	scrollbarSet->FitToEnclosure();

	// appends Edit & Search menus

	itsCommandOutput =
		jnew CMCommandOutputDisplay(menuBar,
							 scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( itsCommandOutput != NULL );
	itsCommandOutput->FitToEnclosure(kJTrue, kJTrue);

	itsCommandOutput->AppendSearchMenu(menuBar);

	itsDebugMenu = CreateDebugMenu(menuBar);
	ListenTo(itsDebugMenu);

	itsCommandInput->ShareEditMenu(itsCommandOutput);
	itsArgInput->ShareEditMenu(itsCommandOutput);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "CMCommandDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "CMCommandDirector");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);

	return menuBar;
}

/******************************************************************************
 DockAll

 ******************************************************************************/

void
CMCommandDirector::DockAll
	(
	JXDockWidget* srcDock,
	JXDockWidget* infoDock,
	JXDockWidget* dataDock
	)
{
	srcDock->Dock(itsCurrentSourceDir);
	srcDock->Dock(itsCurrentAsmDir);

	JSize count = itsSourceDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		srcDock->Dock(itsSourceDirs->GetElement(i));
		}

	count = itsAsmDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		srcDock->Dock(itsAsmDirs->GetElement(i));
		}

	infoDock->Dock(itsThreadsDir);
	infoDock->Dock(itsStackDir);
	infoDock->Dock(itsBreakpointsDir);
	infoDock->Dock(itsFileListDir);

	dataDock->Dock(itsVarTreeDir);
	dataDock->Dock(itsLocalVarsDir);
	dataDock->Dock(itsRegistersDir);

	count = itsArray1DDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		dataDock->Dock(itsArray1DDirs->GetElement(i));
		}

	count = itsArray2DDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		dataDock->Dock(itsArray2DDirs->GetElement(i));
		}

	count = itsPlot2DDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		dataDock->Dock(itsPlot2DDirs->GetElement(i));
		}

	count = itsMemoryDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		dataDock->Dock(itsMemoryDirs->GetElement(i));
		}
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
CMCommandDirector::UpdateWindowTitle
	(
	const JCharacter* programName
	)
{
	JString title = programName;
	title += kWindowTitleSuffix;
	GetWindow()->SetTitle(title);

	if (!JString::IsEmpty(programName))
		{
		itsProgramButton->SetLabel(programName);
		}
	else
		{
		itsProgramButton->SetLabel(JGetString("NoBinaryButtonLabel::CMCommandDirector"));
		}
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
CMCommandDirector::GetName()
	const
{
	return JGetString("WindowsMenuText::CMCommandDirector");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

JBoolean
CMCommandDirector::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = CMGetCommandLineIcon();
	return kJTrue;
}

/******************************************************************************
 CreateDebugMenu

 ******************************************************************************/

#include <jx_executable_small.xpm>
#include "medic_choose_core.xpm"
#include "medic_attach_process.xpm"
#include "medic_show_1d_array.xpm"
#include "medic_show_2d_plot.xpm"
#include "medic_show_2d_array.xpm"
#include "medic_show_memory.xpm"
#include "medic_run_program.xpm"
#include "medic_stop_program.xpm"
#include "medic_kill_program.xpm"
#include "medic_next.xpm"
#include "medic_step.xpm"
#include "medic_finish.xpm"
#include "medic_continue.xpm"
#include "medic_nexti.xpm"
#include "medic_stepi.xpm"
#include "medic_reverse_next.xpm"
#include "medic_reverse_step.xpm"
#include "medic_reverse_finish.xpm"
#include "medic_reverse_continue.xpm"

JXTextMenu*
CMCommandDirector::CreateDebugMenu
	(
	JXMenuBar* menuBar
	)
{
	JXTextMenu* menu = menuBar->AppendTextMenu(kDebugMenuTitleStr);
	menu->SetMenuItems(kDebugMenuStr, "CMCommandDirector");

	menu->SetItemImage(kSelectBinCmd,      jx_executable_small);
	menu->SetItemImage(kSelectCoreCmd,     medic_choose_core);
	menu->SetItemImage(kSelectProcessCmd,  medic_attach_process);
	menu->SetItemImage(kDisplay1DArrayCmd, medic_show_1d_array);
	menu->SetItemImage(kPlot1DArrayCmd,    medic_show_2d_plot);
	menu->SetItemImage(kDisplay2DArrayCmd, medic_show_2d_array);
	menu->SetItemImage(kExamineMemCmd,     medic_show_memory);
	menu->SetItemImage(kRunCmd,            medic_run_program);
	menu->SetItemImage(kStopCmd,           medic_stop_program);
	menu->SetItemImage(kKillCmd,           medic_kill_program);
	menu->SetItemImage(kNextCmd,           medic_next);
	menu->SetItemImage(kStepCmd,           medic_step);
	menu->SetItemImage(kFinishCmd,         medic_finish);
	menu->SetItemImage(kContCmd,           medic_continue);
	menu->SetItemImage(kNextAsmCmd,        medic_nexti);
	menu->SetItemImage(kStepAsmCmd,        medic_stepi);
	menu->SetItemImage(kPrevCmd,           medic_reverse_next);
	menu->SetItemImage(kReverseStepCmd,    medic_reverse_step);
	menu->SetItemImage(kReverseFinishCmd,  medic_reverse_finish);
	menu->SetItemImage(kReverseContCmd,    medic_reverse_continue);

	// Meta-. should also be "stop"

	JXKeyModifiers m(menu->GetDisplay());
	m.SetState(kJXMetaKeyIndex, kJTrue);
	(menu->GetWindow())->InstallMenuShortcut(menu, kStopCmd, '.', m);

	AdjustDebugMenu(menu);
	return menu;
}

/******************************************************************************
 AddDebugMenuItemsToToolBar

 ******************************************************************************/

void
CMCommandDirector::AddDebugMenuItemsToToolBar
	(
	JXToolBar*		toolBar,
	JXTextMenu*		debugMenu,
	const JBoolean	includeStepAsm
	)
{
	toolBar->AppendButton(debugMenu, kRunCmd);
	toolBar->AppendButton(debugMenu, kStopCmd);
	toolBar->AppendButton(debugMenu, kKillCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(debugMenu, kNextCmd);
	toolBar->AppendButton(debugMenu, kStepCmd);
	toolBar->AppendButton(debugMenu, kFinishCmd);
	toolBar->AppendButton(debugMenu, kContCmd);

	if (includeStepAsm)
		{
		toolBar->NewGroup();
		toolBar->AppendButton(debugMenu, kNextAsmCmd);
		toolBar->AppendButton(debugMenu, kStepAsmCmd);
		}
}

/******************************************************************************
 AdjustDebugMenu

 ******************************************************************************/

void
CMCommandDirector::AdjustDebugMenu
	(
	JXTextMenu* menu
	)
{
	const JSize itemCount = menu->GetItemCount();
	for (JIndex i=itemCount; i>=kFirstCustomDebugCmd; i--)
		{
		menu->RemoveItem(i);
		}

	JPtrArray<JString> cmds(JPtrArrayT::kDeleteAll);
	CMGetPrefsManager()->GetCmdList(&cmds);

	const JSize count = cmds.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* cmd = cmds.GetElement(i);
		menu->AppendItem(*cmd, JXMenu::kPlainType, NULL, NULL, *cmd);
		}
}

/******************************************************************************
 CreateWindowsMenuAndToolBar

 ******************************************************************************/

void
CMCommandDirector::CreateWindowsMenuAndToolBar
	(
	JXMenuBar*		menuBar,
	JXToolBar*		toolBar,
	const JBoolean	includeStepAsm,
	const JBoolean	includeCmdLine,
	const JBoolean	includeCurrSrc,
	JXTextMenu*		debugMenu,
	JXTextMenu*		prefsMenu,
	JXTextMenu*		helpMenu,
	const JIndex	tocCmd,
	const JIndex	thisWindowCmd
	)
{
	JXWDMenu* wdMenu =
		jnew JXWDMenu(kWindowsMenuTitleStr, menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != NULL );
	menuBar->InsertMenuBefore(prefsMenu, wdMenu);

	toolBar->LoadPrefs();
	if (toolBar->IsEmpty())
		{
		AddDebugMenuItemsToToolBar(toolBar, debugMenu, includeStepAsm);
		toolBar->NewGroup();
		AddWindowsMenuItemsToToolBar(toolBar, wdMenu, includeCmdLine, includeCurrSrc);
		toolBar->NewGroup();
		toolBar->AppendButton(helpMenu, tocCmd);
		toolBar->AppendButton(helpMenu, thisWindowCmd);
		}
}

/******************************************************************************
 AddWindowsMenuItemsToToolBar

 ******************************************************************************/

void
CMCommandDirector::AddWindowsMenuItemsToToolBar
	(
	JXToolBar*		toolBar,
	JXTextMenu*		windowsMenu,
	const JBoolean	includeCmdLine,
	const JBoolean	includeCurrSrc
	)
{
	if (includeCmdLine)
		{
		toolBar->AppendButton(windowsMenu, kCMShowCommandLineAction);
		}
	if (includeCurrSrc)
		{
		toolBar->AppendButton(windowsMenu, kCMShowCurrentSourceAction);
		}
	toolBar->AppendButton(windowsMenu, kCMShowStackTraceAction);
	toolBar->AppendButton(windowsMenu, kCMShowBreakpointsAction);
	toolBar->AppendButton(windowsMenu, kCMShowVariablesAction);
	toolBar->AppendButton(windowsMenu, kCMShowLocalVariablesAction);
	toolBar->AppendButton(windowsMenu, kCMShowFileListAction);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMCommandDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	// CMLink

	if (sender == itsLink && message.Is(CMLink::kUserOutput))
		{
		const CMLink::UserOutput* output =
			dynamic_cast<const CMLink::UserOutput*>(&message);
		assert(output != NULL);
		itsCommandOutput->SetCaretLocation(itsCommandOutput->GetTextLength() + 1);
//		itsISOStyler->FilterISO(output->GetText());

		JFont font = itsCommandOutput->GetDefaultFont();
		font.SetBold(output->IsFromTarget());
		font.SetColor(output->IsError() ?
			(itsCommandOutput->GetColormap())->GetDarkRedColor() :
			(itsCommandOutput->GetColormap())->GetBlackColor());
		itsCommandOutput->SetCurrentFont(font);
		itsCommandOutput->Paste(output->GetText());
		itsCommandOutput->ClearUndo();
		}

	else if (sender == itsLink && message.Is(CMLink::kDebuggerBusy))
		{
		itsFakePrompt->SetFontStyle(
			CMGetPrefsManager()->GetColor(CMPrefsManager::kRightMarginColorIndex));
		}
	else if (sender == itsLink && message.Is(CMLink::kDebuggerReadyForInput))
		{
		itsFakePrompt->SetText(itsLink->GetPrompt());
		itsFakePrompt->SetFontStyle(
			CMGetPrefsManager()->GetColor(CMPrefsManager::kTextColorIndex));
		}
	else if (sender == itsLink && message.Is(CMLink::kDebuggerDefiningScript))
		{
		itsFakePrompt->SetText(itsLink->GetScriptPrompt());
		itsFakePrompt->SetFontStyle(
			CMGetPrefsManager()->GetColor(CMPrefsManager::kTextColorIndex));
		// can't do it here because, when core file loaded, hook-run is
		// defined after stack window is opened
//		Activate();
//		GetWindow()->RequestFocus();
		}

	else if (sender == itsLink && message.Is(CMLink::kProgramFinished))
		{
		GetDisplay()->Beep();
		}

	else if (sender == itsLink && message.Is(CMLink::kSymbolsLoaded))
		{
		const CMLink::SymbolsLoaded* info =
			dynamic_cast<const CMLink::SymbolsLoaded*>(&message);
		assert( info != NULL );
		UpdateWindowTitle(info->GetProgramName());

		if (itsWaitingToRunFlag)
			{
			CMRunProgramTask* task = jnew CMRunProgramTask();
			assert( task != NULL );
			task->Start();
			itsWaitingToRunFlag = kJFalse;
			}
		}

	else if (sender == itsLink && message.Is(CMLink::kDebuggerStarted))
		{
		if (itsGetArgsCmd != NULL)
			{
			itsGetArgsCmd->Send();
			itsGetArgsCmd = NULL;	// one shot
			}
		}

	else if (sender == itsLink && message.Is(CMLink::kProgramStopped))
		{
		itsCommandInput->Focus();

		JBoolean active = kJFalse;
		if (itsCurrentSourceDir->IsActive())
			{
			(itsCurrentSourceDir->GetWindow())->Raise(kJFalse);
			active = kJTrue;
			}
		if (itsCurrentAsmDir->IsActive() &&
			(!itsCurrentAsmDir->GetWindow()->IsDocked() ||
			 !itsCurrentSourceDir->IsActive()))
			{
			(itsCurrentAsmDir->GetWindow())->Raise(kJFalse);
			active = kJTrue;
			}
		if (!active)
			{
			itsCurrentSourceDir->Activate();
			}
		}

	// CMCommandInput

	else if (sender == itsCommandInput && message.Is(CMCommandInput::kReturnKeyPressed))
		{
		HandleUserInput();
		}
	else if (sender == itsCommandInput && message.Is(CMCommandInput::kTabKeyPressed))
		{
		HandleCompletionRequest();
		}
	else if (sender == itsCommandInput &&
			 (message.Is(JXWidget::kGotFocus) ||
			  message.Is(JXWidget::kLostFocus)))
		{
		itsDownRect->SetBackColor(itsCommandInput->GetCurrBackColor());
		itsFakePrompt->SetBackColor(itsCommandInput->GetCurrBackColor());
		}
	else if (sender == itsCommandInput && message.Is(CMCommandInput::kUpArrowKeyPressed))
		{
		ShowHistoryCommand(+1);
		}
	else if (sender == itsCommandInput && message.Is(CMCommandInput::kDownArrowKeyPressed))
		{
		ShowHistoryCommand(-1);
		}

	// menus

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
		UpdateDebugMenu(itsDebugMenu, itsCommandOutput, itsCommandInput);
		}
	else if (sender == itsDebugMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleDebugMenu(itsDebugMenu, selection->GetIndex(), itsCommandOutput, itsCommandInput);
		}

	else if (sender == CMGetPrefsManager() && message.Is(CMPrefsManager::kCustomCommandsChanged))
		{
		AdjustDebugMenu(itsDebugMenu);
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

	else if (sender == itsHistoryMenu && message.Is(JXMenu::kItemSelected))
		{
		const JString& str = itsHistoryMenu->GetItemText(message);
		itsCommandInput->SetText(str);
		itsCommandInput->SetCaretLocation(str.GetLength()+1);
		}

	// misc

	else if (sender == itsProgramButton && message.Is(JXButton::kPushed))
		{
		ChangeProgram();
		}

	else if (message.Is(CMGetFullPath::kFileFound))
		{
		const CMGetFullPath::FileFound* info =
			dynamic_cast<const CMGetFullPath::FileFound*>(&message);
		assert( info != NULL );
		OpenSourceFile(info->GetFullName(), info->GetLineIndex());
		}
	else if (message.Is(CMGetFullPath::kFileNotFound))
		{
		const CMGetFullPath::FileNotFound* info =
			dynamic_cast<const CMGetFullPath::FileNotFound*>(&message);
		assert( info != NULL );
		ReportUnreadableSourceFile(info->GetFileName());
		}
	else if (message.Is(CMGetFullPath::kNewCommand))
		{
		const CMGetFullPath::NewCommand* info =
			dynamic_cast<const CMGetFullPath::NewCommand*>(&message);
		assert( info != NULL );
		ListenTo(info->GetNewCommand());
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
CMCommandDirector::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !CMIsShuttingDown())
		{
		itsLink = CMGetLink();
		ListenTo(itsLink);

		itsWaitingToRunFlag = kJFalse;

		UpdateWindowTitle("");
		itsFakePrompt->SetText(itsLink->GetPrompt());
		itsFakePrompt->SetFontStyle(
			CMGetPrefsManager()->GetColor(CMPrefsManager::kRightMarginColorIndex));

		itsProgramButton->SetActive(itsLink->GetFeature(CMLink::kSetProgram));
		itsArgInput->SetText("");

		jdelete itsGetArgsCmd;
		itsGetArgsCmd = itsLink->CreateGetInitArgs(itsArgInput);
		}
	else
		{
		JXWindowDirector::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 HandleUserInput (private)

 ******************************************************************************/

void
CMCommandDirector::HandleUserInput()
{
	itsHistoryIndex = 0;
	JString input   = itsCommandInput->GetText();

	if ((!itsLink->IsDebugging() || itsLink->ProgramIsStopped()) &&
		!itsLink->IsDefiningScript())
		{
		itsCommandOutput->PlaceCursorAtEnd();
//		itsISOStyler->FilterISO("\n" + itsLink->GetPrompt() + " ");
		itsCommandOutput->SetCurrentFontStyle(itsCommandOutput->GetDefaultFont().GetStyle());
		itsCommandOutput->Paste("\n" + itsLink->GetPrompt() + " ");
		}
	else
		{
		itsCommandOutput->SetCaretLocation(itsCommandOutput->GetTextLength() + 1);
		}
//	itsISOStyler->FilterISO(input);
	itsCommandOutput->SetCurrentFontStyle(itsCommandOutput->GetDefaultFont().GetStyle());
	itsCommandOutput->Paste(input);

	itsCommandOutput->Paste("\n");
	input.TrimWhitespace();
	if (!input.IsEmpty())
		{
		itsHistoryMenu->AddString(input);
		}
	itsLink->SendRaw(input);

	itsCommandInput->SetText("");
}

/******************************************************************************
 HandleCompletionRequest (private)

 ******************************************************************************/

void
CMCommandDirector::HandleCompletionRequest()
{
	const JString& input = itsCommandInput->GetText();
	if (!input.EndsWith(" ") &&
		!input.EndsWith("\t") &&
		!input.EndsWith(":"))
		{
		CMGetCompletions* cmd =
			itsLink->CreateGetCompletions(itsCommandInput, itsCommandOutput);
		cmd->Send();
		}
	else
		{
		GetDisplay()->Beep();
		}
}

/******************************************************************************
 ShowHistoryCommand (private)

 ******************************************************************************/

void
CMCommandDirector::ShowHistoryCommand
	(
	const JInteger delta
	)
{
	if (itsHistoryIndex == 0)
		{
		itsCurrentCommand = itsCommandInput->GetText();
		}

	const JInteger i = JInteger(itsHistoryIndex) + delta;
	if (0 < i && i <= (JInteger) itsHistoryMenu->GetItemCount())
		{
		itsHistoryIndex      = i;
		const JString& input = itsHistoryMenu->JXTextMenu::GetItemText(itsHistoryIndex);
		itsCommandInput->SetText(input);
		}
	else if (i <= 0)
		{
		itsHistoryIndex = 0;
		itsCommandInput->SetText(itsCurrentCommand);
		}

	itsCommandInput->SetCaretLocation(itsCommandInput->GetTextLength()+1);
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
CMCommandDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kCloseCmd, !GetWindow()->IsDocked());
	itsFileMenu->SetItemEnable(kLoadConfigCmd, itsLink->HasProgram());
}

/******************************************************************************
 HandleFileMenu

 ******************************************************************************/

void
CMCommandDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenCmd)
		{
		OpenSourceFiles();
		}

	else if (index == kLoadConfigCmd)
		{
		LoadConfig();
		}
	else if (index == kSaveConfigCmd)
		{
		SaveConfig();
		}

	else if (index == kSaveCmd)
		{
		SaveInCurrentFile();
		}
	else if (index == kSaveAsCmd)
		{
		SaveInNewFile();
		}

	else if (index == kPageSetupCmd)
		{
		itsCommandOutput->HandlePTPageSetup();
		}
	else if (index == kPrintCmd)
		{
		itsCommandOutput->PrintPT();
		}

	else if (index == kCloseCmd)
		{
		Deactivate();
		}
	else if (index == kQuitCmd)
		{
		JXGetApplication()->Quit();
		}
}

/******************************************************************************
 OpenSourceFiles

 ******************************************************************************/

void
CMCommandDirector::OpenSourceFiles()
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	if ((JGetChooseSaveFile())->ChooseFiles("Choose Source Files", NULL, &list))
		{
		const JSize count = list.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			OpenSourceFile(*(list.GetElement(i)));
			}
		}
}

/******************************************************************************
 OpenSourceFile

 ******************************************************************************/

void
CMCommandDirector::OpenSourceFile
	(
	const JCharacter*	fileName,
	const JSize			lineIndex,
	const JBoolean		askDebuggerWhenRelPath
	)
{
	JString fullName;
	if (JIsRelativePath(fileName) && askDebuggerWhenRelPath)
		{
		JBoolean exists;
		JString fullName;
		const JBoolean known = itsLink->FindFile(fileName, &exists, &fullName);
		if (known && exists)
			{
			OpenSourceFile(fullName, lineIndex);
			}
		else if (known)
			{
			ReportUnreadableSourceFile(fileName);
			}
		else
			{
			CMGetFullPath* cmd	= itsLink->CreateGetFullPath(fileName, lineIndex);
			ListenTo(cmd);
			}
		}
	else if (JConvertToAbsolutePath(fileName, NULL, &fullName))
		{
		const JSize count = itsSourceDirs->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			CMSourceDirector* dir = itsSourceDirs->GetElement(i);
			const JString* f;
			if (dir->GetFileName(&f) && JSameDirEntry(fullName, *f))
				{
				dir->Activate();
				if (lineIndex > 0)
					{
					dir->DisplayLine(lineIndex);
					}
				return;
				}
			}

		CMSourceDirector* dir = CMSourceDirector::Create(this, fullName, CMSourceDirector::kSourceType);
		itsSourceDirs->Append(dir);
		dir->Activate();
		if (lineIndex > 0)
			{
			dir->DisplayLine(lineIndex);
			}
		}
	else
		{
		ReportUnreadableSourceFile(fileName);
		}
}

/******************************************************************************
 DisassembleFunction

 ******************************************************************************/

void
CMCommandDirector::DisassembleFunction
	(
	const JCharacter* origFn,
	const JCharacter* addr
	)
{
	CMSourceDirector* dir = NULL;

	JString fn = origFn;
	JIndex i;
	if (fn.LocateSubstring("(", &i))
		{
		fn.RemoveSubstring(i, fn.GetLength());
		}

	const JSize count = itsAsmDirs->GetElementCount();
	for (i=1; i<=count; i++)
		{
		dir = itsAsmDirs->GetElement(i);
		const JString* f;
		if (dir->GetFunctionName(&f) && fn == *f)
			{
			dir->Activate();
			break;
			}
		dir = NULL;		// trigger Create()
		}

	if (dir == NULL)
		{
		dir = CMSourceDirector::Create(this, JString::IsEmpty(addr) ? fn.GetCString() : NULL, CMSourceDirector::kAsmType);
		itsAsmDirs->Append(dir);
		dir->Activate();
		}

	if (!JString::IsEmpty(addr))
		{
		CMLocation loc;
		loc.SetFunctionName(fn);
		loc.SetMemoryAddress(addr);
		dir->DisplayDisassembly(loc);
		}
}

/******************************************************************************
 ReportUnreadableSourceFile

 ******************************************************************************/

void
CMCommandDirector::ReportUnreadableSourceFile
	(
	const JCharacter* fileName
	)
	const
{
	const JCharacter* map[] =
		{
		"name", fileName
		};
	const JString err = JGetString("CannotOpenFile::CMCommandDirector", map, sizeof(map));
	(JGetUserNotification())->ReportError(err);
}

/******************************************************************************
 LoadConfig

 ******************************************************************************/

void
CMCommandDirector::LoadConfig()
{
	JString fullName;
	if ((JGetChooseSaveFile())->ChooseFile("Choose configuration file", NULL, &fullName))
		{
		std::ifstream input(fullName);

		JFileVersion vers;
		input >> vers;
		if (vers <= kCurrentConfigVersion)
			{
			(itsLink->GetBreakpointManager())->ReadSetup(input, vers);
			itsVarTreeDir->ReadSetup(input, vers);

			JSize count;
			input >> count;

			for (JIndex i=1; i<=count; i++)
				{
				CMArray1DDir* dir = jnew CMArray1DDir(input, vers, this);
				assert( dir != NULL );
				// adds itself to list automatically
				dir->Activate();
				}

			input >> count;

			for (JIndex i=1; i<=count; i++)
				{
				CMArray2DDir* dir = jnew CMArray2DDir(input, vers, this);
				assert( dir != NULL );
				// adds itself to list automatically
				dir->Activate();
				}

			input >> count;

			for (JIndex i=1; i<=count; i++)
				{
				CMPlot2DDir* dir = jnew CMPlot2DDir(input, vers, this);
				assert( dir != NULL );
				// adds itself to list automatically
				dir->Activate();
				}

			if (vers >= 3)
				{
				input >> count;

				for (JIndex i=1; i<=count; i++)
					{
					CMMemoryDir* dir = jnew CMMemoryDir(input, vers, this);
					assert( dir != NULL );
					// adds itself to list automatically
					dir->Activate();
					}
				}
			}
		else
			{
			(JGetUserNotification())->DisplayMessage(
				"This configuration file was created by a newer version of "
				"Code Medic.  You need the newest version in order to open it.");
			}
		}
}

/******************************************************************************
 SaveConfig

 ******************************************************************************/

void
CMCommandDirector::SaveConfig()
{
	JString fullName, path, origName;
	if (itsLink->GetProgram(&fullName))
		{
		JSplitPathAndName(fullName, &path, &origName);
		origName += "_debug_config";
		}
	else
		{
		origName = "debug_config";
		}

	if ((JGetChooseSaveFile())->SaveFile("Choose file name", NULL, origName, &fullName))
		{
		std::ofstream output(fullName);

		output << kCurrentConfigVersion;
		(itsLink->GetBreakpointManager())->WriteSetup(output);
		itsVarTreeDir->WriteSetup(output);

		JSize count = itsArray1DDirs->GetElementCount();
		output << ' ' << count;

		for (JIndex i=1; i<=count; i++)
			{
			(itsArray1DDirs->GetElement(i))->StreamOut(output);
			}

		count = itsArray2DDirs->GetElementCount();
		output << ' ' << count;

		for (JIndex i=1; i<=count; i++)
			{
			(itsArray2DDirs->GetElement(i))->StreamOut(output);
			}

		count = itsPlot2DDirs->GetElementCount();
		output << ' ' << count;

		for (JIndex i=1; i<=count; i++)
			{
			(itsPlot2DDirs->GetElement(i))->StreamOut(output);
			}

		count = itsMemoryDirs->GetElementCount();
		output << ' ' << count;

		for (JIndex i=1; i<=count; i++)
			{
			(itsMemoryDirs->GetElement(i))->StreamOut(output);
			}
		}
}

/******************************************************************************
 SaveInCurrentFile

 ******************************************************************************/

void
CMCommandDirector::SaveInCurrentFile()
{
	if (itsCurrentHistoryFile.IsEmpty())
		{
		SaveInNewFile();
		}
	else
		{
		itsCommandOutput->WritePlainText(itsCurrentHistoryFile, JTextEditor::kUNIXText);
		}
}

/******************************************************************************
 SaveInNewFile

 ******************************************************************************/

void
CMCommandDirector::SaveInNewFile()
{
	if ((JGetChooseSaveFile())->SaveFile("Choose file name:", NULL, "",  &itsCurrentHistoryFile))
		{
		SaveInCurrentFile();
		}
}

/******************************************************************************
 UpdateDebugMenu

	te1 and te2 can both be NULL.  If not, they are used to look for
	selected text to send to the Variable Tree window.

	Shared with CMSourceDirector

 ******************************************************************************/

void
CMCommandDirector::UpdateDebugMenu
	(
	JXTextMenu*	menu,
	JXTEBase*	te1,
	JXTEBase*	te2
	)
{
	if (itsLink->GetFeature(CMLink::kSetArgs))
		{
		menu->EnableItem(kSetArgsCmd);
		}

	if (itsLink->DebuggerHasStarted())
		{
		const JBoolean canSetProgram = itsLink->GetFeature(CMLink::kSetProgram);
		if (canSetProgram)
			{
			menu->EnableItem(kSelectBinCmd);
			}

		if (itsLink->GetFeature(CMLink::kSetCore))
			{
			menu->EnableItem(kSelectCoreCmd);
			}
		if (itsLink->GetFeature(CMLink::kSetProcess))
			{
			menu->EnableItem(kSelectProcessCmd);
			}

		menu->EnableItem(kRestartDebuggerCmd);
		if (itsLink->GetFeature(CMLink::kRunProgram))
			{
			menu->EnableItem(kRunCmd);
			}

		const JSize count = menu->GetItemCount();
		for (JIndex i=kFirstCustomDebugCmd; i<=count; i++)
			{
			menu->EnableItem(i);
			}

		menu->EnableItem(kRemoveAllBreakpointsCmd);
		if (itsLink->HasProgram())
			{
			if (canSetProgram)
				{
				menu->EnableItem(kReloadBinCmd);
				}
			if (itsLink->GetFeature(CMLink::kExamineMemory))
				{
				menu->EnableItem(kExamineMemCmd);
				}
			if (itsLink->GetFeature(CMLink::kDisassembleMemory))
				{
				menu->EnableItem(kDisassembleMemCmd);
				}

			if (itsLink->HasLoadedSymbols())
				{
				JString text;
				if (((te1 != NULL && te1->GetSelection(&text)) ||
					 (te2 != NULL && te2->GetSelection(&text))) &&
					 !text.Contains("\n"))
					{
					menu->EnableItem(kDisplayVarCmd);
					menu->EnableItem(kDisplay1DArrayCmd);
					menu->EnableItem(kPlot1DArrayCmd);
					menu->EnableItem(kDisplay2DArrayCmd);
					menu->EnableItem(kWatchVarCmd);
					menu->EnableItem(kWatchLocCmd);

					if (itsLink->GetFeature(CMLink::kDisassembleMemory))
						{
						menu->EnableItem(kDisassembleFnCmd);
						}
					}

				if (itsLink->IsDebugging())
					{
					if (itsLink->GetFeature(CMLink::kStopProgram))
						{
						menu->EnableItem(kKillCmd);
						}

					if (itsLink->ProgramIsStopped())
						{
						menu->EnableItem(kNextCmd);
						menu->EnableItem(kStepCmd);
						menu->EnableItem(kFinishCmd);
						menu->EnableItem(kContCmd);

						if (itsLink->GetFeature(CMLink::kDisassembleMemory))
							{
							menu->EnableItem(kNextAsmCmd);
							menu->EnableItem(kStepAsmCmd);
							}

						const JBoolean bkwd = itsLink->GetFeature(CMLink::kExecuteBackwards);
						menu->SetItemEnable(kPrevCmd, bkwd);
						menu->SetItemEnable(kReverseStepCmd, bkwd);
						menu->SetItemEnable(kReverseFinishCmd, bkwd);
						menu->SetItemEnable(kReverseContCmd, bkwd);
						}
					else if (itsLink->GetFeature(CMLink::kStopProgram))
						{
						menu->EnableItem(kStopCmd);
						}
					}
				}
			}
		}
}

/******************************************************************************
 HandleDebugMenu

	te1 and te2 can both be NULL.  If not, they are used to look for
	selected text to send to the Variable Tree window.

	Shared with CMSourceDirector

 ******************************************************************************/

void
CMCommandDirector::HandleDebugMenu
	(
	JXTextMenu*		menu,
	const JIndex	index,
	JXTEBase*		te1,
	JXTEBase*		te2
	)
{
	if (index == kSelectBinCmd)
		{
		ChangeProgram();
		}
	else if (index == kReloadBinCmd)
		{
		ReloadProgram();
		}
	else if (index == kSelectCoreCmd)
		{
		ChooseCoreFile();
		}
	else if (index == kSelectProcessCmd)
		{
		ChooseProcess();
		}
	else if (index == kSetArgsCmd)
		{
		Activate();
		GetWindow()->RequestFocus();
		if (itsArgInput->Focus())
			{
			itsArgInput->SelectAll();
			}
		}

	else if (index == kRestartDebuggerCmd)
		{
		itsLink->RestartDebugger();
		}

	else if (index == kDisplayVarCmd)
		{
		JString text;
		if ((te1 != NULL && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != NULL && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			DisplayExpression(text);
			}
		}
	else if (index == kDisplay1DArrayCmd)
		{
		JString text;
		if ((te1 != NULL && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != NULL && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			Display1DArray(text);
			}
		}
	else if (index == kPlot1DArrayCmd)
		{
		JString text;
		if ((te1 != NULL && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != NULL && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			Plot1DArray(text);
			}
		}
	else if (index == kDisplay2DArrayCmd)
		{
		JString text;
		if ((te1 != NULL && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != NULL && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			Display2DArray(text);
			}
		}

	else if (index == kWatchVarCmd)
		{
		JString text;
		if ((te1 != NULL && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != NULL && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			(CMGetLink())->WatchExpression(text);
			}
		}
	else if (index == kWatchLocCmd)
		{
		JString text;
		if ((te1 != NULL && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != NULL && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			(CMGetLink())->WatchLocation(text);
			}
		}

	else if (index == kExamineMemCmd)
		{
		JString s1, s2;
		if ((te1 != NULL && te1->GetSelection(&s1) && !s1.Contains("\n")) ||
			(te2 != NULL && te2->GetSelection(&s1) && !s1.Contains("\n")))
			{
			s2 = s1;
			}

		CMMemoryDir* dir = jnew CMMemoryDir(this, s2);
		assert(dir != NULL);
		dir->Activate();
		}
	else if (index == kDisassembleMemCmd)
		{
		JString s1, s2;
		if ((te1 != NULL && te1->GetSelection(&s1) && !s1.Contains("\n")) ||
			(te2 != NULL && te2->GetSelection(&s1) && !s1.Contains("\n")))
			{
			s2 = s1;
			}

		CMMemoryDir* dir = jnew CMMemoryDir(this, s2);
		assert(dir != NULL);
		dir->SetDisplayType(CMMemoryDir::kAsm);
		dir->Activate();
		}
	else if (index == kDisassembleFnCmd)
		{
		JString fn;
		if ((te1 != NULL && te1->GetSelection(&fn) && !fn.Contains("\n")) ||
			(te2 != NULL && te2->GetSelection(&fn) && !fn.Contains("\n")))
			{
			DisassembleFunction(fn);
			}
		}

	else if (index == kRunCmd)
		{
		RunProgram();
		}
	else if (index == kStopCmd)
		{
		itsLink->StopProgram();
		}
	else if (index == kKillCmd)
		{
		itsLink->KillProgram();
		}

	else if (index == kNextCmd)
		{
		itsLink->StepOver();
		}
	else if (index == kStepCmd)
		{
		itsLink->StepInto();
		}
	else if (index == kFinishCmd)
		{
		itsLink->StepOut();
		}
	else if (index == kContCmd)
		{
		itsLink->Continue();
		}

	else if (index == kNextAsmCmd)
		{
		itsLink->StepOverAssembly();
		}
	else if (index == kStepAsmCmd)
		{
		itsLink->StepIntoAssembly();
		}

	else if (index == kPrevCmd)
		{
		itsLink->BackupOver();
		}
	else if (index == kReverseStepCmd)
		{
		itsLink->BackupInto();
		}
	else if (index == kReverseFinishCmd)
		{
		itsLink->BackupOut();
		}
	else if (index == kReverseContCmd)
		{
		itsLink->BackupContinue();
		}

	else if (index == kRemoveAllBreakpointsCmd)
		{
		itsLink->RemoveAllBreakpoints();
		}

	else if (index >= kFirstCustomDebugCmd)
		{
		JString s = menu->GetItemText(index);

		JString text;
		if ((te1 != NULL && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != NULL && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			JIndex i;
			while (s.LocateSubstring("##", &i))
				{
				s.ReplaceSubstring(i, i+1, text);
				}
			}
		else if (s.Contains("##"))
			{
			(JGetUserNotification())->ReportError(JGetString("NoSelection::CMCommandDirector"));
			return;
			}

		itsLink->SendRaw(s);
		}
}

/******************************************************************************
 RunProgram

 ******************************************************************************/

void
CMCommandDirector::RunProgram()
{
	if (!itsLink->HasProgram())
		{
		ChangeProgram();
		if (itsLink->HasProgram())
			{
			itsWaitingToRunFlag = kJTrue;
			}
		}
	else if ((CMGetLink())->HasPendingCommands())
		{
		CMRunProgramTask* task = jnew CMRunProgramTask();
		assert( task != NULL );
		task->Start();
		}
	else if (itsLink->OKToDetachOrKill())
		{
		itsLink->RunProgram(itsArgInput->GetText());
		}
}

/******************************************************************************
 ChangeProgram

 ******************************************************************************/

void
CMCommandDirector::ChangeProgram()
{
	JString fullName;
	const JString instr = itsLink->GetChooseProgramInstructions();
	if (itsLink->OKToDetachOrKill() &&
		(JGetChooseSaveFile())->ChooseFile("Choose binary", instr, &fullName))
		{
		CMMDIServer::UpdateDebuggerType(fullName);
		itsLink->SetProgram(fullName);
		}
}

/******************************************************************************
 ReloadProgram

 ******************************************************************************/

void
CMCommandDirector::ReloadProgram()
{
	if (itsLink->OKToDetachOrKill())
		{
		itsLink->ReloadProgram();
		}
}

/******************************************************************************
 ChooseCoreFile

 ******************************************************************************/

void
CMCommandDirector::ChooseCoreFile()
{
	JString coreName, origPath;

#ifdef _J_OSX

	origPath = "/cores/*";

#endif

	if (itsLink->OKToDetachOrKill() &&
		(JGetChooseSaveFile())->ChooseFile("Choose core file", NULL, origPath, &coreName))
		{
		itsLink->SetCore(coreName);
		}
}

/******************************************************************************
 ChooseProcess

 ******************************************************************************/

void
CMCommandDirector::ChooseProcess()
{
	if (itsLink->OKToDetachOrKill())
		{
		CMChooseProcessDialog* dialog = jnew CMChooseProcessDialog(this);
		assert( dialog != NULL );
		dialog->BeginDialog();
		}
}

/******************************************************************************
 DisplayExpression

 ******************************************************************************/

void
CMCommandDirector::DisplayExpression
	(
	const JCharacter* expr
	)
{
	itsVarTreeDir->Activate();
	itsVarTreeDir->DisplayExpression(expr);
}

/******************************************************************************
 Display1DArray

 ******************************************************************************/

void
CMCommandDirector::Display1DArray
	(
	const JCharacter* expr
	)
{
	CMArray1DDir* dir = jnew CMArray1DDir(this, expr);
	assert( dir != NULL );
	dir->Activate();
}

/******************************************************************************
 Display2DArray

 ******************************************************************************/

void
CMCommandDirector::Display2DArray
	(
	const JCharacter* expr
	)
{
	CMArray2DDir* dir = jnew CMArray2DDir(this, expr);
	assert( dir != NULL );
	dir->Activate();
}

/******************************************************************************
 Plot1DArray

 ******************************************************************************/

void
CMCommandDirector::Plot1DArray
	(
	const JCharacter* expr
	)
{
	CMPlot2DDir* dir = jnew CMPlot2DDir(this, expr);
	assert( dir != NULL );
	dir->Activate();
}

/******************************************************************************
 UpdatePrefsMenu

 ******************************************************************************/

void
CMCommandDirector::UpdatePrefsMenu()
{
	CMPrefsManager::DebuggerType type = CMGetPrefsManager()->GetDebuggerType();
	itsPrefsMenu->CheckItem(kDebuggerTypeToMenuIndex[ type ]);

//	itsPrefsMenu->DisableItem(kUseJavaCmd);
}

/******************************************************************************
 HandlePrefsMenu

 ******************************************************************************/

void
CMCommandDirector::HandlePrefsMenu
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
		assert( dlog != NULL );
		dlog->BeginDialog();
		}
	else if (index == kEditMacWinPrefsCmd)
		{
		JXMacWinPrefsDialog::EditPrefs();
		}
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
CMCommandDirector::HandleHelpMenu
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
		(JXGetHelpManager())->ShowSection("CMCommandDirHelp");
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
