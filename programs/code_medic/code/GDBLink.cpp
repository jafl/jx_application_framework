/******************************************************************************
 GDBLink.cpp

	Interface to gdb.

	http://ftp.gnu.org/pub/old-gnu/Manuals/gdb-5.1.1/html_node/gdb_211.html
	http://www.devworld.apple.com/documentation/DeveloperTools/gdb/gdb/gdb_25.html

	BASE CLASS = CMLink

	Copyright (C) 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "GDBLink.h"
#include "GDBScanner.h"
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

static const JBoolean kFeatures[]=
{
	kJTrue,		// kSetProgram
	kJTrue,		// kSetArgs
	kJTrue,		// kSetCore
	kJTrue,		// kSetProcess
	kJTrue,		// kRunProgram
	kJTrue,		// kStopProgram
	kJTrue,		// kSetExecutionPoint
	kJTrue,		// kExecuteBackwards
	kJTrue,		// kShowBreakpointInfo
	kJTrue,		// kSetBreakpointCondition
	kJTrue,		// kSetBreakpointIgnoreCount
	kJTrue,		// kWatchExpression
	kJTrue,		// kWatchLocation
	kJTrue,		// kExamineMemory
	kJTrue,		// kDisassembleMemory
};

/******************************************************************************
 Constructor

 *****************************************************************************/

GDBLink::GDBLink()
	:
	CMLink(kFeatures),
	itsDebuggerProcess(NULL),
	itsChildProcess(NULL),
	itsOutputLink(NULL),
	itsInputLink(NULL)
{
	InitFlags();

	itsScanner = jnew GDBScanner;
	assert( itsScanner != NULL );

	itsBPMgr = jnew GDBBreakpointManager(this);
	assert( itsBPMgr != NULL );

	itsGetStopLocation = jnew GDBGetStopLocationForLink();
	assert( itsGetStopLocation != NULL );

	itsGetStopLocation2 = jnew GDBGetStopLocationForAsm();
	assert( itsGetStopLocation2 != NULL );

	itsPingTask = jnew GDBPingTask();
	assert( itsPingTask != NULL );

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
	itsHasStartedFlag           = kJFalse;
	itsInitFinishedFlag         = kJFalse;
	itsSymbolsLoadedFlag        = kJFalse;
	itsDebuggerBusyFlag         = kJTrue;
	itsIsDebuggingFlag          = kJFalse;
	itsIgnoreNextMaybeReadyFlag = kJFalse;
	itsIsAttachedFlag           = kJFalse;
	itsProgramIsStoppedFlag     = kJTrue;
	itsFirstBreakFlag           = kJFalse;
	itsPrintingOutputFlag       = kJTrue;	// print welcome message
	itsDefiningScriptFlag       = kJFalse;
	itsWaitingToQuitFlag        = kJFalse;
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

JBoolean
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

JBoolean
GDBLink::HasProgram()
	const
{
	return !itsProgramName.IsEmpty();
}

/******************************************************************************
 GetProgram

 ******************************************************************************/

JBoolean
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

JBoolean
GDBLink::HasCore()
	const
{
	return !itsCoreName.IsEmpty();
}

/******************************************************************************
 GetCore

 ******************************************************************************/

JBoolean
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

JBoolean
GDBLink::HasLoadedSymbols()
	const
{
	return itsSymbolsLoadedFlag;
}

/******************************************************************************
 IsDebugging

 *****************************************************************************/

JBoolean
GDBLink::IsDebugging()
	const
{
	return itsIsDebuggingFlag;
}

/******************************************************************************
 ProgramIsRunning

 *****************************************************************************/

JBoolean
GDBLink::ProgramIsRunning()
	const
{
	return JI2B(itsIsDebuggingFlag && !itsProgramIsStoppedFlag);
}

/******************************************************************************
 ProgramIsStopped

 *****************************************************************************/

JBoolean
GDBLink::ProgramIsStopped()
	const
{
	return JI2B(itsIsDebuggingFlag && itsProgramIsStoppedFlag);
}

/******************************************************************************
 OKToSendMultipleCommands

 *****************************************************************************/

JBoolean
GDBLink::OKToSendMultipleCommands()
	const
{
	#ifdef _J_OLD_OSX
	return kJFalse;
	#else
	return CMLink::OKToSendMultipleCommands();
	#endif
}

/******************************************************************************
 OKToSendCommands

 *****************************************************************************/

JBoolean
GDBLink::OKToSendCommands
	(
	const JBoolean background
	)
	const
{
	#ifdef _J_OLD_OSX
	return (background ? JI2B(itsContinueCount == 0) :
						 !itsDebuggerBusyFlag);
	#else
	return JI2B(itsContinueCount == 0);
	#endif
}

/******************************************************************************
 IsDefiningScript

 *****************************************************************************/

JBoolean
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
			(JGetUserNotification())->AskUserYes(JGetString("DebuggerCrashed::GDBLink")))
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

