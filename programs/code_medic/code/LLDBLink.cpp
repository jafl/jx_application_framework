/******************************************************************************
 LLDBLink.cpp

	Interface to lldb.

	BASE CLASS = CMLink

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#include "LLDBLink.h"
#include "LLDBBreakpointManager.h"
#include "LLDBWelcomeTask.h"
#include "LLDBSymbolsLoadedTask.h"
#include "LLDBRunBackgroundCommandTask.h"

#include "LLDBArray2DCommand.h"
#include "LLDBPlot2DCommand.h"
#include "LLDBDisplaySourceForMain.h"
#include "LLDBGetCompletions.h"
#include "LLDBGetFrame.h"
#include "LLDBGetStack.h"
#include "LLDBGetThread.h"
#include "LLDBGetThreads.h"
#include "LLDBGetMemory.h"
#include "LLDBGetAssembly.h"
#include "LLDBGetRegisters.h"
#include "LLDBGetFullPath.h"
#include "LLDBGetInitArgs.h"
#include "LLDBGetLocalVars.h"
#include "LLDBGetSourceFileList.h"
#include "LLDBVarCommand.h"
#include "LLDBVarNode.h"

// must be before X11 includes which #define Success
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBBreakpoint.h"
#include "lldb/API/SBBreakpointLocation.h"
#include "lldb/API/SBListener.h"
#include "lldb/API/SBEvent.h"

#include "cmGlobals.h"

#include <JXAssert.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jProcessUtil.h>
#include <jSysUtil.h>
#include <jAssert.h>

extern char **environ;

static const JBoolean kFeatures[]=
{
	kJTrue,		// kSetProgram
	kJTrue,		// kSetArgs
	kJTrue,		// kSetCore -- lldb requires program to be chosen first, unlike gdb
	kJTrue,		// kSetProcess -- lldb requires program to be chosen first, unlike gdb
	kJTrue,		// kRunProgram
	kJTrue,		// kStopProgram
	kJTrue,		// kSetExecutionPoint
	kJFalse,	// kExecuteBackwards
	kJTrue,		// kShowBreakpointInfo
	kJTrue,		// kSetBreakpointCondition
	kJTrue,		// kSetBreakpointIgnoreCount
	kJTrue,		// kWatchExpression
	kJTrue,		// kWatchLocation
	kJTrue,		// kExamineMemory
	kJTrue,		// kDisassembleMemory
};

const uint32_t kLLDBEventMask = 0xFFFFFFFF;

/******************************************************************************
 Constructor

 *****************************************************************************/

