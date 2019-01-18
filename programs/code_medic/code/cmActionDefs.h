/******************************************************************************
 cmActionDefs.h

	Shared actions for use as menu item ID's and in keybinding tables.
	These preprocessor definitions allow them to be included in static
	menu definitions.

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_cmActionDefs
#define _H_cmActionDefs

#include <jXActionDefs.h>	// for convenience

// File menu

#define kCMOpenSourceFileAction  "GDBOpenSourceFile"
#define kCMLoadConfigAction      "GDBLoadConfig"
#define kCMSaveConfigAction      "GDBSaveConfig"
#define kCMEditSourceFileAction  "GDBEditSourceFile"
#define kCMSaveHistoryAction     "GDBSaveHistory"
#define kCMSaveHistoryAsAction   "GDBSaveHistoryAs"

#define kCMRestartDebuggerAction "GDBRestartDebugger"
#define kCMChangeDebuggerAction  "GDBChangeDebugger"

// Debug menu

#define kCMChooseBinaryAction          "GDBChooseBinary"
#define kCMReloadBinaryAction          "GDBReloadBinary"
#define kCMChooseCoreAction            "GDBChooseCore"
#define kCMChooseProcessAction         "GDBChooseProcess"
#define kCMSetArgumentsAction          "GDBSetArguments"
#define kCMEnableAllBreakpointsAction  "GDBEnableAllBreakpoints"
#define kCMDisableAllBreakpointsAction "GDBDisableAllBreakpoints"
#define kCMClearAllBreakpointsAction   "GDBClearAllBreakpoints"
#define kCMDisplayVariableAction       "GDBDisplayVariable"
#define kCMDisplayAsCStringAction      "GDBDisplayAsCStringAction"
#define kCMDisplay1DArrayAction        "GDBDisplay1DArray"
#define kCMPlot1DArrayAction           "GDBPlot1DArray"
#define kCMDisplay2DArrayAction        "GDBDisplay2DArray"
#define kCMExamineMemoryAction         "GDBExamineMemory"
#define kCMDisasmMemoryAction          "GDBDisasmMemory"
#define kCMDisasmFunctionAction        "GDBDisasmFunction"
#define kCMRunProgramAction            "GDBRunProgram"
#define kCMStopProgramAction           "GDBStopProgram"
#define kCMKillProgramAction           "GDBKillProgram"
#define kCMNextInstrAction             "GDBNextInstr"
#define kCMStepIntoAction              "GDBStepInto"
#define kCMFinishSubAction             "GDBFinishSub"
#define kCMContinueRunAction           "GDBContinueRun"
#define kCMNextAsmInstrAction          "GDBNextInstrAsm"
#define kCMStepIntoAsmAction           "GDBStepIntoAsm"
#define kCMPrevInstrAction             "GDBPrevInstr"
#define kCMReverseStepIntoAction       "GDBReverseStepInto"
#define kCMReverseFinishSubAction      "GDBReverseFinishSub"
#define kCMReverseContinueRunAction    "GDBReverseContinueRun"

// Windows menu

#define kCMBringAllFrontAction      "GDBBringAllFront"
#define kCMCloseAllSourceAction     "GDBCloseAllSource"
#define kCMShowCommandLineAction    "GDBShowCommandLine"
#define kCMShowCurrentSourceAction  "GDBShowCurrentSource"
#define kCMShowThreadsAction        "GDBShowThreads"
#define kCMShowStackTraceAction     "GDBShowStackTrace"
#define kCMShowBreakpointsAction    "GDBShowBreakpoints"
#define kCMShowVariablesAction      "GDBShowVariables"
#define kCMShowLocalVariablesAction "GDBShowLocalVariables"
#define kCMShowCurrentAsmAction     "GDBShowCurrentAsm"
#define kCMShowRegistersAction      "GDBShowRegisters"
#define kCMShowFileListAction       "GDBShowFileList"
#define kCMShowDebugInfoAction      "GDBShowDebugInfo"

// Search menu

#define kCMBalanceGroupingSymbolAction "GDBBalanceGroupingSymbol"
#define kCMGoToLineAction              "GDBGoToLine"
#define kCMGoToCurrentLineAction       "GDBGoToCurrentLine"

// Variables

#define kCMNewExpressionAction    "GDBNewExpression"
#define kCMRemoveExpressionAction "GDBRemoveExpression"
#define kCMWatchVarValueAction    "GDBWatchVarValue"
#define kCMWatchVarLocationAction "GDBWatchVarLocation"
#define kCMCopyFullVarNameAction  "GDBCopyFullVarName"
#define kCMCopyVarValueAction     "GDBCopyVarValue"

#endif