static JRegex pingRecvPattern =
	"^(&\"echo \\\\\\\\032\\\\\\\\032:Medic debugger ready:[[:digit:]]+:\\\\n\"\n"
	"\\^error,msg=\"Cannot execute this command while the target is running.\"\n"
	"\\(gdb\\) \n)+$";

void
GDBLink::ReadFromDebugger()
{
	JString data;
	itsInputLink->Read(&data);

#ifdef _J_OLD_OSX
	pingRecvPattern.SetSingleLine();
	if (!pingRecvPattern.Match(data))
		{
#endif

		Broadcast(DebugOutput(data, kOutputType));

#ifdef _J_OLD_OSX
		}
#endif

	itsScanner->AppendInput(data);
	while (1)
		{
		const GDBScanner::Token token = itsScanner->NextToken();
		if (token.type == GDBScanner::kEOF)
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

		const JBoolean wasStopped = itsProgramIsStoppedFlag;
		if (token.type == GDBScanner::kMaybeReadyForInput)
			{
			if (!itsIgnoreNextMaybeReadyFlag)
				{
				SendPing();
				itsPingTask->Start();
				itsIgnoreNextMaybeReadyFlag = kJTrue;
				}
			else
				{
				itsIgnoreNextMaybeReadyFlag = kJFalse;
				}
			continue;
			}
		else if (token.type == GDBScanner::kReadyForInput)
			{
			#ifdef _J_OLD_OSX
			itsIgnoreNextMaybeReadyFlag = kJTrue;
			#endif

			itsPingTask->Stop();

			if (0 < token.data.number && token.data.number < itsPingID)
				{
				continue;
				}

			itsProgramIsStoppedFlag = kJTrue;
//			itsPrintingOutputFlag   = kJTrue;
			itsDefiningScriptFlag   = kJFalse;

			itsDebuggerBusyFlag = kJFalse;
			Broadcast(DebuggerReadyForInput());
			RunNextCommand();

			// If we stopped the program in order to send commands to the
			// debugger, we need to continue once everything settles down.
			// Unless it is invoked by a CMCommand, itsContinueCount has to
			// start at 2 because we have to ignore the prompt received
			// immediately after sending SIGINT.

			if (itsContinueCount > 0 && !HasForegroundCommands())
				{
				#ifdef _J_OLD_OSX
				itsContinueCount = 0;
				#else
				itsContinueCount--;
				#endif

				if (itsContinueCount == 0)
					{
					Send("continue");
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
		else if (!itsDebuggerBusyFlag && !itsIgnoreNextMaybeReadyFlag)
			{
			itsDebuggerBusyFlag = kJTrue;
			Broadcast(DebuggerBusy());
			}

		// process token

		if (token.type == GDBScanner::kProgramOutput)
			{
			if (itsPrintingOutputFlag)
				{
				// We cannot tell the difference between gdb and program output
				Broadcast(UserOutput(*(token.data.pString), kJFalse, kJFalse));
				}
			}
		else if (token.type == GDBScanner::kErrorOutput)
			{
			if (itsPrintingOutputFlag)
				{
				Broadcast(UserOutput(*(token.data.pString), kJTrue));
				}
			}

		else if (token.type == GDBScanner::kBeginMedicCmd ||
				 token.type == GDBScanner::kBeginMedicIgnoreCmd)
			{
			HandleCommandRunning(token.data.number);
			itsPrintingOutputFlag = kJFalse;
			}
		else if (token.type == GDBScanner::kCommandOutput)
			{
			CMCommand* cmd;
			if (GetRunningCommand(&cmd))
				{
				cmd->Accumulate(*(token.data.pString));
				}
			}
		else if (token.type == GDBScanner::kCommandResult)
			{
			CMCommand* cmd;
			if (GetRunningCommand(&cmd))
				{
				cmd->SaveResult(*(token.data.pString));
				}
			}
		else if (token.type == GDBScanner::kEndMedicCmd ||
				 token.type == GDBScanner::kEndMedicIgnoreCmd)
			{
			CMCommand* cmd;
			if (GetRunningCommand(&cmd))
				{
				cmd->Finished(kJTrue);	// may jdelete object
				SetRunningCommand(NULL);

				#ifdef _J_OLD_OSX
				RunNextCommand();
				#else
				if (!HasForegroundCommands())
					{
					RunNextCommand();
					}
				#endif
				}

			itsPrintingOutputFlag = kJTrue;
			}

		else if (token.type == GDBScanner::kBreakpointsChanged)
			{
			if (!itsFirstBreakFlag)		// ignore tbreak in hook-run
				{
				Broadcast(BreakpointsChanged());
				}
			}
		else if (token.type == GDBScanner::kFrameChanged)
			{
			Broadcast(FrameChanged());	// sync with kFrameChangedAndProgramStoppedAtLocation
			}
		else if (token.type == GDBScanner::kThreadChanged)
			{
			Broadcast(ThreadChanged());
			}
		else if (token.type == GDBScanner::kValueChanged)
			{
			Broadcast(ValueChanged());
			}

		else if (token.type == GDBScanner::kPrepareToLoadSymbols)
			{
			itsIsDebuggingFlag   = kJFalse;
			itsSymbolsLoadedFlag = kJFalse;
			itsProgramName.Clear();
			ClearFileNameMap();
			Broadcast(PrepareToLoadSymbols());
			}
		else if (token.type == GDBScanner::kFinishedLoadingSymbolsFromProgram)
			{
			itsSymbolsLoadedFlag = kJTrue;

			// We can't use a lexer to extract the file name from gdb's
			// output because the pattern "Reading symbols from [^\n]+..."
			// will slurp up "(no debugging symbols found)..." as well.

			GDBGetProgramName* cmd = jnew GDBGetProgramName;
			assert( cmd != NULL );

			Broadcast(UserOutput(*(token.data.pString), kJFalse));
			}
		else if (token.type == GDBScanner::kNoSymbolsInProgram)
			{
			if (!itsIsAttachedFlag)
				{
				if (token.data.pString != NULL)
					{
					Broadcast(UserOutput(*(token.data.pString), kJTrue));
					}

				JString name;
				Broadcast(SymbolsLoaded(kJFalse, name));
				}
			}
		else if (token.type == GDBScanner::kSymbolsReloaded)
			{
			Broadcast(PrepareToLoadSymbols());
			Broadcast(SymbolsReloaded());
			}

		else if (token.type == GDBScanner::kCoreChanged)
			{
			// We have to check whether a core was loaded or cleared.

			GDBCheckCoreStatus* cmd = jnew GDBCheckCoreStatus;
			assert( cmd != NULL );
			}

		else if (token.type == GDBScanner::kAttachedToProcess)
			{
			itsIsAttachedFlag  = kJTrue;
			itsIsDebuggingFlag = kJTrue;
			ProgramStarted(token.data.number);
			Broadcast(AttachedToProcess());
			}
		else if (token.type == GDBScanner::kDetachingFromProcess)
			{
			ProgramFinished1();
			}

		else if (token.type == GDBScanner::kProgramStarting)
			{
			itsIsDebuggingFlag      = kJTrue;
			itsProgramIsStoppedFlag = kJFalse;
			itsFirstBreakFlag       = kJTrue;
			itsPrintingOutputFlag   = kJFalse;	// ignore tbreak output
			}
		else if (token.type == GDBScanner::kBeginGetPID)
			{
			itsPrintingOutputFlag = kJFalse;	// ignore "info prog"
			}
		else if (token.type == GDBScanner::kProgramPID)
			{
			ProgramStarted(token.data.number);
			}
		else if (token.type == GDBScanner::kEndGetPID)
			{
			if (itsChildProcess == NULL)	// ask user for PID
				{
				CMChooseProcessDialog* dialog =
					jnew CMChooseProcessDialog(JXGetApplication(), kJFalse);
				assert( dialog != NULL );
				dialog->BeginDialog();
				}
			}

		else if (token.type == GDBScanner::kProgramStoppedAtLocation)
			{
			itsProgramIsStoppedFlag = kJTrue;
			if (token.data.pLocation != NULL)
				{
				SendProgramStopped(*(token.data.pLocation));
//				Send("call (JXGetAssertHandler())->UnlockDisplays()");
				}
			else
				{
				itsGetStopLocation->Send();
				}

			if (token.data.pString != NULL)
				{
				Broadcast(UserOutput(*(token.data.pString), kJFalse));
				}
			}
		else if (token.type == GDBScanner::kFrameChangedAndProgramStoppedAtLocation)
			{
			Broadcast(FrameChanged());	// sync with kFrameChanged

			itsProgramIsStoppedFlag = kJTrue;
			itsGetStopLocation->Send();
			}

		else if (token.type == GDBScanner::kBeginScriptDefinition)
			{
			itsDefiningScriptFlag = kJTrue;
			Broadcast(DebuggerDefiningScript());
			}

		else if (token.type == GDBScanner::kPlugInMessage)
			{
			Broadcast(PlugInMessage(*(token.data.pString)));
			}

		else if (token.type == GDBScanner::kProgramRunning)
			{
			itsProgramIsStoppedFlag = kJFalse;
			CancelBackgroundCommands();
			Broadcast(ProgramRunning());
			}
		else if (token.type == GDBScanner::kProgramFinished)
			{
			ProgramFinished1();

			if (token.data.pString != NULL)
				{
				Broadcast(UserOutput(*(token.data.pString), kJFalse));
				}
			}
		else if (token.type == GDBScanner::kProgramKilled)
			{
			ProgramFinished1();
			}

		else if (token.type == GDBScanner::kDebuggerFinished)
			{
			(JXGetApplication())->Quit();
			itsWaitingToQuitFlag = kJTrue;
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
	const JCharacter* fileName
	)
{
	itsProgramName = fileName;

	JString path, name;
	if (!itsProgramName.IsEmpty())
		{
		JSplitPathAndName(itsProgramName, &path, &name);
		}

	Broadcast(SymbolsLoaded(kJTrue, name));
}

/******************************************************************************
 SaveCoreName

	Callback for GDBCheckCoreStatus.

 *****************************************************************************/

void
GDBLink::SaveCoreName
	(
	const JCharacter* fileName
	)
{
	itsCoreName = fileName;
	if (!itsCoreName.IsEmpty())
		{
		itsIsDebuggingFlag = kJFalse;
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

		itsFirstBreakFlag     = kJFalse;
		itsPrintingOutputFlag = kJTrue;
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

		itsFirstBreakFlag     = kJFalse;
		itsPrintingOutputFlag = kJTrue;		// "info prog" is finished

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
			Send("continue");
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
	const JCharacter* fullName
	)
{
	if (itsHasStartedFlag)
		{
		if (itsInitFinishedFlag && !JSameDirEntry(fullName, itsProgramName))
			{
			Send("jdelete");
			}
		DetachOrKill();
		Send("core-file");

		JString cmd  = "file ";
		cmd         += JPrepArgForExec(fullName);
		Send(cmd);
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
	const JCharacter* fullName
	)
{
	if (itsHasStartedFlag)
		{
		DetachOrKill();

		// can't use JPrepArgForExec() because gdb doesn't like the quotes

		JString cmdStr = "core-file ";
		cmdStr        += fullName;
		if (itsProgramName.IsEmpty())
			{
			GDBAnalyzeCore* cmd = jnew GDBAnalyzeCore(cmdStr);
			assert( cmd != NULL );
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
	Send("core-file");
	DetachOrKill();

	JString cmd = "attach ";
	cmd        += JString(pid, JString::kBase10);
	Send(cmd);
}

/******************************************************************************
 RunProgram

 *****************************************************************************/

void
GDBLink::RunProgram
	(
	const JCharacter* args
	)
{
	Send("core-file");
	DetachOrKill();

	JString cmd  = "set args ";
	cmd         += args;
	Send(cmd);

	Send("run");
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

	JString cmd = "echo \\n\ninfo breakpoint ";
	cmd        += JString(debuggerIndex, JString::kBase10);
	cmd        += "\necho \\n";
	SendWhenStopped(cmd);
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
GDBLink::SetBreakpoint
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex,
	const JBoolean		temporary
	)
{
	JString path, name;
	JSplitPathAndName(fileName, &path, &name);

	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	JString cmd = "echo \\032\\032:Medic breakpoints changed:\n";
	cmd += (temporary ? "-break-insert -t " : "-break-insert ");
	#ifdef _J_OLD_OSX
	cmd += "-l -1 ";
	#endif
	cmd += name + ":" + JString(lineIndex, JString::kBase10);
	SendWhenStopped(cmd);
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
GDBLink::SetBreakpoint
	(
	const JCharacter*	address,
	const JBoolean		temporary
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	JString cmd = "echo \\032\\032:Medic breakpoints changed:\n";
	cmd += (temporary ? "-break-insert -t " : "-break-insert ");
	#ifdef _J_OLD_OSX
	cmd += "-l -1 ";
	#endif
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

	const JString cmd = "jdelete " + JString(debuggerIndex, JString::kBase10);
	SendWhenStopped(cmd);
}

/******************************************************************************
 RemoveAllBreakpointsOnLine

 *****************************************************************************/

void
GDBLink::RemoveAllBreakpointsOnLine
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
	JString path, name;
	JSplitPathAndName(fileName, &path, &name);

	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	const JString cmd = "clear " + name + ":" + JString(lineIndex, JString::kBase10);
	SendWhenStopped(cmd);
}

/******************************************************************************
 RemoveAllBreakpointsAtAddress

 *****************************************************************************/

void
GDBLink::RemoveAllBreakpointsAtAddress
	(
	const JCharacter* addr
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

	SendWhenStopped("jdelete");
}

/******************************************************************************
 SetBreakpointEnabled

 *****************************************************************************/

void
GDBLink::SetBreakpointEnabled
	(
	const JIndex	debuggerIndex,
	const JBoolean	enabled,
	const JBoolean	once
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	const JString cmd =
		(enabled ? "enable " : "disable ") + 
		((once ? "once " : "" ) + JString(debuggerIndex, JString::kBase10));
	SendWhenStopped(cmd);
}

/******************************************************************************
 SetBreakpointCondition

 *****************************************************************************/

void
GDBLink::SetBreakpointCondition
	(
	const JIndex		debuggerIndex,
	const JCharacter*	condition
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	const JString cmd = "condition " + JString(debuggerIndex, JString::kBase10) + " " + condition;
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

	const JString cmd = "condition " + JString(debuggerIndex, JString::kBase10);
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

	const JString cmd = "ignore " + JString(debuggerIndex, JString::kBase10) + " " + JString(count, JString::kBase10);
	SendWhenStopped(cmd);
}

/******************************************************************************
 WatchExpression

 *****************************************************************************/

void
GDBLink::WatchExpression
	(
	const JCharacter* expr
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	JString cmd = "watch ";
	cmd        += expr;
	SendWhenStopped(cmd);
}

/******************************************************************************
 WatchLocation

 *****************************************************************************/

void
GDBLink::WatchLocation
	(
	const JCharacter* expr
	)
{
	if (!itsProgramIsStoppedFlag)
		{
		itsContinueCount = 2;
		}

	JString cmd = "watch -location ";
	cmd        += expr;
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
		itsPrintingOutputFlag = kJFalse;

		JString cmd = "thread ";
		cmd        += JString(id, JString::kBase10);
		Send(cmd);
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
		itsPrintingOutputFlag = kJFalse;

		JString cmd = "frame ";
		cmd        += JString(id, JString::kBase10);
		Send(cmd);
		}
}

/******************************************************************************
 StepOver

 *****************************************************************************/

void
GDBLink::StepOver()
{
	Send("next");
}

/******************************************************************************
 StepInto

 *****************************************************************************/

void
GDBLink::StepInto()
{
	Send("step");
}

/******************************************************************************
 StepOut

 *****************************************************************************/

void
GDBLink::StepOut()
{
	Send("finish");
}

/******************************************************************************
 Continue

 *****************************************************************************/

void
GDBLink::Continue()
{
	Send("continue");
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
GDBLink::RunUntil
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
	if (ProgramIsStopped())
		{
		JString path, name;
		JSplitPathAndName(fileName, &path, &name);

		const JString cmd = "until " + name + ":" + JString(lineIndex, JString::kBase10);
		Send(cmd);
		}
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
GDBLink::SetExecutionPoint
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
	if (ProgramIsStopped())
		{
		JString path, name;
		JSplitPathAndName(fileName, &path, &name);

		const JString loc = name + ":" + JString(lineIndex, JString::kBase10);

		JString cmd = "tbreak " + loc;
		SendRaw(cmd);

		cmd = "jump " + loc;
		SendRaw(cmd);
		}
}

/******************************************************************************
 StepOverAssembly

 *****************************************************************************/

void
GDBLink::StepOverAssembly()
{
	Send("nexti");
}

/******************************************************************************
 StepIntoAssembly

 *****************************************************************************/

void
GDBLink::StepIntoAssembly()
{
	Send("stepi");
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
GDBLink::RunUntil
	(
	const JCharacter* addr
	)
{
	if (ProgramIsStopped())
		{
		JString cmd = "until *";
		cmd        += addr;
		Send(cmd);
		}
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
GDBLink::SetExecutionPoint
	(
	const JCharacter* addr
	)
{
	if (ProgramIsStopped())
		{
		JString loc = "*";
		loc        += addr;

		JString cmd = "tbreak " + loc;
		SendRaw(cmd);

		cmd = "jump " + loc;
		SendRaw(cmd);
		}
}

/******************************************************************************
 BackupOver

 *****************************************************************************/

void
GDBLink::BackupOver()
{
	Send("reverse-next");
}

/******************************************************************************
 BackupInto

 *****************************************************************************/

void
GDBLink::BackupInto()
{
	Send("reverse-step");
}

/******************************************************************************
 BackupOut

 *****************************************************************************/

void
GDBLink::BackupOut()
{
	Send("reverse-finish");
}

/******************************************************************************
 BackupContinue

 *****************************************************************************/

void
GDBLink::BackupContinue()
{
	Send("reverse-continue");
}

/******************************************************************************
 SetValue

 *****************************************************************************/

void
GDBLink::SetValue
	(
	const JCharacter* name,
	const JCharacter* value
	)
{
	if (ProgramIsStopped())
		{
		JString cmd = "set variable ";
		cmd        += name;
		cmd        += " = ";
		cmd        += value;
		Send(cmd);

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
	assert( cmd != NULL );
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
	assert( cmd != NULL );
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
	assert( cmd != NULL );
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
	assert( cmd != NULL );
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
	assert( cmd != NULL );
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
	assert( cmd != NULL );
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
	assert( cmd != NULL );
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
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetFullPath

 *****************************************************************************/

CMGetFullPath*
GDBLink::CreateGetFullPath
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
	#ifdef _J_OLD_OSX
	if (ProgramIsRunning())
		{
		StopProgram();
		if (!itsFirstBreakFlag)
			{
			itsContinueCount = 1;	// only at final prompt will itsForegroundQ be empty
			}
		}
	#endif

	CMGetFullPath* cmd = jnew GDBGetFullPath(fileName, lineIndex);
	assert( cmd != NULL );
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
	assert( cmd != NULL );
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
	assert( cmd != NULL );
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
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateVarValueCommand

 *****************************************************************************/

CMVarCommand*
GDBLink::CreateVarValueCommand
	(
	const JCharacter* expr
	)
{
	JString s = "print ";
	s        += expr;

	CMVarCommand* cmd = jnew GDBVarCommand(s);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateVarContentCommand

 *****************************************************************************/

CMVarCommand*
GDBLink::CreateVarContentCommand
	(
	const JCharacter* expr
	)
{
	JString s = "print *(";
	s        += expr;
	s        += ")";

	CMVarCommand* cmd = jnew GDBVarCommand(s);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateVarNode

 *****************************************************************************/

CMVarNode*
GDBLink::CreateVarNode
	(
	const JBoolean shouldUpdate		// kJFalse for Local Variables
	)
{
	CMVarNode* node = jnew GDBVarNode(shouldUpdate);
	assert( node != NULL );
	return node;
}

CMVarNode*
GDBLink::CreateVarNode
	(
	JTreeNode*			parent,
	const JCharacter*	name,
	const JCharacter*	fullName,
	const JCharacter*	value
	)
{
	CMVarNode* node = jnew GDBVarNode(parent, name, value);
	assert( node != NULL );
	return node;
}

/******************************************************************************
 Build1DArrayExpression

 *****************************************************************************/

JString
GDBLink::Build1DArrayExpression
	(
	const JCharacter*	expr,
	const JInteger		index
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
	const JCharacter*	expr,
	const JInteger		rowIndex,
	const JInteger		colIndex
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
	assert( cmd != NULL );
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
	assert( cmd != NULL );
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
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 ParseList (static)

	Reads data of the format "...","...","...".  It terminates when it
	finds a closing brace.

 *****************************************************************************/

JBoolean
GDBLink::ParseList
	(
	std::istringstream&	stream,
	JPtrArray<JString>*	list,
	const JCharacter	terminator
	)
{
	list->CleanOut();

	JString value;
	while (1)
		{
		stream >> value;
		if (!stream.good())
			{
			return kJFalse;
			}

		list->Append(value);

		int c = stream.peek();
		if (c == terminator)
			{
			return kJTrue;
			}
		if (c == ',')
			{
			stream.ignore();
			}
		else
			{
			return kJFalse;
			}
		}
}

/******************************************************************************
 ParseMap (static)

	Reads data of the format x="...",y="...",z="...".  It terminates when
	it finds a closing brace.

 *****************************************************************************/

JBoolean
GDBLink::ParseMap
	(
	std::istringstream&		stream,
	JStringPtrMap<JString>*	map
	)
{
	map->CleanOut();

	JString key, value;
	JBoolean found;
	while (1)
		{
		key = JReadUntil(stream, '=', &found);
		if (!found)
			{
			return kJFalse;
			}

		const JCharacter next = stream.peek();
		if (next == '{' || next == '[')
			{
			stream.ignore();	// skip {
			JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
			if (!ParseList(stream, &list, next == '[' ? ']' : '}'))
				{
				return kJFalse;
				}
			stream.ignore();	// skip }

			value.Clear();
			const JSize count = list.GetElementCount();
			for (JIndex i=1; i<=count; i++)
				{
				if (i > 1)
					{
					value.AppendCharacter('\1');
					}
				value.Append(*(list.NthElement(i)));
				}
			}
		else
			{
			stream >> value;
			if (!stream.good())
				{
				return kJFalse;
				}
			}

		map->SetNewElement(key, value);

		int c = stream.peek();
		if (c == '}')
			{
			return kJTrue;
			}
		if (c == ',')
			{
			stream.ignore();
			}
		else
			{
			return kJFalse;
			}
		}
}

/******************************************************************************
 ParseMapArray (static)

	Reads data of the format {...},{...},{...}.  It terminates when it
	finds a closing square bracket.

 *****************************************************************************/

JBoolean
GDBLink::ParseMapArray
	(
	std::istringstream&						stream,
	JPtrArray< JStringPtrMap<JString> >*	list
	)
{
	list->CleanOut();

	JBoolean found;
	while (1)
		{
		int c = stream.peek();
		if (c == ']')
			{
			return kJTrue;
			}
		else if (c != '{')
			{
			return kJFalse;
			}
		stream.ignore();

		JStringPtrMap<JString>* map = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
		assert( map != NULL );
		if (ParseMap(stream, map))
			{
			list->Append(map);
			}
		else
			{
			jdelete map;
			return kJFalse;
			}
		stream.ignore();

		c = stream.peek();
		if (c == ']')
			{
			return kJTrue;
			}
		if (c == ',')
			{
			stream.ignore();
			}
		else
			{
			return kJFalse;
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
	const JString idStr(itsPingID, JString::kBase10);
	const JCharacter* map[] =
		{
		"id", idStr
		};
	const JString cmd = JGetString("PingCommand::GDBLink", map, sizeof(map));
	SendRaw(cmd);
}

/******************************************************************************
 SendWhenStopped

	Sends the given text as command(s) to gdb after the program stops.

 *****************************************************************************/

static const JCharacter* kFakeCommandPrefix = "echo \\032\\032:Medic command:0:\n";
static const JCharacter* kFakeCommandSuffix = "echo \\032\\032:Medic command done:0:\n";

void
GDBLink::SendWhenStopped
	(
	const JCharacter* text
	)
{
#ifdef _J_OLD_OSX

	if (itsOutputLink != NULL)
		{
		if (ProgramIsRunning())
			{
			StopProgram();
			}

		JString s = text;
		if (s.BeginsWith(kFakeCommandPrefix))
			{
			s.RemoveSubstring(1, strlen(kFakeCommandPrefix));
			}
		if (s.EndsWith(kFakeCommandSuffix))
			{
			const JSize length = s.GetLength();
			s.RemoveSubstring(length - strlen(kFakeCommandSuffix) + 1, length);
			}

		CMCommand* cmd = jnew GDBSimpleCommand(s);
		assert( cmd != NULL );
		cmd->Send();
		}

#else
	Send(text);
#endif
}

/******************************************************************************
 Send

	Sends the given text as command(s) to gdb.

 *****************************************************************************/

void
GDBLink::Send
	(
	const JCharacter* text
	)
{
	if (itsOutputLink != NULL)
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

static JRegex pingSendPattern =
	"^echo \\\\032\\\\032:Medic debugger ready:[[:digit:]]+:$";

void
GDBLink::SendRaw
	(
	const JCharacter* text
	)
{
	if (itsOutputLink != NULL)
		{
		itsOutputLink->Write(text);

#ifdef _J_OLD_OSX
		if (!pingSendPattern.Match(text))
			{
#endif

			Broadcast(DebugOutput(text, kCommandType));

#ifdef _J_OLD_OSX
			}
#endif

		if (!itsDebuggerBusyFlag)
			{
			itsDebuggerBusyFlag = kJTrue;
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
	if (itsOutputLink == NULL)
		{
		return;
		}

	command->Starting();

	if (!itsFirstBreakFlag && !itsProgramIsStoppedFlag)
		{
		itsContinueCount = 1;	// only at final prompt will itsForegroundQ be empty
		}

	const JString id(command->GetTransactionID(), JString::kBase10);

	const JCharacter *startId, *endId;
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

	const JCharacter* map[] =
		{
		"id", id
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
	itsChildProcess = NULL;

	if (pid != 0)
		{
		itsChildProcess = jnew JProcess(pid);
		assert( itsChildProcess != NULL );
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
	itsChildProcess = NULL;

	if (itsIsAttachedFlag)
		{
		Broadcast(DetachedFromProcess());
		itsIsAttachedFlag = kJFalse;
		}

	if (itsIsDebuggingFlag)
		{
		itsIsDebuggingFlag = kJFalse;
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
	else if (itsChildProcess != NULL)
		{
		Log("stopping program");
		itsChildProcess->SendSignal(SIGINT);
		}
//	else if (itsDebuggerProcess != NULL)	// remote debugging
//		{
//		itsDebuggerProcess->SendSignal(SIGINT);
//		}
	else
		{
		CMChooseProcessDialog* dlog =
			jnew CMChooseProcessDialog(JXGetApplication(), kJFalse, kJTrue);
		assert( dlog != NULL );
		dlog->BeginDialog();
		}
}

/******************************************************************************
 KillProgram

 *****************************************************************************/

void
GDBLink::KillProgram()
{
	SendWhenStopped("kill");
}

/******************************************************************************
 DetachOrKill (private)

 *****************************************************************************/

void
GDBLink::DetachOrKill()
{
	if (itsIsAttachedFlag)
		{
		Send("detach");
		}
	else if (itsChildProcess != NULL)
		{
		Send("kill");
		}
}

/******************************************************************************
 OKToDetachOrKill

 *****************************************************************************/

JBoolean
GDBLink::OKToDetachOrKill()
	const
{
	if (itsIsAttachedFlag)
		{
		return (JGetUserNotification())->AskUserYes(JGetString("WarnDetachProgram::GDBLink"));
		}
	else if (itsIsDebuggingFlag)
		{
		return (JGetUserNotification())->AskUserYes(JGetString("WarnKillProgram::GDBLink"));
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 StartDebugger (private)

	We cannot send anything to gdb until it has successfully started.

 *****************************************************************************/

JBoolean
GDBLink::StartDebugger()
{
	assert( itsDebuggerProcess == NULL && itsChildProcess == NULL );

	itsScanner->Reset();

	itsDebuggerCmd    = CMGetPrefsManager()->GetGDBCommand();
	const JString cmd = itsDebuggerCmd + " --fullname --interpreter=mi2";

	int toFD, fromFD;
	const JError err = JProcess::Create(&itsDebuggerProcess, cmd,
										kJCreatePipe, &toFD,
										kJCreatePipe, &fromFD,
										kJAttachToFromFD, NULL);
	if (err.OK())
		{
		itsOutputLink = new ProcessLink(toFD);
		assert( itsOutputLink != NULL );

		itsInputLink = new ProcessLink(fromFD);
		assert( itsInputLink != NULL );
		ListenTo(itsInputLink);

		ListenTo(itsDebuggerProcess);

		itsWaitingToQuitFlag = kJFalse;
		return kJTrue;
		}
	else
		{
		(JGetStringManager())->ReportError("UnableToStartDebugger::GDBLink", err);
		return kJFalse;
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

	itsHasStartedFlag = kJTrue;
	Broadcast(DebuggerStarted());

	if (!itsProgramName.IsEmpty())
		{
		SetProgram(itsProgramName);
		}
	if (!itsCoreName.IsEmpty())
		{
		SetCore(itsCoreName);
		}

	itsInitFinishedFlag = kJTrue;
}

/******************************************************************************
 ChangeDebugger

 *****************************************************************************/

JBoolean
GDBLink::ChangeDebugger()
{
	CMPrefsManager* mgr = CMGetPrefsManager();
	if (itsDebuggerCmd != mgr->GetGDBCommand())
		{
		return RestartDebugger();
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 RestartDebugger

 *****************************************************************************/

JBoolean
GDBLink::RestartDebugger()
{
	const JBoolean symbolsWereLoaded = itsSymbolsLoadedFlag;

	StopDebugger();
	const JBoolean ok = StartDebugger();

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
	Send("quit");

	jdelete itsDebuggerProcess;
	itsDebuggerProcess = NULL;

	jdelete itsChildProcess;
	itsChildProcess = NULL;

	delete itsOutputLink;
	itsOutputLink = NULL;

	delete itsInputLink;
	itsInputLink = NULL;

	CancelAllCommands();

	InitFlags();
}