LLDBLink::LLDBLink()
	:
	CMLink(kFeatures),
	SBListener("LLDBLink"),		// without a name, the listener is invalid
	itsDebugger(NULL),
	itsStdoutStream(NULL),
	itsStderrStream(NULL),
	itsPrompt(NULL)
{
	InitFlags();

	itsBPMgr = jnew LLDBBreakpointManager(this);
	assert( itsBPMgr != NULL );

	StartDebugger(kJFalse);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

LLDBLink::~LLDBLink()
{
	StopDebugger();

	jdelete itsBPMgr;
	jdelete itsPrompt;

	DeleteOneShotCommands();
}

/******************************************************************************
 InitFlags (private)

 *****************************************************************************/

void
LLDBLink::InitFlags()
{
	itsIsAttachedFlag = kJFalse;
}

/******************************************************************************
 GetPrompt

 ******************************************************************************/

const JString&
LLDBLink::GetPrompt()
	const
{
	jdelete itsPrompt;
	const_cast<LLDBLink*>(this)->itsPrompt = jnew JString(itsDebugger->GetPrompt());
	assert( itsPrompt != NULL );

	return *itsPrompt;
}

/******************************************************************************
 GetScriptPrompt

 ******************************************************************************/

const JString&
LLDBLink::GetScriptPrompt()
	const
{
	return JGetString("ScriptPrompt::LLDBLink");
}

/******************************************************************************
 DebuggerHasStarted

 ******************************************************************************/

JBoolean
LLDBLink::DebuggerHasStarted()
	const
{
	return kJTrue;
}

/******************************************************************************
 GetChooseProgramInstructions

 ******************************************************************************/

JString
LLDBLink::GetChooseProgramInstructions()
	const
{
	return JGetString("ChooseProgramInstr::LLDBLink");
}

/******************************************************************************
 HasProgram

 ******************************************************************************/

JBoolean
LLDBLink::HasProgram()
	const
{
	return JI2B(itsDebugger->GetNumTargets() > 0);
}

/******************************************************************************
 GetProgram

 ******************************************************************************/

JBoolean
LLDBLink::GetProgram
	(
	JString* fullName
	)
	const
{
	lldb::SBFileSpec f = itsDebugger->GetSelectedTarget().GetExecutable();
	if (f.Exists())
	{
		*fullName = JCombinePathAndName(f.GetDirectory(), f.GetFilename());
		return kJTrue;
	}
	else
	{
		fullName->Clear();
		return kJFalse;
	}
}

/******************************************************************************
 HasCore

 ******************************************************************************/

JBoolean
LLDBLink::HasCore()
	const
{
	return !itsCoreName.IsEmpty();
}

/******************************************************************************
 GetCore

 ******************************************************************************/

JBoolean
LLDBLink::GetCore
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
LLDBLink::HasLoadedSymbols()
	const
{
	lldb::SBProcess p     = itsDebugger->GetSelectedTarget().GetProcess();
	lldb::StateType state = p.GetState();
	return JI2B(p.IsValid() &&
				state != lldb::eStateInvalid  &&
				state != lldb::eStateUnloaded &&
				state != lldb::eStateConnected);
}

/******************************************************************************
 IsDebugging

 *****************************************************************************/

JBoolean
LLDBLink::IsDebugging()
	const
{
	lldb::SBProcess p     = itsDebugger->GetSelectedTarget().GetProcess();
	lldb::StateType state = p.GetState();
	return JI2B(p.IsValid() &&
				(state == lldb::eStateAttaching ||
				 state == lldb::eStateLaunching ||
				 state == lldb::eStateRunning   ||
				 state == lldb::eStateStepping  ||
				 state == lldb::eStateStopped   ||
				 state == lldb::eStateCrashed   ||
				 state == lldb::eStateSuspended));
}

/******************************************************************************
 ProgramIsRunning

 *****************************************************************************/

JBoolean
LLDBLink::ProgramIsRunning()
	const
{
	lldb::SBProcess p     = itsDebugger->GetSelectedTarget().GetProcess();
	lldb::StateType state = p.GetState();
	return JI2B(p.IsValid() &&
				(state == lldb::eStateAttaching ||
				 state == lldb::eStateLaunching ||
				 state == lldb::eStateRunning   ||
				 state == lldb::eStateStepping));
}

/******************************************************************************
 ProgramIsStopped

 *****************************************************************************/

JBoolean
LLDBLink::ProgramIsStopped()
	const
{
	lldb::SBProcess p     = itsDebugger->GetSelectedTarget().GetProcess();
	lldb::StateType state = p.GetState();
	return JI2B(p.IsValid() &&
				(state == lldb::eStateStopped   ||
				 state == lldb::eStateCrashed   ||
				 state == lldb::eStateSuspended));
}

/******************************************************************************
 OKToSendMultipleCommands

 *****************************************************************************/

JBoolean
LLDBLink::OKToSendMultipleCommands()
	const
{
	return CMLink::OKToSendMultipleCommands();
}

/******************************************************************************
 OKToSendCommands

 *****************************************************************************/

JBoolean
LLDBLink::OKToSendCommands
	(
	const JBoolean background
	)
	const
{
	return JNegate(ProgramIsRunning());
}

/******************************************************************************
 IsDefiningScript

 *****************************************************************************/

JBoolean
LLDBLink::IsDefiningScript()
	const
{
	return kJFalse;
}

/******************************************************************************
 HandleLLDBEvent

 *****************************************************************************/

void
LLDBLink::HandleLLDBEvent()
{
	// read from LLDB

	JCharacter buf[1024];
	JSize count = fread(buf, sizeof(JCharacter), 1023, itsDebugger->GetOutputFileHandle());
	if (count > 0)
		{
		buf[ count ] = '\0';
		Broadcast(UserOutput(buf, kJFalse));
		}

	count = fread(buf, sizeof(JCharacter), 1023, itsDebugger->GetErrorFileHandle());
	if (count > 0)
		{
		buf[ count ] = '\0';
		Broadcast(UserOutput(buf, kJTrue));
		}

	// read from process

	lldb::SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	if (p.IsValid())
		{
		count = p.GetSTDOUT(buf, 1023);
		if (count > 0)
			{
			buf[ count ]  = '\0';
			JCharacter* b = buf;

			const JSize len = itsLastProgramInput.GetLength();
			if (len < count &&
				JCompareMaxN(itsLastProgramInput, len, buf, count, len) &&
				(buf[len] == '\n' || buf[len] == '\r'))
				{
				b += len;
				while (*b == '\n' || *b == '\r')
					{
					b++;
					}
				}
			itsLastProgramInput.Clear();

			Broadcast(UserOutput(b, kJFalse, kJTrue));
			}

		count = p.GetSTDERR(buf, 1023);
		if (count > 0)
			{
			buf[ count ] = '\0';
			Broadcast(UserOutput(buf, kJTrue, kJTrue));
			}
		}

	// check for events

	lldb::SBEvent e;
	if (GetNextEvent(e))
		{
		HandleLLDBEvent(e);
		}
}

/******************************************************************************
 HandleLLDBEvent (private)

 *****************************************************************************/

void
LLDBLink::HandleLLDBEvent
	(
	const lldb::SBEvent& e
	)
{
	if (!e.IsValid())
		{
		return;
		}

	const JCharacter* eventClass = e.GetBroadcasterClass();
	const uint32_t eventType     = e.GetType();

	JString msg = eventClass;
	msg        += ":";
	msg        += JString(eventType, JString::kBase10);

	if (lldb::SBProcess::EventIsProcessEvent(e))
		{
		const lldb::StateType state = lldb::SBProcess::GetStateFromEvent(e);
		msg += "; process state: " + JString(state, JString::kBase10) + ", " + JString(lldb::SBProcess::GetRestartedFromEvent(e), JString::kBase10);

		if (state == lldb::eStateRunning ||
			state == lldb::eStateStepping)
			{
			CancelBackgroundCommands();
			Broadcast(DebuggerBusy());
			Broadcast(ProgramRunning());
			}
		else if (state == lldb::eStateStopped ||
				 state == lldb::eStateCrashed)
			{
			Broadcast(DebuggerReadyForInput());
			RunNextCommand();

			JString s;
			if (ProgramStopped(&s))
				{
//				Send("call (JXGetAssertHandler())->UnlockDisplays()");
				msg += s;
				}
			}
		else if (state == lldb::eStateExited ||
				 state == lldb::eStateDetached)
			{
			ProgramFinished1();
			}
		}
	else if (lldb::SBThread::EventIsThreadEvent(e))
		{
		lldb::SBThread t = lldb::SBThread::GetThreadFromEvent(e);
		lldb::SBFrame f  = lldb::SBThread::GetStackFrameFromEvent(e);

		if (eventType & lldb::SBThread::eBroadcastBitThreadSelected)
			{
			// sync with SwitchToThread()
			Broadcast(ThreadChanged());
			ProgramStopped();
			}
		else if ((eventType & lldb::SBThread::eBroadcastBitStackChanged) ||
				 (eventType & lldb::SBThread::eBroadcastBitSelectedFrameChanged))
			{
			// sync with SwitchToFrame()
			Broadcast(FrameChanged());
			ProgramStopped();
			}
		}
	else if (lldb::SBBreakpoint::EventIsBreakpointEvent(e))
		{
		msg += "; bkpt event type: " + JString(lldb::SBBreakpoint::GetBreakpointEventTypeFromEvent(e), JString::kBase10);
		Broadcast(BreakpointsChanged());
		}
	else if (lldb::SBWatchpoint::EventIsWatchpointEvent(e))
		{
		msg += "; watch event type: " + JString(lldb::SBWatchpoint::GetWatchpointEventTypeFromEvent(e), JString::kBase10);
		Broadcast(BreakpointsChanged());
		}
	else if (lldb::SBCommandInterpreter::EventIsCommandInterpreterEvent(e))
		{
		if (eventType & lldb::SBCommandInterpreter::eBroadcastBitQuitCommandReceived)
			{
			(JXGetApplication())->Quit();
			}
		}

	Broadcast(DebugOutput(msg, kLogType));
}

/******************************************************************************
 ReceiveLLDBMessageLine (private static)

 *****************************************************************************/

int
LLDBLink::ReceiveLLDBMessageLine
	(
	void*		baton,
	const char*	line,
	int			count
	)
{
	const JString msg(line, count);
	static_cast<LLDBLink*>(baton)->Broadcast(UserOutput(msg, kJFalse));
	return count;
}

/******************************************************************************
 ReceiveLLDBErrorLine (private static)

 *****************************************************************************/

int
LLDBLink::ReceiveLLDBErrorLine
	(
	void*		baton,
	const char*	line,
	int			count
	)
{
	const JString msg(line, count);
	static_cast<LLDBLink*>(baton)->Broadcast(UserOutput(msg, kJTrue));
	return count;
}

/******************************************************************************
 LogLLDBMessage (private static)

 *****************************************************************************/

void
LLDBLink::LogLLDBMessage
	(
	const JCharacter*	msg,
	void*				baton
	)
{
	static_cast<LLDBLink*>(baton)->Broadcast(DebugOutput(msg, kLogType));
}

/******************************************************************************
 SetProgram

 *****************************************************************************/

void
LLDBLink::SetProgram
	(
	const JCharacter* fullName
	)
{
	DetachOrKill(kJTrue);

	lldb::SBTarget t = itsDebugger->CreateTarget(fullName);
	if (t.IsValid())
		{
		StartListeningForEvents(t.GetBroadcaster(), kLLDBEventMask);

		LLDBSymbolsLoadedTask* task = jnew LLDBSymbolsLoadedTask(fullName);
		assert( task != NULL );
		task->Go();
		}
}

/******************************************************************************
 SymbolsLoaded

	XXX: we never get eBroadcastBitSymbolsLoaded

 *****************************************************************************/

void
LLDBLink::SymbolsLoaded
	(
	const JCharacter* fullName
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);
	Broadcast(PrepareToLoadSymbols());
	Broadcast(CMLink::SymbolsLoaded(kJTrue, name));
}

