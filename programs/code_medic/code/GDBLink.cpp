/******************************************************************************
 GDBLink.cpp

	Interface to gdb.

	http://ftp.gnu.org/pub/old-gnu/Manuals/gdb-5.1.1/html_node/gdb_211.html
	http://www.devworld.apple.com/documentation/DeveloperTools/gdb/gdb/gdb_25.html

	BASE CLASS = CMLink

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "GDBLink.h"
#include "GDBOutputScanner.h"
#include "GDBPingTask.h"
#include "GDBBreakpointManager.h"
#include "CMChooseProcessDialog.h"

#include "GDBAnalyzeCore.h"
#include "GDBArray2DCommand.h"
#include "GDBPlot2DCommand.h"
#include "GDBCheckCoreStatus.h"
#include "GDBDisplaySourceForMain.h"
#include "GDBGetStopLocationForLink.h"
#include "GDBGetStopLocationForAsm.h"
#include "GDBGetCompletions.h"
#include "GDBGetFrame.h"
#include "GDBGetStack.h"
#include "GDBGetThread.h"
#include "GDBGetThreads.h"
#include "GDBGetMemory.h"
#include "GDBGetAssembly.h"
#include "GDBGetRegisters.h"
#include "GDBGetFullPath.h"
#include "GDBGetInitArgs.h"
#include "GDBGetLocalVars.h"
#include "GDBGetProgramName.h"
#include "GDBGetSourceFileList.h"
#include "GDBSimpleCommand.h"
#include "GDBVarCommand.h"
#include "GDBVarNode.h"

#include "cmGlobals.h"

#include <JXAssert.h>
#include <JProcess.h>
#include <JRegex.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jSysUtil.h>
#include <jAssert.h>

static const bool kFeatures[]=
{
	true,		// kSetProgram
	true,		// kSetArgs
	true,		// kSetCore
	true,		// kSetProcess
	true,		// kRunProgram
	true,		// kStopProgram
	true,		// kSetExecutionPoint
	true,		// kExecuteBackwards
	true,		// kShowBreakpointInfo
	true,		// kSetBreakpointCondition
	true,		// kSetBreakpointIgnoreCount
	true,		// kWatchExpression
	true,		// kWatchLocation
	true,		// kExamineMemory
	true,		// kDisassembleMemory
};

/******************************************************************************
 Constructor

 *****************************************************************************/

