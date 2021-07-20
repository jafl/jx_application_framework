/******************************************************************************
 CMCommandDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997-2004 by John Lindal.

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
#include <JXColorManager.h>
#include <JXMacWinPrefsDialog.h>

#include <JStringIterator.h>
#include <JFontManager.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

const JSize kCmdHistoryLength = 100;

// File menu

static const JUtf8Byte* kFileMenuStr =
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

static const JUtf8Byte* kDebugMenuStr =
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

// Prefs menu

static const JUtf8Byte* kPrefsMenuStr =
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

static const JUtf8Byte* kHelpMenuStr =
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
	assert( itsCurrentSourceDir != nullptr );
	itsCurrentSourceDir->Activate();

	itsCurrentAsmDir = jnew CMSourceDirector(this, CMSourceDirector::kMainAsmType);
	assert( itsCurrentAsmDir != nullptr );

	itsThreadsDir = jnew CMThreadsDir(this);
	assert( itsThreadsDir != nullptr );

	itsStackDir = jnew CMStackDir(this);
	assert(itsStackDir != nullptr);

	itsBreakpointsDir = jnew CMBreakpointsDir(this);
	assert(itsBreakpointsDir != nullptr);

	itsVarTreeDir = jnew CMVarTreeDir(this);
	assert(itsVarTreeDir != nullptr);

	itsLocalVarsDir = jnew CMLocalVarsDir(this);
	assert(itsLocalVarsDir != nullptr);

	itsRegistersDir = jnew CMRegistersDir(this);
	assert(itsRegistersDir != nullptr);

	itsFileListDir = jnew CMFileListDir(this);
	assert(itsFileListDir != nullptr);

	itsDebugDir = jnew CMDebugDir(this);
	assert(itsDebugDir!=nullptr);

	JXWDManager* wdMgr = GetDisplay()->GetWDManager();
	wdMgr->PermanentDirectorCreated(this,                JString::empty,                            kCMShowCommandLineAction);
	wdMgr->PermanentDirectorCreated(itsCurrentSourceDir, JString::empty,                            kCMShowCurrentSourceAction);
	wdMgr->PermanentDirectorCreated(itsThreadsDir,       JString("Meta-Shift-T", JString::kNoCopy), kCMShowThreadsAction);
	wdMgr->PermanentDirectorCreated(itsStackDir,         JString("Meta-Shift-S", JString::kNoCopy), kCMShowStackTraceAction);
	wdMgr->PermanentDirectorCreated(itsBreakpointsDir,   JString("Meta-Shift-B", JString::kNoCopy), kCMShowBreakpointsAction);
	wdMgr->PermanentDirectorCreated(itsVarTreeDir,       JString("Meta-Shift-V", JString::kNoCopy), kCMShowVariablesAction);
	wdMgr->PermanentDirectorCreated(itsLocalVarsDir,     JString("Meta-Shift-L", JString::kNoCopy), kCMShowLocalVariablesAction);
	wdMgr->PermanentDirectorCreated(itsCurrentAsmDir,    JString("Meta-Shift-Y", JString::kNoCopy), kCMShowCurrentAsmAction);
	wdMgr->PermanentDirectorCreated(itsRegistersDir,     JString("Meta-Shift-R", JString::kNoCopy), kCMShowRegistersAction);
	wdMgr->PermanentDirectorCreated(itsFileListDir,      JString("Meta-Shift-F", JString::kNoCopy), kCMShowFileListAction);
	wdMgr->PermanentDirectorCreated(itsDebugDir,         JString::empty,                            kCMShowDebugInfoAction);

	CreateWindowsMenuAndToolBar(menuBar, itsToolBar, false, false, true,
								itsDebugMenu, itsPrefsMenu,
								itsHelpMenu, kTOCCmd, kThisWindowCmd);
	itsCurrentSourceDir->CreateWindowsMenu();
	itsCurrentAsmDir->CreateWindowsMenu();

	itsSourceDirs = jnew JPtrArray<CMSourceDirector>(JPtrArrayT::kForgetAll);
	assert( itsSourceDirs != nullptr );

	itsAsmDirs = jnew JPtrArray<CMSourceDirector>(JPtrArrayT::kForgetAll);
	assert( itsAsmDirs != nullptr );

	itsArray1DDirs = jnew JPtrArray<CMArray1DDir>(JPtrArrayT::kForgetAll);
	assert( itsArray1DDirs != nullptr );

	itsArray2DDirs = jnew JPtrArray<CMArray2DDir>(JPtrArrayT::kForgetAll);
	assert( itsArray2DDirs != nullptr );

	itsPlot2DDirs = jnew JPtrArray<CMPlot2DDir>(JPtrArrayT::kForgetAll);
	assert( itsPlot2DDirs != nullptr );

	itsMemoryDirs = jnew JPtrArray<CMMemoryDir>(JPtrArrayT::kForgetAll);
	assert( itsMemoryDirs != nullptr );

	itsHistoryIndex     = 0;
	itsWaitingToRunFlag = false;

	InitializeCommandOutput();

	itsGetArgsCmd = itsLink->CreateGetInitArgs(itsArgInput);

	ListenTo(CMGetPrefsManager());

	itsFakePrompt->GetText()->SetText(itsLink->GetPrompt());
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
	jdelete itsGetArgsCmd;
}

/******************************************************************************
 Close (virtual)

 ******************************************************************************/