/******************************************************************************
 ReloadProgram

 *****************************************************************************/

void
LLDBLink::ReloadProgram()
{
	JString fullName;
	if (GetProgram(&fullName))
		{
		SetProgram(fullName);
		}
}

/******************************************************************************
 SetCore

 *****************************************************************************/

void
LLDBLink::SetCore
	(
	const JCharacter* fullName
	)
{
	DetachOrKill(kJFalse);

	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		t.LoadCore(fullName);
		itsCoreName = fullName;
		Broadcast(CoreLoaded());
		}
	else
		{
		JString cmdStr = "target create --core ";
		cmdStr        += fullName;
		SendRaw(cmdStr);
		SendRaw("frame select 1");
		SendRaw("frame select 0");
		}
}

/******************************************************************************
 AttachToProcess

 *****************************************************************************/

void
LLDBLink::AttachToProcess
	(
	const pid_t pid
	)
{
	DetachOrKill(kJTrue);

	lldb::SBTarget t = itsDebugger->CreateTarget("");
	if (t.IsValid())
		{
		lldb::SBAttachInfo info(pid);
		info.SetListener(*this);

		lldb::SBError e;
		t.Attach(info, e);

		if (e.Fail())
			{
			Broadcast(UserOutput(e.GetCString(), kJTrue));
			}
		else if (t.IsValid())
			{
			itsIsAttachedFlag = kJTrue;
			StartListeningForEvents(t.GetBroadcaster(), kLLDBEventMask);

			lldb::SBFileSpec f     = t.GetExecutable();
			const JString fullName = JCombinePathAndName(f.GetDirectory(), f.GetFilename());

			LLDBSymbolsLoadedTask* task = jnew LLDBSymbolsLoadedTask(fullName);
			assert( task != NULL );
			task->Go();

			Broadcast(AttachedToProcess());
			}
		}
}