GDBLink::GDBLink()
	:
	CMLink(kFeatures),
	itsDebuggerProcess(nullptr),
	itsChildProcess(nullptr),
	itsOutputLink(nullptr),
	itsInputLink(nullptr)
{
	InitFlags();

	itsScanner = jnew GDB::Output::Scanner;
	assert( itsScanner != nullptr );

	itsBPMgr = jnew GDBBreakpointManager(this);
	assert( itsBPMgr != nullptr );

	itsGetStopLocation = jnew GDBGetStopLocationForLink();
	assert( itsGetStopLocation != nullptr );

	itsGetStopLocation2 = jnew GDBGetStopLocationForAsm();
	assert( itsGetStopLocation2 != nullptr );

	itsPingTask = jnew GDBPingTask();
	assert( itsPingTask != nullptr );

	StartDebugger();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GDBLink::~GDBLink()
{
	StopDebugger();

	jdelete itsScanner;
	jdelete itsBPMgr;
	jdelete itsGetStopLocation;
	jdelete itsGetStopLocation2;
	jdelete itsPingTask;

	DeleteOneShotCommands();
}

/******************************************************************************
 InitFlags (private)

 *****************************************************************************/

void
GDBLink::InitFlags()
{
	itsHasStartedFlag           = false;
	itsInitFinishedFlag         = false;
	itsSymbolsLoadedFlag        = false;
	itsDebuggerBusyFlag         = true;
	itsIsDebuggingFlag          = false;
	itsIsAttachedFlag           = false;
	itsProgramIsStoppedFlag     = true;
	itsFirstBreakFlag           = false;
	itsPrintingOutputFlag       = true;	// print welcome message
	itsDefiningScriptFlag       = false;
	itsWaitingToQuitFlag        = false;
	itsContinueCount            = 0;
	itsPingID                   = 0;
}

/******************************************************************************
 GetPrompt

 ******************************************************************************/

const JString&
GDBLink::GetPrompt()
	const
{
	return JGetString("Prompt::GDBLink");
}

/******************************************************************************
 GetScriptPrompt

 ******************************************************************************/

const JString&
GDBLink::GetScriptPrompt()
	const
{
	return JGetString("ScriptPrompt::GDBLink");
}

/******************************************************************************
 DebuggerHasStarted

 ******************************************************************************/

bool
GDBLink::DebuggerHasStarted()
	const
{
	return itsHasStartedFlag;
}

/******************************************************************************
 GetChooseProgramInstructions

 ******************************************************************************/

JString
GDBLink::GetChooseProgramInstructions()
	const
{
	return JGetString("ChooseProgramInstr::GDBLink");
}

/******************************************************************************
 HasProgram

 ******************************************************************************/

bool
GDBLink::HasProgram()
	const
{
	return !itsProgramName.IsEmpty();
}

/******************************************************************************
 GetProgram

 ******************************************************************************/

bool
GDBLink::GetProgram
	(
	JString* fullName
	)
	const
{
	*fullName = itsProgramName;
	return !itsProgramName.IsEmpty();
}

/******************************************************************************
 HasCore

 ******************************************************************************/

bool
GDBLink::HasCore()
	const
{
	return !itsCoreName.IsEmpty();
}

/******************************************************************************
 GetCore

 ******************************************************************************/

bool
GDBLink::GetCore
	(
	JString* fullName
	)
	const
{
	*fullName = itsCoreName;
	return !itsCoreName.IsEmpty();
}

/******************************************************************************
 HasLoadedSymbols

 ******************************************************************************/

bool
GDBLink::HasLoadedSymbols()
	const
{
	return itsSymbolsLoadedFlag;
}

/******************************************************************************
 IsDebugging

 *****************************************************************************/

bool
GDBLink::IsDebugging()
	const
{
	return itsIsDebuggingFlag;
}

/******************************************************************************
 ProgramIsRunning

 *****************************************************************************/

bool
GDBLink::ProgramIsRunning()
	const
{
	return itsIsDebuggingFlag && !itsProgramIsStoppedFlag;
}

/******************************************************************************
 ProgramIsStopped

 *****************************************************************************/

bool
GDBLink::ProgramIsStopped()
	const
{
	return itsIsDebuggingFlag && itsProgramIsStoppedFlag;
}

/******************************************************************************
 OKToSendMultipleCommands

 *****************************************************************************/

bool
GDBLink::OKToSendMultipleCommands()
	const
{
	return CMLink::OKToSendMultipleCommands();
}

/******************************************************************************
 OKToSendCommands

 *****************************************************************************/

bool
GDBLink::OKToSendCommands
	(
	const bool background
	)
	const
{
	return itsContinueCount == 0;
}

/******************************************************************************
 IsDefiningScript

 *****************************************************************************/

bool
GDBLink::IsDefiningScript()
	const
{
	return itsDefiningScriptFlag;
}

/******************************************************************************
 Receive (virtual protected)

 *****************************************************************************/

void
GDBLink::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsInputLink && message.Is(CMPipeT::kReadReady))
		{
		ReadFromDebugger();
		}
	else if (sender == itsDebuggerProcess && message.Is(JProcess::kFinished))
		{
		if (!itsWaitingToQuitFlag &&
			JGetUserNotification()->AskUserYes(JGetString("DebuggerCrashed::GDBLink")))
			{
			RestartDebugger();
			}
		}
	else if (sender == itsChildProcess && message.Is(JProcess::kFinished))
		{
		ProgramFinished1();
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 ReadFromDebugger (private)

 *****************************************************************************/

void
GDBLink::ReadFromDebugger()
{
	JString data;
	itsInputLink->Read(&data);
	Broadcast(DebugOutput(data, kOutputType));

	itsScanner->AppendInput(data);
	while (true)
		{
		const GDB::Output::Scanner::Token token = itsScanner->NextToken();
		if (token.type == GDB::Output::Scanner::kEOF)
			{
			break;
			}

		// can't send init cmds until gdb has started

		if (!itsHasStartedFlag)
			{
			InitDebugger();
			}

		// check whether or not debugger is busy
		// (We set itsProgramIsStoppedFlag here so *all* commands know the
		//  correct status.)

		const bool wasStopped = itsProgramIsStoppedFlag;
		if (token.type == GDB::Output::Scanner::kReadyForInput)
			{
			itsPingTask->Stop();

			if (0 < token.data.number && token.data.number < itsPingID)
				{
				continue;
				}

			itsProgramIsStoppedFlag = true;
//			itsPrintingOutputFlag   = true;
			itsDefiningScriptFlag   = false;

			itsDebuggerBusyFlag = false;
			Broadcast(DebuggerReadyForInput());
			RunNextCommand();

			// If we stopped the program in order to send commands to the
			// debugger, we need to continue once everything settles down.
			// Unless it is invoked by a CMCommand, itsContinueCount has to
			// start at 2 because we have to ignore the prompt received
			// immediately after sending SIGINT.

			if (itsContinueCount > 0 && !HasForegroundCommands())
				{
				itsContinueCount--;

				if (itsContinueCount == 0)
					{
					Send(JString("continue", JString::kNoCopy));
					}
				}

			// if no source file for current location, won't get
			// ProgramStoppedAtLocation message

			else if (itsIsDebuggingFlag && !wasStopped)
				{
				CMLocation location;
				SendProgramStopped(location);
				}

			continue;
			}
		else if (!itsDebuggerBusyFlag)
			{
			itsDebuggerBusyFlag = true;
			Broadcast(DebuggerBusy());
			}

		// process token

		if (token.type == GDB::Output::Scanner::kProgramOutput)
			{
			if (itsPrintingOutputFlag)
				{
				// We cannot tell the difference between gdb and program output
				Broadcast(UserOutput(*(token.data.pString), false, false));
				}
			}
		else if (token.type == GDB::Output::Scanner::kErrorOutput)
			{
			if (itsPrintingOutputFlag)
				{
				Broadcast(UserOutput(*(token.data.pString), true));
				}
			}

		else if (token.type == GDB::Output::Scanner::kBeginMedicCmd ||
				 token.type == GDB::Output::Scanner::kBeginMedicIgnoreCmd)
			{
			HandleCommandRunning(token.data.number);
			itsPrintingOutputFlag = false;
			}
		else if (token.type == GDB::Output::Scanner::kCommandOutput)
			{
			CMCommand* cmd;
			if (GetRunningCommand(&cmd))
				{
				cmd->Accumulate(*(token.data.pString));
				}
			}
		else if (token.type == GDB::Output::Scanner::kCommandResult)
			{
			CMCommand* cmd;
			if (GetRunningCommand(&cmd))
				{
				cmd->SaveResult(*(token.data.pString));
				}
			}
		else if (token.type == GDB::Output::Scanner::kEndMedicCmd ||
				 token.type == GDB::Output::Scanner::kEndMedicIgnoreCmd)
			{
			CMCommand* cmd;
			if (GetRunningCommand(&cmd))
				{
				cmd->Finished(true);	// may delete object
				SetRunningCommand(nullptr);

				if (!HasForegroundCommands())
					{
					RunNextCommand();
					}
				}

			itsPrintingOutputFlag = true;
			}

		else if (token.type == GDB::Output::Scanner::kBreakpointsChanged)
			{
			if (!itsFirstBreakFlag)		// ignore tbreak in hook-run
				{
				Broadcast(BreakpointsChanged());
				}
			}
		else if (token.type == GDB::Output::Scanner::kFrameChanged)
			{
			Broadcast(FrameChanged());	// sync with kFrameChangedAndProgramStoppedAtLocation
			}
		else if (token.type == GDB::Output::Scanner::kThreadChanged)
			{
			Broadcast(ThreadChanged());
			}
		else if (token.type == GDB::Output::Scanner::kValueChanged)
			{
			Broadcast(ValueChanged());
			}

		else if (token.type == GDB::Output::Scanner::kPrepareToLoadSymbols)
			{
			itsIsDebuggingFlag   = false;
			itsSymbolsLoadedFlag = false;
			itsProgramName.Clear();
			ClearFileNameMap();
			Broadcast(PrepareToLoadSymbols());
			}
		else if (token.type == GDB::Output::Scanner::kFinishedLoadingSymbolsFromProgram)
			{
			itsSymbolsLoadedFlag = true;

			// We can't use a lexer to extract the file name from gdb's
			// output because the pattern "Reading symbols from [^\n]+..."
			// will slurp up "(no debugging symbols found)..." as well.

			auto* cmd = jnew GDBGetProgramName;
			assert( cmd != nullptr );

			if (token.data.pString != nullptr)
				{
				Broadcast(UserOutput(*(token.data.pString), false));
				}
			}
		else if (token.type == GDB::Output::Scanner::kNoSymbolsInProgram)
			{
			if (!itsIsAttachedFlag)
				{
				if (token.data.pString != nullptr)
					{
					Broadcast(UserOutput(*(token.data.pString), true));
					}

				JString name;
				Broadcast(SymbolsLoaded(false, name));
				}
			}
		else if (token.type == GDB::Output::Scanner::kSymbolsReloaded)
			{
			Broadcast(PrepareToLoadSymbols());
			Broadcast(SymbolsReloaded());
			}

		else if (token.type == GDB::Output::Scanner::kCoreChanged)
			{
			// We have to check whether a core was loaded or cleared.

			auto* cmd = jnew GDBCheckCoreStatus;
			assert( cmd != nullptr );
			}

		else if (token.type == GDB::Output::Scanner::kAttachedToProcess)
			{
			itsIsAttachedFlag  = true;
			itsIsDebuggingFlag = true;
			ProgramStarted(token.data.number);
			Broadcast(AttachedToProcess());
			}
		else if (token.type == GDB::Output::Scanner::kDetachingFromProcess)
			{
			ProgramFinished1();
			}

		else if (token.type == GDB::Output::Scanner::kProgramStarting)
			{
			itsIsDebuggingFlag      = true;
			itsProgramIsStoppedFlag = false;
			itsFirstBreakFlag       = true;
			itsPrintingOutputFlag   = false;	// ignore tbreak output
			}
		else if (token.type == GDB::Output::Scanner::kBeginGetPID)
			{
			itsPrintingOutputFlag = false;	// ignore "info prog"
			}
		else if (token.type == GDB::Output::Scanner::kProgramPID)
			{
			ProgramStarted(token.data.number);
			}
		else if (token.type == GDB::Output::Scanner::kEndGetPID)
			{
			if (itsChildProcess == nullptr)	// ask user for PID
				{
				auto* dialog =
					jnew CMChooseProcessDialog(JXGetApplication(), false);
				assert( dialog != nullptr );
				dialog->BeginDialog();
				}
			}

		else if (token.type == GDB::Output::Scanner::kProgramStoppedAtLocation)
			{
			itsProgramIsStoppedFlag = true;
			if (token.data.pLocation != nullptr)
				{
				SendProgramStopped(*(token.data.pLocation));
//				Send("call (JXGetAssertHandler())->UnlockDisplays()");
				}
			else
				{
				itsGetStopLocation->Send();
				}

			if (token.data.pString != nullptr)
				{
				Broadcast(UserOutput(*(token.data.pString), false));
				}
			}
		else if (token.type == GDB::Output::Scanner::kFrameChangedAndProgramStoppedAtLocation)
			{
			Broadcast(FrameChanged());	// sync with kFrameChanged

			itsProgramIsStoppedFlag = true;
			itsGetStopLocation->Send();
			}

		else if (token.type == GDB::Output::Scanner::kBeginScriptDefinition)
			{
			itsDefiningScriptFlag = true;
			Broadcast(DebuggerDefiningScript());
			}

		else if (token.type == GDB::Output::Scanner::kPlugInMessage)
			{
			Broadcast(PlugInMessage(*(token.data.pString)));
			}

		else if (token.type == GDB::Output::Scanner::kProgramRunning)
			{
			itsProgramIsStoppedFlag = false;
			CancelBackgroundCommands();
			Broadcast(ProgramRunning());
			}
		else if (token.type == GDB::Output::Scanner::kProgramFinished)
			{
			ProgramFinished1();

			if (token.data.pString != nullptr)
				{
				Broadcast(UserOutput(*(token.data.pString), false));
				}
			}
		else if (token.type == GDB::Output::Scanner::kProgramKilled)
			{
			ProgramFinished1();
			}

		else if (token.type == GDB::Output::Scanner::kDebuggerFinished)
			{
			JXGetApplication()->Quit();
			itsWaitingToQuitFlag = true;
			}
		}
}

/******************************************************************************
 SaveProgramName

	Callback for GDBGetProgramName.

 *****************************************************************************/

void
GDBLink::SaveProgramName
	(
	const JString& fileName
	)
{
	itsProgramName = fileName;

	JString path, name;
	if (!itsProgramName.IsEmpty())
		{
		JSplitPathAndName(itsProgramName, &path, &name);
		}

	Broadcast(SymbolsLoaded(true, name));
}

/******************************************************************************
 SaveCoreName

	Callback for GDBCheckCoreStatus.

 *****************************************************************************/

void
GDBLink::SaveCoreName
	(
	const JString& fileName
	)
{
	itsCoreName = fileName;
	if (!itsCoreName.IsEmpty())
		{
		itsIsDebuggingFlag = false;
		Broadcast(CoreLoaded());
		}
	else
		{
		// debugging status set elsewhere
		Broadcast(CoreCleared());
		}
}

/******************************************************************************
 FirstBreakImpossible

 *****************************************************************************/

void
GDBLink::FirstBreakImpossible()
{
	if (itsFirstBreakFlag)
		{
		// parallel to SendProgramStopped()

		itsFirstBreakFlag     = false;
		itsPrintingOutputFlag = true;
		}
}

/******************************************************************************
 SendProgramStopped

 *****************************************************************************/

void
GDBLink::SendProgramStopped
	(
	const CMLocation& location
	)
{
	if (itsFirstBreakFlag)					// tbreak to get pid
		{
		// parallel to FirstBreakImpossible()

		itsFirstBreakFlag     = false;
		itsPrintingOutputFlag = true;		// "info prog" is finished

		Broadcast(ProgramFirstStop());

		if (itsBPMgr->HasBreakpointAt(location))
			{
			if (itsContinueCount == 0)
				{
				PrivateSendProgramStopped(location);
				}
			}
		else
			{
			Send(JString("continue", JString::kNoCopy));
			}
		}
	else if (itsContinueCount == 0)
		{
		PrivateSendProgramStopped(location);
		}
}

/******************************************************************************
 PrivateSendProgramStopped (private)

 *****************************************************************************/

void
GDBLink::PrivateSendProgramStopped
	(
	const CMLocation& location
	)
{
	Broadcast(ProgramStopped(location));

	if (location.GetFunctionName().IsEmpty() ||
		location.GetMemoryAddress().IsEmpty())
	{
		itsGetStopLocation2->Send();
	}

	RunNextCommand();
}

/******************************************************************************
 SendProgramStopped2

 *****************************************************************************/

void
GDBLink::SendProgramStopped2
	(
	const CMLocation& location
	)
{
	Broadcast(ProgramStopped2(location));
}

/******************************************************************************
 SetProgram

 *****************************************************************************/

void
GDBLink::SetProgram
	(
	const JString& fullName
	)
{
	if (itsHasStartedFlag)
		{
		if (itsInitFinishedFlag && !JSameDirEntry(fullName, itsProgramName))
			{
			Send(JString("delete", JString::kNoCopy));
			}
		DetachOrKill();
		Send(JString("core-file", JString::kNoCopy));

		Send("file " + JPrepArgForExec(fullName));
		}

	itsProgramName = fullName;
}

/******************************************************************************
 ReloadProgram

 *****************************************************************************/

void
GDBLink::ReloadProgram()
{
	if (HasProgram())
		{
		SetProgram(itsProgramName);
		}
}

/******************************************************************************
 SetCore

 *****************************************************************************/

void
GDBLink::SetCore
	(
	const JString& fullName
	)
{
	if (itsHasStartedFlag)
		{
		DetachOrKill();

		// can't use JPrepArgForExec() because gdb doesn't like the quotes

		const JString cmdStr = "core-file " + fullName;
		if (itsProgramName.IsEmpty())
			{
			auto* cmd = jnew GDBAnalyzeCore(cmdStr);
			assert( cmd != nullptr );
			cmd->Send();
			}
		else
			{
			Send(cmdStr);
			}
		}

	itsCoreName = fullName;
}

/******************************************************************************
 AttachToProcess

 *****************************************************************************/

void
GDBLink::AttachToProcess
	(
	const pid_t pid
	)
{
	Send(JString("core-file", JString::kNoCopy));
	DetachOrKill();

	Send("attach " + JString((JUInt64) pid));
}

/******************************************************************************
 RunProgram

 *****************************************************************************/

void
GDBLink::RunProgram
	(
	const JString& args
	)
{
	Send(JString("core-file", JString::kNoCopy));
	DetachOrKill();

	Send("set args " + args);

	Send(JString("run", JString::kNoCopy));
}

/******************************************************************************
 GetBreakpointManager

 *****************************************************************************/

CMBreakpointManager*
GDBLink::GetBreakpointManager()
{
	return itsBPMgr;
}

/******************************************************************************
 ShowBreakpointInfo

 *****************************************************************************/

void
GDBLink::ShowBreakpointInfo
	(
	const JIndex debuggerIndex
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	JString cmd("echo \\n\ninfo breakpoint ");
	cmd += JString((JUInt64) debuggerIndex);
	cmd += "\necho \\n";
	SendWhenStopped(cmd);
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
GDBLink::SetBreakpoint
	(
	const JString&	fileName,
	const JIndex	lineIndex,
	const bool	temporary
	)
{
	JString path, name;
	JSplitPathAndName(fileName, &path, &name);

	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	JString cmd("echo \\032\\032:Medic breakpoints changed:\n");
	cmd += (temporary ? "-break-insert -t " : "-break-insert ");
	cmd += name + ":" + JString((JUInt64) lineIndex);
	SendWhenStopped(cmd);
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
GDBLink::SetBreakpoint
	(
	const JString&	address,
	const bool	temporary
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	JString cmd("echo \\032\\032:Medic breakpoints changed:\n");
	cmd += (temporary ? "-break-insert -t " : "-break-insert ");
	cmd += "*";
	cmd += address;
	SendWhenStopped(cmd);
}

/******************************************************************************
 RemoveBreakpoint

 *****************************************************************************/

void
GDBLink::RemoveBreakpoint
	(
	const JIndex debuggerIndex
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	const JString cmd = "delete " + JString((JUInt64) debuggerIndex);
	SendWhenStopped(cmd);
}

/******************************************************************************
 RemoveAllBreakpointsOnLine

 *****************************************************************************/

void
GDBLink::RemoveAllBreakpointsOnLine
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	JString path, name;
	JSplitPathAndName(fileName, &path, &name);

	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	const JString cmd = "clear " + name + ":" + JString((JUInt64) lineIndex);
	SendWhenStopped(cmd);
}

/******************************************************************************
 RemoveAllBreakpointsAtAddress

 *****************************************************************************/

void
GDBLink::RemoveAllBreakpointsAtAddress
	(
	const JString& addr
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	const JString cmd = "clear *" + JString(addr);
	SendWhenStopped(cmd);
}

/******************************************************************************
 RemoveAllBreakpoints

 *****************************************************************************/

void
GDBLink::RemoveAllBreakpoints()
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	SendWhenStopped(JString("delete", JString::kNoCopy));
}

/******************************************************************************
 SetBreakpointEnabled

 *****************************************************************************/

void
GDBLink::SetBreakpointEnabled
	(
	const JIndex	debuggerIndex,
	const bool	enabled,
	const bool	once
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	const JString cmd =
		(enabled ? "enable " : "disable ") + 
		((once ? "once " : "" ) + JString((JUInt64) debuggerIndex));
	SendWhenStopped(cmd);
}

/******************************************************************************
 SetBreakpointCondition

 *****************************************************************************/

void
GDBLink::SetBreakpointCondition
	(
	const JIndex	debuggerIndex,
	const JString&	condition
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	const JString cmd = "condition " + JString((JUInt64) debuggerIndex) + " " + condition;
	SendWhenStopped(cmd);
}

/******************************************************************************
 RemoveBreakpointCondition

 *****************************************************************************/

void
GDBLink::RemoveBreakpointCondition
	(
	const JIndex debuggerIndex
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	const JString cmd = "condition " + JString((JUInt64) debuggerIndex);
	SendWhenStopped(cmd);
}

/******************************************************************************
 SetBreakpointIgnoreCount

 *****************************************************************************/

void
GDBLink::SetBreakpointIgnoreCount
	(
	const JIndex	debuggerIndex,
	const JSize		count
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	const JString cmd = "ignore " + JString((JUInt64) debuggerIndex) + " " + JString((JUInt64) count);
	SendWhenStopped(cmd);
}

/******************************************************************************
 WatchExpression

 *****************************************************************************/

void
GDBLink::WatchExpression
	(
	const JString& expr
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	JString cmd("watch ");
	cmd += expr;
	SendWhenStopped(cmd);
}

/******************************************************************************
 WatchLocation

 *****************************************************************************/

void
GDBLink::WatchLocation
	(
	const JString& expr
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	JString cmd("watch -location ");
	cmd += expr;
	SendWhenStopped(cmd);
}

/******************************************************************************
 SwitchToThread

 *****************************************************************************/

void
GDBLink::SwitchToThread
	(
	const JUInt64 id
	)
{
	if (ProgramIsStopped() || HasCore())
		{
		itsPrintingOutputFlag = false;
		Send("thread " + JString((JUInt64) id));
		}
}

/******************************************************************************
 SwitchToFrame

 *****************************************************************************/

void
GDBLink::SwitchToFrame
	(
	const JUInt64 id
	)
{
	if (ProgramIsStopped() || HasCore())
		{
		itsPrintingOutputFlag = false;
		Send("frame " + JString((JUInt64) id));
		}
}

/******************************************************************************
 StepOver

 *****************************************************************************/

void
GDBLink::StepOver()
{
	Send(JString("next", JString::kNoCopy));
}

/******************************************************************************
 StepInto

 *****************************************************************************/

void
GDBLink::StepInto()
{
	Send(JString("step", JString::kNoCopy));
}

/******************************************************************************
 StepOut

 *****************************************************************************/

void
GDBLink::StepOut()
{
	Send(JString("finish", JString::kNoCopy));
}

/******************************************************************************
 Continue

 *****************************************************************************/

void
GDBLink::Continue()
{
	Send(JString("continue", JString::kNoCopy));
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
GDBLink::RunUntil
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	if (ProgramIsStopped())
		{
		JString path, name;
		JSplitPathAndName(fileName, &path, &name);

		Send("until " + name + ":" + JString((JUInt64) lineIndex));
		}
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
GDBLink::SetExecutionPoint
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	if (ProgramIsStopped())
		{
		JString path, name;
		JSplitPathAndName(fileName, &path, &name);

		const JString loc = name + ":" + JString((JUInt64) lineIndex);

		SendRaw("tbreak " + loc);
		SendRaw("jump " + loc);
		}
}

/******************************************************************************
 StepOverAssembly

 *****************************************************************************/

void
GDBLink::StepOverAssembly()
{
	Send(JString("nexti", JString::kNoCopy));
}

/******************************************************************************
 StepIntoAssembly

 *****************************************************************************/

void
GDBLink::StepIntoAssembly()
{
	Send(JString("stepi", JString::kNoCopy));
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
GDBLink::RunUntil
	(
	const JString& addr
	)
{
	if (ProgramIsStopped())
		{
		Send("until *" + addr);
		}
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
GDBLink::SetExecutionPoint
	(
	const JString& addr
	)
{
	if (ProgramIsStopped())
		{
		JString loc = "*" + addr;

		SendRaw("tbreak " + loc);
		SendRaw("jump " + loc);
		}
}

/******************************************************************************
 BackupOver

 *****************************************************************************/

void
GDBLink::BackupOver()
{
	Send(JString("reverse-next", JString::kNoCopy));
}

/******************************************************************************
 BackupInto

 *****************************************************************************/

void
GDBLink::BackupInto()
{
	Send(JString("reverse-step", JString::kNoCopy));
}

/******************************************************************************
 BackupOut

 *****************************************************************************/

void
GDBLink::BackupOut()
{
	Send(JString("reverse-finish", JString::kNoCopy));
}

/******************************************************************************
 BackupContinue

 *****************************************************************************/

void
GDBLink::BackupContinue()
{
	Send(JString("reverse-continue", JString::kNoCopy));
}

/******************************************************************************
 SetValue

 *****************************************************************************/

void
GDBLink::SetValue
	(
	const JString& name,
	const JString& value
	)
{
	if (ProgramIsStopped())
		{
		Send("set variable " + name + " = " + value);

		// We have to broadcast manually because only "set x=y" triggers
		// our hook, not "set variable x=y".  This is actually a good thing
		// because it means we won't get stuck in an infinite loop when we
		// use "set" to change the display options before requesting data.
		// (But it's silly that "set variable" doesn't trigger it!)

		Broadcast(ValueChanged());
		}
}

/******************************************************************************
 CreateArray2DCommand

 *****************************************************************************/

CMArray2DCommand*
GDBLink::CreateArray2DCommand
	(
	CMArray2DDir*		dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
{
	CMArray2DCommand* cmd = jnew GDBArray2DCommand(dir, table, data);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreatePlot2DCommand

 *****************************************************************************/

CMPlot2DCommand*
GDBLink::CreatePlot2DCommand
	(
	CMPlot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
{
	CMPlot2DCommand* cmd = jnew GDBPlot2DCommand(dir, x, y);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateDisplaySourceForMain

 *****************************************************************************/

CMDisplaySourceForMain*
GDBLink::CreateDisplaySourceForMain
	(
	CMSourceDirector* sourceDir
	)
{
	CMDisplaySourceForMain* cmd = jnew GDBDisplaySourceForMain(sourceDir);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetCompletions

 *****************************************************************************/

CMGetCompletions*
GDBLink::CreateGetCompletions
	(
	CMCommandInput*	input,
	CMCommandOutputDisplay*	history
	)
{
	CMGetCompletions* cmd = jnew GDBGetCompletions(input, history);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetFrame

 *****************************************************************************/

CMGetFrame*
GDBLink::CreateGetFrame
	(
	CMStackWidget* widget
	)
{
	CMGetFrame* cmd = jnew GDBGetFrame(widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetStack

 *****************************************************************************/

CMGetStack*
GDBLink::CreateGetStack
	(
	JTree*			tree,
	CMStackWidget*	widget
	)
{
	CMGetStack* cmd = jnew GDBGetStack(tree, widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetThread

 *****************************************************************************/

CMGetThread*
GDBLink::CreateGetThread
	(
	CMThreadsWidget* widget
	)
{
	CMGetThread* cmd = jnew GDBGetThread(widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetThreads

 *****************************************************************************/

CMGetThreads*
GDBLink::CreateGetThreads
	(
	JTree*				tree,
	CMThreadsWidget*	widget
	)
{
	CMGetThreads* cmd = jnew GDBGetThreads(tree, widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetFullPath

 *****************************************************************************/

CMGetFullPath*
GDBLink::CreateGetFullPath
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	CMGetFullPath* cmd = jnew GDBGetFullPath(fileName, lineIndex);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetInitArgs

 *****************************************************************************/

CMGetInitArgs*
GDBLink::CreateGetInitArgs
	(
	JXInputField* argInput
	)
{
	CMGetInitArgs* cmd = jnew GDBGetInitArgs(argInput);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetLocalVars

 *****************************************************************************/

CMGetLocalVars*
GDBLink::CreateGetLocalVars
	(
	CMVarNode* rootNode
	)
{
	CMGetLocalVars* cmd = jnew GDBGetLocalVars(rootNode);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetSourceFileList

 *****************************************************************************/

CMGetSourceFileList*
GDBLink::CreateGetSourceFileList
	(
	CMFileListDir* fileList
	)
{
	CMGetSourceFileList* cmd = jnew GDBGetSourceFileList(fileList);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarValueCommand

 *****************************************************************************/

CMVarCommand*
GDBLink::CreateVarValueCommand
	(
	const JString& expr
	)
{
	CMVarCommand* cmd = jnew GDBVarCommand("print " + expr);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarContentCommand

 *****************************************************************************/

CMVarCommand*
GDBLink::CreateVarContentCommand
	(
	const JString& expr
	)
{
	CMVarCommand* cmd = jnew GDBVarCommand("print *(" + expr + ")");
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarNode

 *****************************************************************************/

CMVarNode*
GDBLink::CreateVarNode
	(
	const bool shouldUpdate		// false for Local Variables
	)
{
	CMVarNode* node = jnew GDBVarNode(shouldUpdate);
	assert( node != nullptr );
	return node;
}

CMVarNode*
GDBLink::CreateVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	fullName,
	const JString&	value
	)
{
	CMVarNode* node = jnew GDBVarNode(parent, name, value);
	assert( node != nullptr );
	return node;
}

/******************************************************************************
 Build1DArrayExpression

 *****************************************************************************/

JString
GDBLink::Build1DArrayExpression
	(
	const JString&	expr,
	const JInteger	index
	)
{
	return Build1DArrayExpressionForCFamilyLanguage(expr, index);
}

/******************************************************************************
 Build2DArrayExpression

 *****************************************************************************/

JString
GDBLink::Build2DArrayExpression
	(
	const JString&	expr,
	const JInteger	rowIndex,
	const JInteger	colIndex
	)
{
	return Build2DArrayExpressionForCFamilyLanguage(expr, rowIndex, colIndex);
}

/******************************************************************************
 CreateGetMemory

 *****************************************************************************/

CMGetMemory*
GDBLink::CreateGetMemory
	(
	CMMemoryDir* dir
	)
{
	CMGetMemory* cmd = jnew GDBGetMemory(dir);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetAssembly

 *****************************************************************************/

CMGetAssembly*
GDBLink::CreateGetAssembly
	(
	CMSourceDirector* dir
	)
{
	CMGetAssembly* cmd = jnew GDBGetAssembly(dir);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetRegisters

 *****************************************************************************/

CMGetRegisters*
GDBLink::CreateGetRegisters
	(
	CMRegistersDir* dir
	)
{
	CMGetRegisters* cmd = jnew GDBGetRegisters(dir);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 ParseList (static)

	Reads data of the format "...","...","...".  It terminates when it
	finds a closing brace.

 *****************************************************************************/

bool
GDBLink::ParseList
	(
	std::istringstream&	stream,
	JPtrArray<JString>*	list,
	const JUtf8Byte		terminator
	)
{
	list->CleanOut();

	JString value;
	while (true)
		{
		stream >> value;
		if (!stream.good())
			{
			return false;
			}

		list->Append(value);

		int c = stream.peek();
		if (c == terminator)
			{
			return true;
			}
		if (c == ',')
			{
			stream.ignore();
			}
		else
			{
			return false;
			}
		}
}

/******************************************************************************
 ParseMap (static)

	Reads data of the format x="...",y="...",z="...".  It terminates when
	it finds a closing brace.

 *****************************************************************************/

bool
GDBLink::ParseMap
	(
	std::istringstream&		stream,
	JStringPtrMap<JString>*	map
	)
{
	map->CleanOut();

	JString key, value;
	bool found;
	while (true)
		{
		key = JReadUntil(stream, '=', &found);
		if (!found)
			{
			return false;
			}

		const JUtf8Byte next = stream.peek();
		if (next == '{' || next == '[')
			{
			stream.ignore();	// skip {
			JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
			if (!ParseList(stream, &list, next == '[' ? ']' : '}'))
				{
				return false;
				}
			stream.ignore();	// skip }

			value.Clear();
			const JSize count = list.GetElementCount();
			for (JIndex i=1; i<=count; i++)
				{
				if (i > 1)
					{
					value.Append("\1");
					}
				value.Append(*(list.GetElement(i)));
				}
			}
		else
			{
			stream >> value;
			if (!stream.good())
				{
				return false;
				}
			}

		map->SetNewElement(key, value);

		int c = stream.peek();
		if (c == '}')
			{
			return true;
			}
		if (c == ',')
			{
			stream.ignore();
			}
		else
			{
			return false;
			}
		}
}

/******************************************************************************
 ParseMapArray (static)

	Reads data of the format {...},{...},{...}.  It terminates when it
	finds a closing square bracket.

 *****************************************************************************/

bool
GDBLink::ParseMapArray
	(
	std::istringstream&						stream,
	JPtrArray< JStringPtrMap<JString> >*	list
	)
{
	list->CleanOut();

	bool found;
	while (true)
		{
		int c = stream.peek();
		if (c == ']')
			{
			return true;
			}
		else if (c != '{')
			{
			return false;
			}
		stream.ignore();

		JStringPtrMap<JString>* map = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
		assert( map != nullptr );
		if (ParseMap(stream, map))
			{
			list->Append(map);
			}
		else
			{
			jdelete map;
			return false;
			}
		stream.ignore();

		c = stream.peek();
		if (c == ']')
			{
			return true;
			}
		if (c == ',')
			{
			stream.ignore();
			}
		else
			{
			return false;
			}
		}
}

/******************************************************************************
 SendPing

	Pings gdb to see if it is ready.  Used when gdb is in asynch mode (OSX).

 *****************************************************************************/

void
GDBLink::SendPing()
{
	itsPingID++;
	if (itsPingID == 0)
		{
		itsPingID++;
		}
	const JString idStr((JUInt64) itsPingID);
	const JUtf8Byte* map[] =
		{
		"id", idStr.GetBytes()
		};
	const JString cmd = JGetString("PingCommand::GDBLink", map, sizeof(map));
	SendRaw(cmd);
}

/******************************************************************************
 SendWhenStopped

	Sends the given text as command(s) to gdb after the program stops.

 *****************************************************************************/

void
GDBLink::SendWhenStopped
	(
	const JString& text
	)
{
	Send(text);
}

/******************************************************************************
 Send

	Sends the given text as command(s) to gdb.

 *****************************************************************************/

void
GDBLink::Send
	(
	const JString& text
	)
{
	if (itsOutputLink != nullptr)
		{
		if (ProgramIsRunning())
			{
			StopProgram();
			}

		SendRaw(text);
		}
}

/******************************************************************************
 SendRaw

	Sends the given text as text to whatever is currently accepting text.

 *****************************************************************************/

void
GDBLink::SendRaw
	(
	const JString& text
	)
{
	if (itsOutputLink != nullptr)
		{
		itsOutputLink->Write(text);
		Broadcast(DebugOutput(text, kCommandType));

		if (!itsDebuggerBusyFlag)
			{
			itsDebuggerBusyFlag = true;
			Broadcast(DebuggerBusy());
			}
		}
}

/******************************************************************************
 SendMedicCommand (virtual protected)

 *****************************************************************************/

void
GDBLink::SendMedicCommand
	(
	CMCommand* command
	)
{
	if (itsOutputLink == nullptr)
		{
		return;
		}

	command->Starting();

	if (!itsFirstBreakFlag && !itsProgramIsStoppedFlag)
		{
		itsContinueCount = 1;	// only at final prompt will itsForegroundQ be empty
		}

	const JString id((JUInt64) command->GetTransactionID());

	const JUtf8Byte *startId, *endId;
	if (command->WillIgnoreResult())
		{
		startId = "StartIgnoreResultCommand::GDBLink";
		endId   = "EndIgnoreResultCommand::GDBLink";
		}
	else
		{
		startId = "StartCommand::GDBLink";
		endId   = "EndCommand::GDBLink";
		}

	const JUtf8Byte* map[] =
		{
		"id", id.GetBytes()
		};
	JString cmd = JGetString(startId, map, sizeof(map));
	Send(cmd);	// switching this to SendRaw() will cripple opening of source files while the program is running

	SendRaw(command->GetCommand());

	cmd = JGetString(endId, map, sizeof(map));
	SendRaw(cmd);
}

/******************************************************************************
 ProgramStarted

	It would be nice to detect "program finished" by *only* listening to
	itsChildProcess, but this doesn't work for remote debugging.

 *****************************************************************************/

void
GDBLink::ProgramStarted
	(
	const pid_t pid
	)
{
	jdelete itsChildProcess;
	itsChildProcess = nullptr;

	if (pid != 0)
		{
		itsChildProcess = jnew JProcess(pid);
		assert( itsChildProcess != nullptr );
		ListenTo(itsChildProcess);

		std::ostringstream log;
		log << "Program started; pid=" << pid;
		Log(log);
		}
}

/******************************************************************************
 ProgramFinished1 (private)

	It would be nice to detect "program finished" by *only* listening to
	itsChildProcess, but this doesn't work for remote debugging.

 *****************************************************************************/

void
GDBLink::ProgramFinished1()
{
	jdelete itsChildProcess;
	itsChildProcess = nullptr;

	if (itsIsAttachedFlag)
		{
		Broadcast(DetachedFromProcess());
		itsIsAttachedFlag = false;
		}

	if (itsIsDebuggingFlag)
		{
		itsIsDebuggingFlag = false;
		Broadcast(ProgramFinished());
		}
}

/******************************************************************************
 StopProgram

 *****************************************************************************/

void
GDBLink::StopProgram()
{
	if (itsFirstBreakFlag)
		{
		// wait for tbreak
		}
	else if (itsChildProcess != nullptr)
		{
		Log("stopping program");
		itsChildProcess->SendSignal(SIGINT);
		}
//	else if (itsDebuggerProcess != nullptr)	// remote debugging
//		{
//		itsDebuggerProcess->SendSignal(SIGINT);
//		}
	else
		{
		auto* dlog =
			jnew CMChooseProcessDialog(JXGetApplication(), false, true);
		assert( dlog != nullptr );
		dlog->BeginDialog();
		}
}

/******************************************************************************
 KillProgram

 *****************************************************************************/

void
GDBLink::KillProgram()
{
	SendWhenStopped(JString("kill", JString::kNoCopy));
}

/******************************************************************************
 DetachOrKill (private)

 *****************************************************************************/

void
GDBLink::DetachOrKill()
{
	if (itsIsAttachedFlag)
		{
		Send(JString("detach", JString::kNoCopy));
		}
	else if (itsChildProcess != nullptr)
		{
		Send(JString("kill", JString::kNoCopy));
		}
}

/******************************************************************************
 OKToDetachOrKill

 *****************************************************************************/

bool
GDBLink::OKToDetachOrKill()
	const
{
	if (itsIsAttachedFlag)
		{
		return JGetUserNotification()->AskUserYes(JGetString("WarnDetachProgram::GDBLink"));
		}
	else if (itsIsDebuggingFlag)
		{
		return JGetUserNotification()->AskUserYes(JGetString("WarnKillProgram::GDBLink"));
		}
	else
		{
		return true;
		}
}

/******************************************************************************
 StartDebugger (private)

	We cannot send anything to gdb until it has successfully started.

 *****************************************************************************/

bool
GDBLink::StartDebugger()
{
	assert( itsDebuggerProcess == nullptr && itsChildProcess == nullptr );

	itsScanner->Reset();

	itsDebuggerCmd    = CMGetPrefsManager()->GetGDBCommand();
	const JString cmd = itsDebuggerCmd + " --fullname --interpreter=mi2";

	int toFD, fromFD;
	const JError err = JProcess::Create(&itsDebuggerProcess, cmd,
										kJCreatePipe, &toFD,
										kJCreatePipe, &fromFD,
										kJAttachToFromFD, nullptr);
	if (err.OK())
		{
		itsOutputLink = new ProcessLink(toFD);
		assert( itsOutputLink != nullptr );

		itsInputLink = new ProcessLink(fromFD);
		assert( itsInputLink != nullptr );
		ListenTo(itsInputLink);

		ListenTo(itsDebuggerProcess);

		itsWaitingToQuitFlag = false;
		return true;
		}
	else
		{
		JGetStringManager()->ReportError("UnableToStartDebugger::GDBLink", err);
		return false;
		}
}

/******************************************************************************
 InitDebugger (private)

	We send the init commands after receiving first prompt so gdb startup
	messages are printed to cmd window.

 *****************************************************************************/

void
GDBLink::InitDebugger()
{
	Send(JGetString("InitCommands::GDBLink"));

	itsHasStartedFlag = true;
	Broadcast(DebuggerStarted());

	if (!itsProgramName.IsEmpty())
		{
		SetProgram(itsProgramName);
		}
	if (!itsCoreName.IsEmpty())
		{
		SetCore(itsCoreName);
		}

	itsInitFinishedFlag = true;
}

/******************************************************************************
 ChangeDebugger

 *****************************************************************************/

bool
GDBLink::ChangeDebugger()
{
	CMPrefsManager* mgr = CMGetPrefsManager();
	if (itsDebuggerCmd != mgr->GetGDBCommand())
		{
		return RestartDebugger();
		}
	else
		{
		return true;
		}
}

/******************************************************************************
 RestartDebugger

 *****************************************************************************/

bool
GDBLink::RestartDebugger()
{
	const bool symbolsWereLoaded = itsSymbolsLoadedFlag;

	StopDebugger();
	const bool ok = StartDebugger();

	if (ok && symbolsWereLoaded)
		{
		Broadcast(DebuggerRestarted());
		}

	return ok;
}

/******************************************************************************
 StopDebugger (private)

 *****************************************************************************/

void
GDBLink::StopDebugger()
{
	DetachOrKill();
	Send(JString("quit", JString::kNoCopy));

	jdelete itsDebuggerProcess;
	itsDebuggerProcess = nullptr;

	jdelete itsChildProcess;
	itsChildProcess = nullptr;

	delete itsOutputLink;
	itsOutputLink = nullptr;

	delete itsInputLink;
	itsInputLink = nullptr;

	CancelAllCommands();

	InitFlags();
}