bool
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
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsCommandInput->Focus())
		{
		itsCommandInput->HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 InitializeCommandOutput

 ******************************************************************************/

void
CMCommandDirector::InitializeCommandOutput()
{
	itsCommandOutput->GetText()->SetText(JString::empty);

	const JUtf8Byte* map[] =
		{
		"vers", CMGetVersionNumberStr().GetBytes()
		};
	const JString welcome = JGetString("Welcome::CMCommandDirector", map, sizeof(map));
	itsCommandOutput->GetText()->SetText(welcome);
	itsCommandOutput->SetCaretLocation(welcome.GetCharacterCount()+1);
}

/******************************************************************************
 PrepareCommand

 ******************************************************************************/

void
CMCommandDirector::PrepareCommand
	(
	const JString& cmd
	)
{
	itsCommandInput->GetText()->SetText(cmd);
	itsCommandInput->Focus();
	itsCommandInput->GoToEndOfLine();
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

	auto* window = jnew JXWindow(this, 500,550, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(CMGetPrefsManager(), kCmdWindowToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,440);
	assert( itsToolBar != nullptr );

	itsProgramButton =
		jnew JXTextButton(JGetString("itsProgramButton::CMCommandDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 0,530, 150,20);
	assert( itsProgramButton != nullptr );

	itsDownRect =
		jnew JXDownRect(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 0,470, 500,60);
	assert( itsDownRect != nullptr );

	itsFakePrompt =
		jnew JXStaticText(JGetString("itsFakePrompt::CMCommandDirector::JXLayout"), itsDownRect,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,0, 38,20);
	assert( itsFakePrompt != nullptr );
	itsFakePrompt->SetToLabel();

	itsCommandInput =
		jnew CMCommandInput(itsDownRect,
					JXWidget::kHElastic, JXWidget::kFixedTop, 53,0, 445,56);
	assert( itsCommandInput != nullptr );

	itsHistoryMenu =
		jnew JXStringHistoryMenu(kCmdHistoryLength, JString::empty, itsDownRect,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 30,20);
	assert( itsHistoryMenu != nullptr );

	itsArgInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 150,530, 350,20);
	assert( itsArgInput != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitleSuffix::CMCommandDirector"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(CMGetWMClassInstance(), CMGetCommandWindowClass());
	window->SetMinSize(300, 200);
	CMGetPrefsManager()->GetWindowSize(kCmdWindSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_command_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	itsDownRect->SetBackColor(itsCommandInput->GetCurrBackColor());
	itsFakePrompt->SetBackColor(itsCommandInput->GetCurrBackColor());
	CMTextDisplayBase::AdjustFont(itsFakePrompt);
	ListenTo(itsCommandInput);

	itsHistoryMenu->SetPopupArrowDirection(JXMenu::kArrowPointsUp);
	itsHistoryMenu->SetHistoryDirection(JXStringHistoryMenu::kNewestItemAtBottom);
	CMGetPrefsManager()->ReadHistoryMenuSetup(itsHistoryMenu);
	ListenTo(itsHistoryMenu);

	itsProgramButton->SetFontName(JFontManager::GetDefaultMonospaceFontName());
	itsProgramButton->SetFontSize(JFontManager::GetDefaultMonospaceFontSize());
	itsProgramButton->SetActive(itsLink->GetFeature(CMLink::kSetProgram));
	ListenTo(itsProgramButton);

	JPoint p = itsProgramButton->GetPadding();
	p.y      = 0;
	itsProgramButton->SetPaddingBeforeFTC(p);

	CMTextDisplayBase::AdjustFont(itsArgInput);
	itsArgInput->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

	// menus

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "CMCommandDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd,  jx_file_open);
	itsFileMenu->SetItemImage(kPrintCmd, jx_file_print);

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure();

	// appends Edit & Search menus

	itsCommandOutput =
		jnew CMCommandOutputDisplay(menuBar,
							 scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( itsCommandOutput != nullptr );
	itsCommandOutput->FitToEnclosure(true, true);

	itsCommandOutput->AppendSearchMenu(menuBar);

	itsDebugMenu = CreateDebugMenu(menuBar);
	ListenTo(itsDebugMenu);

	itsCommandInput->ShareEditMenu(itsCommandOutput);
	itsArgInput->ShareEditMenu(itsCommandOutput);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "CMCommandDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
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
	const JString& programName
	)
{
	JString title = programName;
	title += JGetString("WindowTitleSuffix::CMCommandDirector");
	GetWindow()->SetTitle(title);

	if (!programName.IsEmpty())
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

bool
CMCommandDirector::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = CMGetCommandLineIcon();
	return true;
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
	JXTextMenu* menu = menuBar->AppendTextMenu(JGetString("DebugMenuTitle::CMCommandDirector"));
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
	m.SetState(kJXMetaKeyIndex, true);
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
	const bool	includeStepAsm
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
		menu->AppendItem(*cmd, JXMenu::kPlainType, JString::empty, JString::empty, *cmd);
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
	const bool	includeStepAsm,
	const bool	includeCmdLine,
	const bool	includeCurrSrc,
	JXTextMenu*		debugMenu,
	JXTextMenu*		prefsMenu,
	JXTextMenu*		helpMenu,
	const JIndex	tocCmd,
	const JIndex	thisWindowCmd
	)
{
	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
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
	const bool	includeCmdLine,
	const bool	includeCurrSrc
	)
{
	if (includeCmdLine)
		{
		toolBar->AppendButton(windowsMenu, JString(kCMShowCommandLineAction, JString::kNoCopy));
		}
	if (includeCurrSrc)
		{
		toolBar->AppendButton(windowsMenu, JString(kCMShowCurrentSourceAction, JString::kNoCopy));
		}
	toolBar->AppendButton(windowsMenu, JString(kCMShowStackTraceAction, JString::kNoCopy));
	toolBar->AppendButton(windowsMenu, JString(kCMShowBreakpointsAction, JString::kNoCopy));
	toolBar->AppendButton(windowsMenu, JString(kCMShowVariablesAction, JString::kNoCopy));
	toolBar->AppendButton(windowsMenu, JString(kCMShowLocalVariablesAction, JString::kNoCopy));
	toolBar->AppendButton(windowsMenu, JString(kCMShowFileListAction, JString::kNoCopy));
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
		const auto* output =
			dynamic_cast<const CMLink::UserOutput*>(&message);
		assert(output != nullptr);
		itsCommandOutput->SetCaretLocation(
			itsCommandOutput->GetText()->GetText().GetCharacterCount()+1);

		JFont font = itsCommandOutput->GetText()->GetDefaultFont();
		font.SetBold(output->IsFromTarget());
		font.SetColor(output->IsError() ? JColorManager::GetDarkRedColor() : JColorManager::GetBlackColor());
		itsCommandOutput->SetCurrentFont(font);
		itsCommandOutput->Paste(output->GetText());
		itsCommandOutput->GetText()->ClearUndo();
		}

	else if (sender == itsLink && message.Is(CMLink::kDebuggerBusy))
		{
		itsFakePrompt->SetFontStyle(
			CMGetPrefsManager()->GetColor(CMPrefsManager::kRightMarginColorIndex));
		}
	else if (sender == itsLink && message.Is(CMLink::kDebuggerReadyForInput))
		{
		itsFakePrompt->GetText()->SetText(itsLink->GetPrompt());
		itsFakePrompt->SetFontStyle(
			CMGetPrefsManager()->GetColor(CMPrefsManager::kTextColorIndex));
		}
	else if (sender == itsLink && message.Is(CMLink::kDebuggerDefiningScript))
		{
		itsFakePrompt->GetText()->SetText(itsLink->GetScriptPrompt());
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
		const auto* info =
			dynamic_cast<const CMLink::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		UpdateWindowTitle(info->GetProgramName());

		if (itsWaitingToRunFlag)
			{
			auto* task = jnew CMRunProgramTask();
			assert( task != nullptr );
			task->Start();
			itsWaitingToRunFlag = false;
			}
		}

	else if (sender == itsLink && message.Is(CMLink::kDebuggerStarted))
		{
		if (itsGetArgsCmd != nullptr)
			{
			itsGetArgsCmd->Send();
			itsGetArgsCmd = nullptr;	// one shot
			}
		}

	else if (sender == itsLink && message.Is(CMLink::kProgramStopped))
		{
		itsCommandInput->Focus();

		bool active = false;
		if (itsCurrentSourceDir->IsActive())
			{
			(itsCurrentSourceDir->GetWindow())->Raise(false);
			active = true;
			}
		if (itsCurrentAsmDir->IsActive() &&
			(!itsCurrentAsmDir->GetWindow()->IsDocked() ||
			 !itsCurrentSourceDir->IsActive()))
			{
			(itsCurrentAsmDir->GetWindow())->Raise(false);
			active = true;
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
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsDebugMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateDebugMenu(itsDebugMenu, itsCommandOutput, itsCommandInput);
		}
	else if (sender == itsDebugMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
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

	else if (sender == itsHistoryMenu && message.Is(JXMenu::kItemSelected))
		{
		const JString& str = itsHistoryMenu->GetItemText(message);
		itsCommandInput->GetText()->SetText(str);
		itsCommandInput->GoToEndOfLine();
		}

	// misc

	else if (sender == itsProgramButton && message.Is(JXButton::kPushed))
		{
		ChangeProgram();
		}

	else if (message.Is(CMGetFullPath::kFileFound))
		{
		const auto* info =
			dynamic_cast<const CMGetFullPath::FileFound*>(&message);
		assert( info != nullptr );
		OpenSourceFile(info->GetFullName(), info->GetLineIndex());
		}
	else if (message.Is(CMGetFullPath::kFileNotFound))
		{
		const auto* info =
			dynamic_cast<const CMGetFullPath::FileNotFound*>(&message);
		assert( info != nullptr );
		ReportUnreadableSourceFile(info->GetFileName());
		}
	else if (message.Is(CMGetFullPath::kNewCommand))
		{
		const auto* info =
			dynamic_cast<const CMGetFullPath::NewCommand*>(&message);
		assert( info != nullptr );
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

		itsWaitingToRunFlag = false;

		UpdateWindowTitle(JString::empty);
		itsFakePrompt->GetText()->SetText(itsLink->GetPrompt());
		itsFakePrompt->SetFontStyle(
			CMGetPrefsManager()->GetColor(CMPrefsManager::kRightMarginColorIndex));

		itsProgramButton->SetActive(itsLink->GetFeature(CMLink::kSetProgram));
		itsArgInput->GetText()->SetText(JString::empty);

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
	JString input   = itsCommandInput->GetText()->GetText();

	if ((!itsLink->IsDebugging() || itsLink->ProgramIsStopped()) &&
		!itsLink->IsDefiningScript())
		{
		itsCommandOutput->PlaceCursorAtEnd();
		itsCommandOutput->SetCurrentFontStyle(itsCommandOutput->GetText()->GetDefaultFont().GetStyle());
		itsCommandOutput->Paste("\n" + itsLink->GetPrompt() + " ");
		}
	else
		{
		itsCommandOutput->GoToEndOfLine();
		}
	itsCommandOutput->SetCurrentFontStyle(itsCommandOutput->GetText()->GetDefaultFont().GetStyle());
	itsCommandOutput->Paste(input);

	itsCommandOutput->Paste(JString::newline);
	input.TrimWhitespace();
	if (!input.IsEmpty())
		{
		itsHistoryMenu->AddString(input);
		}
	itsLink->SendRaw(input);

	itsCommandInput->GetText()->SetText(JString::empty);
}

/******************************************************************************
 HandleCompletionRequest (private)

 ******************************************************************************/

void
CMCommandDirector::HandleCompletionRequest()
{
	const JString& input = itsCommandInput->GetText()->GetText();
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
		itsCurrentCommand = itsCommandInput->GetText()->GetText();
		}

	const JIndex menuIndex = itsHistoryMenu->GetItemCount() - itsHistoryIndex + 1;

	const JInteger i = JInteger(menuIndex) - delta;
	if (0 < i && i <= (JInteger) itsHistoryMenu->GetItemCount())
		{
		itsHistoryIndex     += delta;
		const JString& input = itsHistoryMenu->JXTextMenu::GetItemText(i);
		itsCommandInput->GetText()->SetText(input);
		}
	else if (i > (JInteger) itsHistoryMenu->GetItemCount())
		{
		itsHistoryIndex = 0;
		itsCommandInput->GetText()->SetText(itsCurrentCommand);
		}

	itsCommandInput->GoToEndOfLine();
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
	if (JGetChooseSaveFile()->ChooseFiles(JGetString("ChooseSourcePrompt::CMCommandDirector"), JString::empty, &list))
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
	const JString&	fileName,
	const JSize		lineIndex,
	const bool	askDebuggerWhenRelPath
	)
{
	JString fullName;
	if (JIsRelativePath(fileName) && askDebuggerWhenRelPath)
		{
		bool exists;
		JString fullName;
		const bool known = itsLink->FindFile(fileName, &exists, &fullName);
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
	else if (JConvertToAbsolutePath(fileName, JString::empty, &fullName))
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
	const JString& origFn,
	const JString& addr
	)
{
	CMSourceDirector* dir = nullptr;

	JString fn = origFn;
	JStringIterator iter(&fn);
	if (iter.Next("("))
		{
		iter.SkipPrev();
		iter.RemoveAllNext();
		}
	iter.Invalidate();

	const JSize count = itsAsmDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		dir = itsAsmDirs->GetElement(i);
		const JString* f;
		if (dir->GetFunctionName(&f) && fn == *f)
			{
			dir->Activate();
			break;
			}
		dir = nullptr;		// trigger Create()
		}

	if (dir == nullptr)
		{
		dir = CMSourceDirector::Create(this, addr.IsEmpty() ? fn : JString::empty, CMSourceDirector::kAsmType);
		itsAsmDirs->Append(dir);
		dir->Activate();
		}

	if (!addr.IsEmpty())
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
	const JString& fileName
	)
	const
{
	const JUtf8Byte* map[] =
		{
		"name", fileName.GetBytes()
		};
	const JString err = JGetString("CannotOpenFile::CMCommandDirector", map, sizeof(map));
	JGetUserNotification()->ReportError(err);
}

/******************************************************************************
 LoadConfig

 ******************************************************************************/

void
CMCommandDirector::LoadConfig()
{
	JString fullName;
	if (JGetChooseSaveFile()->ChooseFile(JString("ChooseConfigPrompt::CMCommandDirector"), JString::empty, &fullName))
		{
		std::ifstream input(fullName.GetBytes());

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
				auto* dir = jnew CMArray1DDir(input, vers, this);
				assert( dir != nullptr );
				// adds itself to list automatically
				dir->Activate();
				}

			input >> count;

			for (JIndex i=1; i<=count; i++)
				{
				auto* dir = jnew CMArray2DDir(input, vers, this);
				assert( dir != nullptr );
				// adds itself to list automatically
				dir->Activate();
				}

			input >> count;

			for (JIndex i=1; i<=count; i++)
				{
				auto* dir = jnew CMPlot2DDir(input, vers, this);
				assert( dir != nullptr );
				// adds itself to list automatically
				dir->Activate();
				}

			if (vers >= 3)
				{
				input >> count;

				for (JIndex i=1; i<=count; i++)
					{
					auto* dir = jnew CMMemoryDir(input, vers, this);
					assert( dir != nullptr );
					// adds itself to list automatically
					dir->Activate();
					}
				}
			}
		else
			{
			JGetUserNotification()->DisplayMessage(JGetString("ConfigTooNew::CMCommandDirector"));
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

	if (JGetChooseSaveFile()->SaveFile(JGetString("SaveFilePrompt::CMCommandDirector"), JString::empty, origName, &fullName))
		{
		std::ofstream output(fullName.GetBytes());

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
		itsCommandOutput->GetText()->WritePlainText(itsCurrentHistoryFile, JStyledText::kUNIXText);
		}
}

/******************************************************************************
 SaveInNewFile

 ******************************************************************************/

void
CMCommandDirector::SaveInNewFile()
{
	if (JGetChooseSaveFile()->SaveFile(JGetString("SaveFilePrompt::CMCommandDirector"), JString::empty, JString::empty,  &itsCurrentHistoryFile))
		{
		SaveInCurrentFile();
		}
}

/******************************************************************************
 UpdateDebugMenu

	te1 and te2 can both be nullptr.  If not, they are used to look for
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
		const bool canSetProgram = itsLink->GetFeature(CMLink::kSetProgram);
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
				if (((te1 != nullptr && te1->GetSelection(&text)) ||
					 (te2 != nullptr && te2->GetSelection(&text))) &&
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

						const bool bkwd = itsLink->GetFeature(CMLink::kExecuteBackwards);
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

	te1 and te2 can both be nullptr.  If not, they are used to look for
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
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			DisplayExpression(text);
			}
		}
	else if (index == kDisplay1DArrayCmd)
		{
		JString text;
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			Display1DArray(text);
			}
		}
	else if (index == kPlot1DArrayCmd)
		{
		JString text;
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			Plot1DArray(text);
			}
		}
	else if (index == kDisplay2DArrayCmd)
		{
		JString text;
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			Display2DArray(text);
			}
		}

	else if (index == kWatchVarCmd)
		{
		JString text;
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			CMGetLink()->WatchExpression(text);
			}
		}
	else if (index == kWatchLocCmd)
		{
		JString text;
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			CMGetLink()->WatchLocation(text);
			}
		}

	else if (index == kExamineMemCmd)
		{
		JString s1, s2;
		if ((te1 != nullptr && te1->GetSelection(&s1) && !s1.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&s1) && !s1.Contains("\n")))
			{
			s2 = s1;
			}

		auto* dir = jnew CMMemoryDir(this, s2);
		assert(dir != nullptr);
		dir->Activate();
		}
	else if (index == kDisassembleMemCmd)
		{
		JString s1, s2;
		if ((te1 != nullptr && te1->GetSelection(&s1) && !s1.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&s1) && !s1.Contains("\n")))
			{
			s2 = s1;
			}

		auto* dir = jnew CMMemoryDir(this, s2);
		assert(dir != nullptr);
		dir->SetDisplayType(CMMemoryDir::kAsm);
		dir->Activate();
		}
	else if (index == kDisassembleFnCmd)
		{
		JString fn;
		if ((te1 != nullptr && te1->GetSelection(&fn) && !fn.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&fn) && !fn.Contains("\n")))
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
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
			{
			JStringIterator iter(&s);
			while (iter.Next("##"))
				{
				iter.ReplaceLastMatch(text);
				}
			}
		else if (s.Contains("##"))
			{
			JGetUserNotification()->ReportError(JGetString("NoSelection::CMCommandDirector"));
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
			itsWaitingToRunFlag = true;
			}
		}
	else if (CMGetLink()->HasPendingCommands())
		{
		auto* task = jnew CMRunProgramTask();
		assert( task != nullptr );
		task->Start();
		}
	else if (itsLink->OKToDetachOrKill())
		{
		itsLink->RunProgram(itsArgInput->GetText()->GetText());
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
		JGetChooseSaveFile()->ChooseFile(JGetString("ChooseBinaryPrompt::CMCommandDirector"), instr, &fullName))
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
		JGetChooseSaveFile()->ChooseFile(JGetString("ChooseCorePrompt::CMCommandDirector"), JString::empty, origPath, &coreName))
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
		auto* dialog = jnew CMChooseProcessDialog(this);
		assert( dialog != nullptr );
		dialog->BeginDialog();
		}
}

/******************************************************************************
 DisplayExpression

 ******************************************************************************/

void
CMCommandDirector::DisplayExpression
	(
	const JString& expr
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
	const JString& expr
	)
{
	auto* dir = jnew CMArray1DDir(this, expr);
	assert( dir != nullptr );
	dir->Activate();
}

/******************************************************************************
 Display2DArray

 ******************************************************************************/

void
CMCommandDirector::Display2DArray
	(
	const JString& expr
	)
{
	auto* dir = jnew CMArray2DDir(this, expr);
	assert( dir != nullptr );
	dir->Activate();
}

/******************************************************************************
 Plot1DArray

 ******************************************************************************/

void
CMCommandDirector::Plot1DArray
	(
	const JString& expr
	)
{
	auto* dir = jnew CMPlot2DDir(this, expr);
	assert( dir != nullptr );
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
		auto* dlog = jnew CMEditCommandsDialog;
		assert( dlog != nullptr );
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
		JXGetHelpManager()->ShowTOC();
		}
	else if (index == kOverviewCmd)
		{
		JXGetHelpManager()->ShowSection("CMOverviewHelp");
		}
	else if (index == kThisWindowCmd)
		{
		JXGetHelpManager()->ShowSection("CMCommandDirHelp");
		}
	else if (index == kChangesCmd)
		{
		JXGetHelpManager()->ShowChangeLog();
		}
	else if (index == kCreditsCmd)
		{
		JXGetHelpManager()->ShowCredits();
		}
}