/******************************************************************************
 RunProgram

 *****************************************************************************/

void
LLDBLink::RunProgram
	(
	const JCharacter* args
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		DetachOrKill(kJFalse);

		JCharacter** lldbArgs = NULL;

		JPtrArray<JString> argList(JPtrArrayT::kDeleteAll);
		if (strlen(args) > 0)
			{
			JParseArgsForExec(args, &argList);

			lldbArgs = jnew JCharacter*[ argList.GetElementCount()+1 ];
			assert( lldbArgs != NULL );

			for (JIndex i=1; i<=argList.GetElementCount(); i++)
				{
				lldbArgs[ i-1 ] = const_cast<JCharacter*>(argList.GetElement(i)->GetCString());
				}

			lldbArgs[ argList.GetElementCount() ] = NULL;
			}

		lldb::SBError error;
		t.Launch(*this, (const char**) lldbArgs, (const char**) environ,
				 NULL, NULL, NULL, ".", 0, false, error);

		jdelete [] lldbArgs;
		}
}

/******************************************************************************
 GetBreakpointManager

 *****************************************************************************/

CMBreakpointManager*
LLDBLink::GetBreakpointManager()
{
	return itsBPMgr;
}

/******************************************************************************
 ShowBreakpointInfo

 *****************************************************************************/

void
LLDBLink::ShowBreakpointInfo
	(
	const JIndex debuggerIndex
	)
{
	SendRaw("breakpoint list");
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
LLDBLink::SetBreakpoint
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex,
	const JBoolean		temporary
	)
{
	JString path, name;
	JSplitPathAndName(fileName, &path, &name);

	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		lldb::SBBreakpoint b = t.BreakpointCreateByLocation(name, lineIndex);
		b.SetOneShot(temporary);
		}
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
LLDBLink::SetBreakpoint
	(
	const JCharacter*	address,
	const JBoolean		temporary
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		JCharacter* end;
		const lldb::addr_t a = strtoull(address, &end, 0);
		lldb::SBBreakpoint b = t.BreakpointCreateByAddress(a);
		b.SetOneShot(temporary);
		}
}

/******************************************************************************
 RemoveBreakpoint

 *****************************************************************************/

void
LLDBLink::RemoveBreakpoint
	(
	const JIndex debuggerIndex
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		t.BreakpointDelete(debuggerIndex);
		}
}

/******************************************************************************
 RemoveAllBreakpointsOnLine

 *****************************************************************************/

void
LLDBLink::RemoveAllBreakpointsOnLine
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (!t.IsValid())
		{
		return;
		}

	JString path, name;
	JSplitPathAndName(fileName, &path, &name);

	const JSize bpCount = t.GetNumBreakpoints();
	for (long i=bpCount-1; i>=0; i--)
		{
		lldb::SBBreakpoint b = t.GetBreakpointAtIndex(i);

		const JSize locCount = b.GetNumLocations();
		for (JIndex j=0; j<locCount; j++)
			{
			lldb::SBAddress a   = b.GetLocationAtIndex(j).GetAddress();
			lldb::SBLineEntry e = a.GetLineEntry();
			if (e.GetLine() == lineIndex &&
				JString::Compare(e.GetFileSpec().GetFilename(), name) == 0)
				{
				t.BreakpointDelete(b.GetID());
				}
			}
		}
}

/******************************************************************************
 RemoveAllBreakpointsAtAddress

 *****************************************************************************/

void
LLDBLink::RemoveAllBreakpointsAtAddress
	(
	const JCharacter* addrStr
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (!t.IsValid())
		{
		return;
		}

	JCharacter* end;
	const lldb::addr_t addr = strtoull(addrStr, &end, 0);

	const JSize bpCount = t.GetNumBreakpoints();
	for (long i=bpCount-1; i>=0; i--)
		{
		lldb::SBBreakpoint b = t.GetBreakpointAtIndex(i);

		const JSize locCount = b.GetNumLocations();
		for (JIndex j=0; j<locCount; j++)
			{
			lldb::SBAddress a = b.GetLocationAtIndex(j).GetAddress();
			if (a.GetLoadAddress(t) == addr)
				{
				t.BreakpointDelete(b.GetID());
				}
			}
		}
}

/******************************************************************************
 RemoveAllBreakpoints

 *****************************************************************************/

void
LLDBLink::RemoveAllBreakpoints()
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		t.DeleteAllBreakpoints();
		}
}

/******************************************************************************
 SetBreakpointEnabled

 *****************************************************************************/

void
LLDBLink::SetBreakpointEnabled
	(
	const JIndex	debuggerIndex,
	const JBoolean	enabled,
	const JBoolean	once
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		lldb::SBBreakpoint b = t.FindBreakpointByID(debuggerIndex);
		b.SetEnabled(enabled);
		b.SetOneShot(once);
		}
}

/******************************************************************************
 SetBreakpointCondition

 *****************************************************************************/

void
LLDBLink::SetBreakpointCondition
	(
	const JIndex		debuggerIndex,
	const JCharacter*	condition
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		t.FindBreakpointByID(debuggerIndex).SetCondition(condition);
		}
}

/******************************************************************************
 RemoveBreakpointCondition

 *****************************************************************************/

void
LLDBLink::RemoveBreakpointCondition
	(
	const JIndex debuggerIndex
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		t.FindBreakpointByID(debuggerIndex).SetCondition("");
		}
}

/******************************************************************************
 SetBreakpointIgnoreCount

 *****************************************************************************/

void
LLDBLink::SetBreakpointIgnoreCount
	(
	const JIndex	debuggerIndex,
	const JSize		count
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		t.FindBreakpointByID(debuggerIndex).SetIgnoreCount(count);
		}
}

/******************************************************************************
 WatchExpression

 *****************************************************************************/

void
LLDBLink::WatchExpression
	(
	const JCharacter* expr
	)
{
	Watch(expr, kJFalse);
}

/******************************************************************************
 WatchLocation

 *****************************************************************************/

void
LLDBLink::WatchLocation
	(
	const JCharacter* expr
	)
{
	Watch(expr, kJTrue);
}

/******************************************************************************
 Watch (private)

 *****************************************************************************/

void
LLDBLink::Watch
	(
	const JCharacter*	expr,
	const JBoolean		resolveAddress
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		lldb::SBValue v = t.EvaluateExpression(expr);
		if (v.IsValid())
			{
			v.Watch(resolveAddress, false, true);
			}
		}
}

/******************************************************************************
 SwitchToThread

 *****************************************************************************/

void
LLDBLink::SwitchToThread
	(
	const JUInt64 id
	)
{
	lldb::SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	if (p.IsValid() && ProgramIsStopped())
		{
		p.SetSelectedThreadByID(id);	// does not broadcast

		// sync with HandleLLDBEvent()
		Broadcast(ThreadChanged());
		ProgramStopped();
		}
}

/******************************************************************************
 SwitchToFrame

 *****************************************************************************/

void
LLDBLink::SwitchToFrame
	(
	const JUInt64 id
	)
{
	lldb::SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
		{
		t.SetSelectedFrame(id);		// does not broadcast

		// sync with HandleLLDBEvent()
		Broadcast(FrameChanged());
		ProgramStopped();
		}
}

/******************************************************************************
 StepOver

 *****************************************************************************/

void
LLDBLink::StepOver()
{
	lldb::SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
		{
		t.StepOver();
		}
}

/******************************************************************************
 StepInto

 *****************************************************************************/

void
LLDBLink::StepInto()
{
	lldb::SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
		{
		t.StepInto();
		}
}

/******************************************************************************
 StepOut

 *****************************************************************************/

void
LLDBLink::StepOut()
{
	lldb::SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
		{
		t.StepOut();
		}
}

/******************************************************************************
 Continue

 *****************************************************************************/

void
LLDBLink::Continue()
{
	lldb::SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	if (p.IsValid() && ProgramIsStopped())
		{
		p.Continue();
		}
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
LLDBLink::RunUntil
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
	if (ProgramIsStopped())
		{
		SetBreakpoint(fileName, lineIndex, kJTrue);
		Continue();
		}
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
LLDBLink::SetExecutionPoint
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
	if (ProgramIsStopped())
		{
		JString cmd = "_regexp-jump ";
		cmd        += fileName;
		cmd        += ":";
		cmd        += JString(lineIndex, JString::kBase10);
		itsDebugger->HandleCommand(cmd);
		}
}

/******************************************************************************
 StepOverAssembly

 *****************************************************************************/

void
LLDBLink::StepOverAssembly()
{
	lldb::SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
		{
		t.StepInstruction(true);
		}
}

/******************************************************************************
 StepIntoAssembly

 *****************************************************************************/

void
LLDBLink::StepIntoAssembly()
{
	lldb::SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
		{
		t.StepInstruction(false);
		}
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
LLDBLink::RunUntil
	(
	const JCharacter* addr
	)
{
	lldb::SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
		{
		JCharacter* end;
		const lldb::addr_t a = strtoull(addr, &end, 0);
		t.RunToAddress(a);
		}
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
LLDBLink::SetExecutionPoint
	(
	const JCharacter* addr
	)
{
	if (ProgramIsStopped())
		{
		JString cmd = "_regexp-jump *";
		cmd        += addr;
		itsDebugger->HandleCommand(cmd);
		}
}

/******************************************************************************
 BackupOver

 *****************************************************************************/

void
LLDBLink::BackupOver()
{
}

/******************************************************************************
 BackupInto

 *****************************************************************************/

void
LLDBLink::BackupInto()
{
}

/******************************************************************************
 BackupOut

 *****************************************************************************/

void
LLDBLink::BackupOut()
{
}

/******************************************************************************
 BackupContinue

 *****************************************************************************/

void
LLDBLink::BackupContinue()
{
}

/******************************************************************************
 SetValue

 *****************************************************************************/

void
LLDBLink::SetValue
	(
	const JCharacter* name,
	const JCharacter* value
	)
{
	lldb::SBFrame f = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();
	if (f.IsValid())
		{
		JString expr = name;
		expr        += " = ";
		expr        += value;
		f.EvaluateExpression(expr);

		Broadcast(ValueChanged());
		}
}

/******************************************************************************
 CreateArray2DCommand

 *****************************************************************************/

CMArray2DCommand*
LLDBLink::CreateArray2DCommand
	(
	CMArray2DDir*		dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
{
	CMArray2DCommand* cmd = jnew LLDBArray2DCommand(dir, table, data);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreatePlot2DCommand

 *****************************************************************************/

CMPlot2DCommand*
LLDBLink::CreatePlot2DCommand
	(
	CMPlot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
{
	CMPlot2DCommand* cmd = jnew LLDBPlot2DCommand(dir, x, y);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateDisplaySourceForMain

 *****************************************************************************/

CMDisplaySourceForMain*
LLDBLink::CreateDisplaySourceForMain
	(
	CMSourceDirector* sourceDir
	)
{
	CMDisplaySourceForMain* cmd = jnew LLDBDisplaySourceForMain(sourceDir);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetCompletions

 *****************************************************************************/

CMGetCompletions*
LLDBLink::CreateGetCompletions
	(
	CMCommandInput*	input,
	CMCommandOutputDisplay*	history
	)
{
	CMGetCompletions* cmd = jnew LLDBGetCompletions(input, history);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetFrame

 *****************************************************************************/

CMGetFrame*
LLDBLink::CreateGetFrame
	(
	CMStackWidget* widget
	)
{
	CMGetFrame* cmd = jnew LLDBGetFrame(widget);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetStack

 *****************************************************************************/

CMGetStack*
LLDBLink::CreateGetStack
	(
	JTree*			tree,
	CMStackWidget*	widget
	)
{
	CMGetStack* cmd = jnew LLDBGetStack(tree, widget);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetThread

 *****************************************************************************/

CMGetThread*
LLDBLink::CreateGetThread
	(
	CMThreadsWidget* widget
	)
{
	CMGetThread* cmd = jnew LLDBGetThread(widget);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetThreads

 *****************************************************************************/

CMGetThreads*
LLDBLink::CreateGetThreads
	(
	JTree*				tree,
	CMThreadsWidget*	widget
	)
{
	CMGetThreads* cmd = jnew LLDBGetThreads(tree, widget);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetFullPath

 *****************************************************************************/

CMGetFullPath*
LLDBLink::CreateGetFullPath
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
	CMGetFullPath* cmd = jnew LLDBGetFullPath(fileName, lineIndex);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetInitArgs

 *****************************************************************************/

CMGetInitArgs*
LLDBLink::CreateGetInitArgs
	(
	JXInputField* argInput
	)
{
	CMGetInitArgs* cmd = jnew LLDBGetInitArgs(argInput);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetLocalVars

 *****************************************************************************/

CMGetLocalVars*
LLDBLink::CreateGetLocalVars
	(
	CMVarNode* rootNode
	)
{
	CMGetLocalVars* cmd = jnew LLDBGetLocalVars(rootNode);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetSourceFileList

 *****************************************************************************/

CMGetSourceFileList*
LLDBLink::CreateGetSourceFileList
	(
	CMFileListDir* fileList
	)
{
	CMGetSourceFileList* cmd = jnew LLDBGetSourceFileList(fileList);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateVarValueCommand

 *****************************************************************************/

CMVarCommand*
LLDBLink::CreateVarValueCommand
	(
	const JCharacter* expr
	)
{
	CMVarCommand* cmd = jnew LLDBVarCommand(expr);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateVarContentCommand

 *****************************************************************************/

CMVarCommand*
LLDBLink::CreateVarContentCommand
	(
	const JCharacter* expr
	)
{
	JString s = "*(";
	s        += expr;
	s        += ")";

	CMVarCommand* cmd = jnew LLDBVarCommand(s);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateVarNode

 *****************************************************************************/

CMVarNode*
LLDBLink::CreateVarNode
	(
	const JBoolean shouldUpdate		// kJFalse for Local Variables
	)
{
	CMVarNode* node = jnew LLDBVarNode(shouldUpdate);
	assert( node != NULL );
	return node;
}

CMVarNode*
LLDBLink::CreateVarNode
	(
	JTreeNode*			parent,
	const JCharacter*	name,
	const JCharacter*	fullName,
	const JCharacter*	value
	)
{
	CMVarNode* node = jnew LLDBVarNode(parent, name, value);
	assert( node != NULL );
	return node;
}

/******************************************************************************
 Build1DArrayExpression

 *****************************************************************************/

JString
LLDBLink::Build1DArrayExpression
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
LLDBLink::Build2DArrayExpression
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
LLDBLink::CreateGetMemory
	(
	CMMemoryDir* dir
	)
{
	CMGetMemory* cmd = jnew LLDBGetMemory(dir);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetAssembly

 *****************************************************************************/

CMGetAssembly*
LLDBLink::CreateGetAssembly
	(
	CMSourceDirector* dir
	)
{
	CMGetAssembly* cmd = jnew LLDBGetAssembly(dir);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetRegisters

 *****************************************************************************/

CMGetRegisters*
LLDBLink::CreateGetRegisters
	(
	CMRegistersDir* dir
	)
{
	CMGetRegisters* cmd = jnew LLDBGetRegisters(dir);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 SendRaw

	Sends the given text as text to whatever is currently accepting text.

 *****************************************************************************/

void
LLDBLink::SendRaw
	(
	const JCharacter* text
	)
{
	if (ProgramIsRunning())
		{
		lldb::SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
		p.PutSTDIN(text, strlen(text));
		p.PutSTDIN("\n", 1);
		itsLastProgramInput = text;
		}
	else
		{
		itsDebugger->HandleCommand(text);
		Broadcast(DebugOutput(text, kCommandType));
		}
}

/******************************************************************************
 SendMedicCommand (virtual protected)

 *****************************************************************************/

void
LLDBLink::SendMedicCommand
	(
	CMCommand* command
	)
{
	command->Starting();

	JXUrgentTask* task = jnew LLDBRunBackgroundCommandTask(command);
	assert( task != NULL );
	task->Go();
}

/******************************************************************************
 SendMedicCommandSync

 *****************************************************************************/

void
LLDBLink::SendMedicCommandSync
	(
	CMCommand* command
	)
{
	HandleCommandRunning(command->GetTransactionID());
	command->Finished(kJTrue);	// may delete object
	SetRunningCommand(NULL);

	if (!HasForegroundCommands())
		{
		RunNextCommand();
		}
}

/******************************************************************************
 ProgramStarted (private)

 *****************************************************************************/

void
LLDBLink::ProgramStarted
	(
	const pid_t pid
	)
{
	std::ostringstream log;
	log << "Program started; pid=" << pid;
	Log(log);
}

/******************************************************************************
 ProgramStopped (private)

 *****************************************************************************/

JBoolean
LLDBLink::ProgramStopped
	(
	JString* msg
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	lldb::SBFrame f  = t.GetProcess().GetSelectedThread().GetSelectedFrame();
	if (f.IsValid())
		{
		lldb::SBLineEntry line = f.GetLineEntry();
		lldb::SBFileSpec file  = line.GetFileSpec();
		JString fullName;
		if (file.IsValid())
			{
			fullName = JCombinePathAndName(file.GetDirectory(), file.GetFilename());
			}
		CMLocation location(fullName, line.IsValid() ? line.GetLine() : 0);

		if (f.GetFunctionName() != NULL)
			{
			location.SetFunctionName(f.GetFunctionName());
			}

		lldb::SBAddress addr = f.GetPCAddress();
		if (addr.IsValid())
			{
			const JString a = JString(addr.GetLoadAddress(t), JString::kBase16);
			location.SetMemoryAddress(a);
			}
		Broadcast(CMLink::ProgramStopped(location));

		if (msg != NULL)
			{
			*msg = "; file: " + location.GetFileName() + ", line: " + JString(location.GetLineNumber(), JString::kBase10) + ", func: " + location.GetFunctionName() + ", addr: " + location.GetMemoryAddress();
			}
		return kJTrue;
		}
	else
		{
		if (msg != NULL)
			{
			msg->Clear();
			}
		return kJFalse;
		}
}

/******************************************************************************
 ProgramFinished1 (private)

 *****************************************************************************/

void
LLDBLink::ProgramFinished1()
{
	if (itsIsAttachedFlag)
		{
		Broadcast(DetachedFromProcess());
		itsIsAttachedFlag = kJFalse;
		}

	lldb::SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	JString reasonStr;
	if (p.IsValid() && p.GetExitDescription() != NULL)
		{
		reasonStr  = p.GetExitDescription();
		reasonStr += "\n\n";
		}
	else
		{
		int result;
		const JChildExitReason cer = JDecodeChildExitReason(p.GetExitStatus(), &result);
		reasonStr                  = JPrintChildExitReason(cer, result) + "\n\n";
		}

	Broadcast(UserOutput(reasonStr, kJFalse));
	Broadcast(ProgramFinished());

	Broadcast(DebuggerReadyForInput());
	RunNextCommand();
}

/******************************************************************************
 StopProgram

 *****************************************************************************/

void
LLDBLink::StopProgram()
{
	Log("stopping program");

	lldb::SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	if (p.IsValid())
		{
		p.Stop();
		}
}

/******************************************************************************
 KillProgram

 *****************************************************************************/

void
LLDBLink::KillProgram()
{
	lldb::SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	if (p.IsValid())
		{
		p.Kill();
		}
}

/******************************************************************************
 DetachOrKill (private)

 *****************************************************************************/

void
LLDBLink::DetachOrKill
	(
	const JBoolean destroyTarget
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		lldb::SBProcess p = t.GetProcess();
		if (itsIsAttachedFlag)
			{
			p.Detach();
			itsIsAttachedFlag = kJFalse;
			}
		else
			{
			p.Kill();
			}

		if (destroyTarget)
			{
			itsDebugger->DeleteTarget(t);
			InitFlags();
			}
		}
}

/******************************************************************************
 OKToDetachOrKill

 *****************************************************************************/

JBoolean
LLDBLink::OKToDetachOrKill()
	const
{
	if (itsIsAttachedFlag)
		{
		return (JGetUserNotification())->AskUserYes(JGetString("WarnDetachProgram::LLDBLink"));
		}
	else if (IsDebugging())
		{
		return (JGetUserNotification())->AskUserYes(JGetString("WarnKillProgram::LLDBLink"));
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 StartDebugger (private)

 *****************************************************************************/

JBoolean
LLDBLink::StartDebugger
	(
	const JBoolean restart
	)
{
	assert( itsDebugger == NULL );

	itsDebugger = jnew lldb::SBDebugger(lldb::SBDebugger::Create(true, LogLLDBMessage, this));
	if (itsDebugger->IsValid())
		{
		StartListeningForEvents(itsDebugger->GetCommandInterpreter().GetBroadcaster(), kLLDBEventMask);
		StartListeningForEventClass(*itsDebugger, lldb::SBThread::GetBroadcasterClassName(), kLLDBEventMask);

		assert( itsStdoutStream == NULL );
		assert( itsStderrStream == NULL );

#ifdef _J_OSX
		itsStdoutStream = fwopen(this, ReceiveLLDBMessageLine);
		assert( itsStdoutStream != NULL );

		itsStderrStream = fwopen(this, ReceiveLLDBErrorLine);
		assert( itsStderrStream != NULL );
#else
		cookie_io_functions_t my_fns = { NULL, ReceiveLLDBMessageLine, NULL, NULL };

		itsStdoutStream = fopencookie(this, "w", my_fns);
		assert( itsStdoutStream != NULL );

		my_fns.write    = ReceiveLLDBErrorLine;
		itsStderrStream = fopencookie(this, "w", my_fns);
		assert( itsStderrStream != NULL );
#endif

		setvbuf(itsStdoutStream, NULL, _IOLBF, 0);
		itsDebugger->SetOutputFileHandle(itsStdoutStream, false);

		setvbuf(itsStderrStream, NULL, _IOLBF, 0);
		itsDebugger->SetErrorFileHandle(itsStderrStream, false);

		const JCharacter* map[] =
			{
			"debugger", itsDebugger->GetVersionString()
			};
		JString msg = JGetString("Welcome::LLDBLink", map, sizeof(map));

		LLDBWelcomeTask* task = jnew LLDBWelcomeTask(msg, restart);
		assert( task != NULL );
		task->Go();

		return kJTrue;
		}
	else
		{
		(JGetStringManager())->ReportError("UnableToStartDebugger::LLDBLink", "");
		return kJFalse;
		}
}

/******************************************************************************
 StopDebugger (private)

 *****************************************************************************/

void
LLDBLink::StopDebugger()
{
	DetachOrKill(kJTrue);

	fclose(itsStdoutStream);
	itsStdoutStream = NULL;

	fclose(itsStderrStream);
	itsStderrStream = NULL;

	lldb::SBDebugger::Destroy(*itsDebugger);
	jdelete itsDebugger;
	itsDebugger = NULL;

	InitFlags();
}

/******************************************************************************
 RestartDebugger

 *****************************************************************************/

JBoolean
LLDBLink::RestartDebugger()
{
	const JBoolean symbolsWereLoaded = HasLoadedSymbols();

	StopDebugger();
	return StartDebugger(symbolsWereLoaded);
}

/******************************************************************************
 ChangeDebugger

 *****************************************************************************/

JBoolean
LLDBLink::ChangeDebugger()
{
	return kJTrue;
}
