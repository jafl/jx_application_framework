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

static const bool kFeatures[]=
{
	true,		// kSetProgram
	true,		// kSetArgs
	true,		// kSetCore -- lldb requires program to be chosen first, unlike gdb
	true,		// kSetProcess -- lldb requires program to be chosen first, unlike gdb
	true,		// kRunProgram
	true,		// kStopProgram
	true,		// kSetExecutionPoint
	false,	// kExecuteBackwards
	true,		// kShowBreakpointInfo
	true,		// kSetBreakpointCondition
	true,		// kSetBreakpointIgnoreCount
	true,		// kWatchExpression
	true,		// kWatchLocation
	true,		// kExamineMemory
	true,		// kDisassembleMemory
};

const uint32_t kLLDBEventMask = 0xFFFFFFFF;

/******************************************************************************
 Constructor

 *****************************************************************************/

LLDBLink::LLDBLink()
	:
	CMLink(kFeatures),
	SBListener("LLDBLink"),		// without a name, the listener is invalid
	itsDebugger(nullptr),
	itsStdoutStream(nullptr),
	itsStderrStream(nullptr),
	itsPrompt(nullptr)
{
	InitFlags();

	itsBPMgr = jnew LLDBBreakpointManager(this);
	assert( itsBPMgr != nullptr );

	StartDebugger(false);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

LLDBLink::~LLDBLink()
{
	StopDebugger();

	jdelete itsBPMgr;

	DeleteOneShotCommands();
}

/******************************************************************************
 InitFlags (private)

 *****************************************************************************/

void
LLDBLink::InitFlags()
{
	itsIsAttachedFlag = false;
}

/******************************************************************************
 GetPrompt

 ******************************************************************************/

const JString&
LLDBLink::GetPrompt()
	const
{
	const_cast<LLDBLink*>(this)->itsPrompt = itsDebugger->GetPrompt();
	return itsPrompt;
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

bool
LLDBLink::DebuggerHasStarted()
	const
{
	return true;
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

bool
LLDBLink::HasProgram()
	const
{
	return itsDebugger->GetNumTargets() > 0;
}

/******************************************************************************
 GetProgram

 ******************************************************************************/

bool
LLDBLink::GetProgram
	(
	JString* fullName
	)
	const
{
	lldb::SBFileSpec f = itsDebugger->GetSelectedTarget().GetExecutable();
	if (f.Exists())
	{
		*fullName = JCombinePathAndName(
			JString(f.GetDirectory(), JString::kNoCopy),
			JString(f.GetFilename(), JString::kNoCopy));
		return true;
	}
	else
	{
		fullName->Clear();
		return false;
	}
}

/******************************************************************************
 HasCore

 ******************************************************************************/

bool
LLDBLink::HasCore()
	const
{
	return !itsCoreName.IsEmpty();
}

/******************************************************************************
 GetCore

 ******************************************************************************/

bool
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

bool
LLDBLink::HasLoadedSymbols()
	const
{
	lldb::SBProcess p     = itsDebugger->GetSelectedTarget().GetProcess();
	lldb::StateType state = p.GetState();
	return p.IsValid() &&
				state != lldb::eStateInvalid  &&
				state != lldb::eStateUnloaded &&
				state != lldb::eStateConnected;
}

/******************************************************************************
 IsDebugging

 *****************************************************************************/

bool
LLDBLink::IsDebugging()
	const
{
	lldb::SBProcess p     = itsDebugger->GetSelectedTarget().GetProcess();
	lldb::StateType state = p.GetState();
	return p.IsValid() &&
				(state == lldb::eStateAttaching ||
				 state == lldb::eStateLaunching ||
				 state == lldb::eStateRunning   ||
				 state == lldb::eStateStepping  ||
				 state == lldb::eStateStopped   ||
				 state == lldb::eStateCrashed   ||
				 state == lldb::eStateSuspended);
}

/******************************************************************************
 ProgramIsRunning

 *****************************************************************************/

bool
LLDBLink::ProgramIsRunning()
	const
{
	lldb::SBProcess p     = itsDebugger->GetSelectedTarget().GetProcess();
	lldb::StateType state = p.GetState();
	return p.IsValid() &&
				(state == lldb::eStateAttaching ||
				 state == lldb::eStateLaunching ||
				 state == lldb::eStateRunning   ||
				 state == lldb::eStateStepping);
}

/******************************************************************************
 ProgramIsStopped

 *****************************************************************************/

bool
LLDBLink::ProgramIsStopped()
	const
{
	lldb::SBProcess p     = itsDebugger->GetSelectedTarget().GetProcess();
	lldb::StateType state = p.GetState();
	return p.IsValid() &&
				(state == lldb::eStateStopped   ||
				 state == lldb::eStateCrashed   ||
				 state == lldb::eStateSuspended);
}

/******************************************************************************
 OKToSendMultipleCommands

 *****************************************************************************/

bool
LLDBLink::OKToSendMultipleCommands()
	const
{
	return CMLink::OKToSendMultipleCommands();
}

/******************************************************************************
 OKToSendCommands

 *****************************************************************************/

bool
LLDBLink::OKToSendCommands
	(
	const bool background
	)
	const
{
	return !ProgramIsRunning();
}

/******************************************************************************
 IsDefiningScript

 *****************************************************************************/

bool
LLDBLink::IsDefiningScript()
	const
{
	return false;
}

/******************************************************************************
 HandleLLDBEvent

 *****************************************************************************/

void
LLDBLink::HandleLLDBEvent()
{
	// read from LLDB

	JUtf8Byte buf[1024];
	JSize count = fread(buf, sizeof(JUtf8Byte), 1023, itsDebugger->GetOutputFileHandle());
	if (count > 0)
		{
		buf[ count ] = '\0';
		Broadcast(UserOutput(JString(buf, JString::kNoCopy), false));
		}

	count = fread(buf, sizeof(JUtf8Byte), 1023, itsDebugger->GetErrorFileHandle());
	if (count > 0)
		{
		buf[ count ] = '\0';
		Broadcast(UserOutput(JString(buf, JString::kNoCopy), true));
		}

	// read from process

	lldb::SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	if (p.IsValid())
		{
		count = p.GetSTDOUT(buf, 1023);
		if (count > 0)
			{
			buf[ count ]  = '\0';
			JUtf8Byte* b = buf;

			const JSize len = itsLastProgramInput.GetByteCount();
			if (len < count &&
				JString::CompareMaxNBytes(itsLastProgramInput.GetBytes(), buf, JMin(count, len)) == 0 &&
				(buf[len] == '\n' || buf[len] == '\r'))
				{
				b += len;
				while (*b == '\n' || *b == '\r')
					{
					b++;
					}
				}
			itsLastProgramInput.Clear();

			Broadcast(UserOutput(JString(b, JString::kNoCopy), false, true));
			}

		count = p.GetSTDERR(buf, 1023);
		if (count > 0)
			{
			buf[ count ] = '\0';
			Broadcast(UserOutput(JString(buf, JString::kNoCopy), true, true));
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

	const JUtf8Byte* eventClass = e.GetBroadcasterClass();
	const uint32_t eventType    = e.GetType();

	JString msg(eventClass);
	msg += ":";
	msg += JString((JUInt64) eventType);

	if (lldb::SBProcess::EventIsProcessEvent(e))
		{
		const lldb::StateType state = lldb::SBProcess::GetStateFromEvent(e);
		msg += "; process state: " + JString((JUInt64) state) + ", " + JString((JUInt64) lldb::SBProcess::GetRestartedFromEvent(e));

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
		msg += "; bkpt event type: " + JString((JUInt64) lldb::SBBreakpoint::GetBreakpointEventTypeFromEvent(e));
		Broadcast(BreakpointsChanged());
		}
	else if (lldb::SBWatchpoint::EventIsWatchpointEvent(e))
		{
		msg += "; watch event type: " + JString((JUInt64) lldb::SBWatchpoint::GetWatchpointEventTypeFromEvent(e));
		Broadcast(BreakpointsChanged());
		}
	else if (lldb::SBCommandInterpreter::EventIsCommandInterpreterEvent(e))
		{
		if (eventType & lldb::SBCommandInterpreter::eBroadcastBitQuitCommandReceived)
			{
			JXGetApplication()->Quit();
			}
		}

	Broadcast(DebugOutput(msg, kLogType));
}

/******************************************************************************
 ReceiveLLDBMessageLine (private static)

 *****************************************************************************/

j_lldb_cookie_fn_return
LLDBLink::ReceiveLLDBMessageLine
	(
	void*				baton,
	const char*			line,
	j_lldb_cookie_size	count
	)
{
	const JString msg(line, count);
	static_cast<LLDBLink*>(baton)->Broadcast(UserOutput(msg, false));
	return count;
}

/******************************************************************************
 ReceiveLLDBErrorLine (private static)

 *****************************************************************************/

j_lldb_cookie_fn_return
LLDBLink::ReceiveLLDBErrorLine
	(
	void*				baton,
	const char*			line,
	j_lldb_cookie_size	count
	)
{
	const JString msg(line, count);
	static_cast<LLDBLink*>(baton)->Broadcast(UserOutput(msg, true));
	return count;
}

/******************************************************************************
 LogLLDBMessage (private static)

 *****************************************************************************/

void
LLDBLink::LogLLDBMessage
	(
	const JUtf8Byte*	msg,
	void*				baton
	)
{
	static_cast<LLDBLink*>(baton)->Broadcast(DebugOutput(JString(msg, JString::kNoCopy), kLogType));
}

/******************************************************************************
 SetProgram

 *****************************************************************************/

void
LLDBLink::SetProgram
	(
	const JString& fullName
	)
{
	DetachOrKill(true);

	lldb::SBTarget t = itsDebugger->CreateTarget(fullName.GetBytes());
	if (t.IsValid())
		{
		StartListeningForEvents(t.GetBroadcaster(), kLLDBEventMask);

		auto* task = jnew LLDBSymbolsLoadedTask(fullName);
		assert( task != nullptr );
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
	const JString& fullName
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);
	Broadcast(PrepareToLoadSymbols());
	Broadcast(CMLink::SymbolsLoaded(true, name));
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
	const JString& fullName
	)
{
	DetachOrKill(false);

	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		t.LoadCore(fullName.GetBytes());
		itsCoreName = fullName;
		Broadcast(CoreLoaded());
		}
	else
		{
		JString cmdStr("target create --core ");
		cmdStr += fullName;
		SendRaw(cmdStr);
		SendRaw(JString("frame select 1", JString::kNoCopy));
		SendRaw(JString("frame select 0", JString::kNoCopy));
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
	DetachOrKill(true);

	lldb::SBTarget t = itsDebugger->CreateTarget("");
	if (t.IsValid())
		{
		lldb::SBAttachInfo info(pid);
		info.SetListener(*this);

		lldb::SBError e;
		t.Attach(info, e);

		if (e.Fail())
			{
			Broadcast(UserOutput(JString(e.GetCString(), JString::kNoCopy), true));
			}
		else if (t.IsValid())
			{
			itsIsAttachedFlag = true;
			StartListeningForEvents(t.GetBroadcaster(), kLLDBEventMask);

			lldb::SBFileSpec f     = t.GetExecutable();
			const JString fullName = JCombinePathAndName(
				JString(f.GetDirectory(), JString::kNoCopy),
				JString(f.GetFilename(), JString::kNoCopy));

			auto* task = jnew LLDBSymbolsLoadedTask(fullName);
			assert( task != nullptr );
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
	const JString& args
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		DetachOrKill(false);

		JUtf8Byte** lldbArgs = nullptr;

		JPtrArray<JString> argList(JPtrArrayT::kDeleteAll);
		if (!args.IsEmpty())
			{
			JParseArgsForExec(args, &argList);

			lldbArgs = jnew JUtf8Byte*[ argList.GetElementCount()+1 ];
			assert( lldbArgs != nullptr );

			for (JIndex i=1; i<=argList.GetElementCount(); i++)
				{
				lldbArgs[ i-1 ] = const_cast<JUtf8Byte*>(argList.GetElement(i)->GetBytes());
				}

			lldbArgs[ argList.GetElementCount() ] = nullptr;
			}

		lldb::SBError error;
		t.Launch(*this, (const char**) lldbArgs, (const char**) environ,
				 nullptr, nullptr, nullptr, ".", 0, false, error);

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
	SendRaw(JString("breakpoint list", JString::kNoCopy));
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
LLDBLink::SetBreakpoint
	(
	const JString&	fileName,
	const JIndex	lineIndex,
	const bool	temporary
	)
{
	JString path, name;
	JSplitPathAndName(fileName, &path, &name);

	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		lldb::SBBreakpoint b = t.BreakpointCreateByLocation(name.GetBytes(), lineIndex);
		b.SetOneShot(temporary);
		}
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
LLDBLink::SetBreakpoint
	(
	const JString&	address,
	const bool	temporary
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		JUtf8Byte* end;
		const lldb::addr_t a = strtoull(address.GetBytes(), &end, 0);
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
	const JString&	fileName,
	const JIndex	lineIndex
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
		for (JUnsignedOffset j=0; j<locCount; j++)
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
	const JString& addrStr
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (!t.IsValid())
		{
		return;
		}

	JUtf8Byte* end;
	const lldb::addr_t addr = strtoull(addrStr.GetBytes(), &end, 0);

	const JSize bpCount = t.GetNumBreakpoints();
	for (long i=bpCount-1; i>=0; i--)
		{
		lldb::SBBreakpoint b = t.GetBreakpointAtIndex(i);

		const JSize locCount = b.GetNumLocations();
		for (JUnsignedOffset j=0; j<locCount; j++)
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
	const bool	enabled,
	const bool	once
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
	const JIndex	debuggerIndex,
	const JString&	condition
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		t.FindBreakpointByID(debuggerIndex).SetCondition(condition.GetBytes());
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
	const JString& expr
	)
{
	Watch(expr, false);
}

/******************************************************************************
 WatchLocation

 *****************************************************************************/

void
LLDBLink::WatchLocation
	(
	const JString& expr
	)
{
	Watch(expr, true);
}

/******************************************************************************
 Watch (private)

 *****************************************************************************/

void
LLDBLink::Watch
	(
	const JString&	expr,
	const bool	resolveAddress
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		lldb::SBValue v = t.EvaluateExpression(expr.GetBytes());
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
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	if (ProgramIsStopped())
		{
		SetBreakpoint(fileName, lineIndex, true);
		Continue();
		}
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
LLDBLink::SetExecutionPoint
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	if (ProgramIsStopped())
		{
		JString cmd("_regexp-jump ");
		cmd += fileName;
		cmd += ":";
		cmd += JString((JUInt64) lineIndex);
		itsDebugger->HandleCommand(cmd.GetBytes());

		ProgramStopped();
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
	const JString& addr
	)
{
	lldb::SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
		{
		JUtf8Byte* end;
		const lldb::addr_t a = strtoull(addr.GetBytes(), &end, 0);
		t.RunToAddress(a);
		}
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
LLDBLink::SetExecutionPoint
	(
	const JString& addr
	)
{
	if (ProgramIsStopped())
		{
		JString cmd("_regexp-jump *");
		cmd += addr;
		itsDebugger->HandleCommand(cmd.GetBytes());

		ProgramStopped();
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
	const JString& name,
	const JString& value
	)
{
	lldb::SBFrame f = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();
	if (f.IsValid())
		{
		JString expr = name;
		expr        += " = ";
		expr        += value;
		f.EvaluateExpression(expr.GetBytes());

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
	assert( cmd != nullptr );
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
	assert( cmd != nullptr );
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
	assert( cmd != nullptr );
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
	assert( cmd != nullptr );
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
	assert( cmd != nullptr );
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
	assert( cmd != nullptr );
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
	assert( cmd != nullptr );
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
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetFullPath

 *****************************************************************************/

CMGetFullPath*
LLDBLink::CreateGetFullPath
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	CMGetFullPath* cmd = jnew LLDBGetFullPath(fileName, lineIndex);
	assert( cmd != nullptr );
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
	assert( cmd != nullptr );
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
	assert( cmd != nullptr );
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
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarValueCommand

 *****************************************************************************/

CMVarCommand*
LLDBLink::CreateVarValueCommand
	(
	const JString& expr
	)
{
	CMVarCommand* cmd = jnew LLDBVarCommand(expr);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarContentCommand

 *****************************************************************************/

CMVarCommand*
LLDBLink::CreateVarContentCommand
	(
	const JString& expr
	)
{
	JString s("*(");
	s += expr;
	s += ")";

	CMVarCommand* cmd = jnew LLDBVarCommand(s);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarNode

 *****************************************************************************/

CMVarNode*
LLDBLink::CreateVarNode
	(
	const bool shouldUpdate		// false for Local Variables
	)
{
	CMVarNode* node = jnew LLDBVarNode(shouldUpdate);
	assert( node != nullptr );
	return node;
}

CMVarNode*
LLDBLink::CreateVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	fullName,
	const JString&	value
	)
{
	CMVarNode* node = jnew LLDBVarNode(parent, name, value);
	assert( node != nullptr );
	return node;
}

/******************************************************************************
 Build1DArrayExpression

 *****************************************************************************/

JString
LLDBLink::Build1DArrayExpression
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
LLDBLink::Build2DArrayExpression
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
LLDBLink::CreateGetMemory
	(
	CMMemoryDir* dir
	)
{
	CMGetMemory* cmd = jnew LLDBGetMemory(dir);
	assert( cmd != nullptr );
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
	assert( cmd != nullptr );
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
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 SendRaw

	Sends the given text as text to whatever is currently accepting text.

 *****************************************************************************/

void
LLDBLink::SendRaw
	(
	const JString& text
	)
{
	if (ProgramIsRunning())
		{
		lldb::SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
		p.PutSTDIN(text.GetBytes(), text.GetByteCount());
		p.PutSTDIN("\n", 1);
		itsLastProgramInput = text;
		}
	else
		{
		itsDebugger->HandleCommand(text.GetBytes());
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
	assert( task != nullptr );
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
	command->Finished(true);	// may delete object
	SetRunningCommand(nullptr);

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

bool
LLDBLink::ProgramStopped
	(
	JString* msg
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	lldb::SBFrame f  = t.GetProcess().GetSelectedThread().GetSelectedFrame();
	if (f.IsValid())
		{
		const lldb::SBLineEntry line = f.GetLineEntry();
		const lldb::SBFileSpec file  = line.GetFileSpec();
		JString fullName;
		if (file.IsValid())
			{
			fullName = JCombinePathAndName(
				JString(file.GetDirectory(), JString::kNoCopy),
				JString(file.GetFilename(), JString::kNoCopy));
			}
		CMLocation location(fullName, line.IsValid() ? line.GetLine() : 0);

		if (f.GetFunctionName() != nullptr)
			{
			location.SetFunctionName(JString(f.GetFunctionName(), JString::kNoCopy));
			}

		const lldb::SBAddress addr = f.GetPCAddress();
		if (addr.IsValid())
			{
			const JString a = JString(addr.GetLoadAddress(t), JString::kBase16);
			location.SetMemoryAddress(a);
			}
		Broadcast(CMLink::ProgramStopped(location));

		if (msg != nullptr && file.IsValid())
			{
			*msg =  "; file: " + location.GetFileName() +
					", line: " + JString((JUInt64) location.GetLineNumber()) +
					", func: " + location.GetFunctionName() +
					", addr: " + location.GetMemoryAddress();
			}
		return file.IsValid();
		}
	else
		{
		if (msg != nullptr)
			{
			msg->Clear();
			}
		return false;
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
		itsIsAttachedFlag = false;
		}

	lldb::SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	JString reasonStr;
	if (p.IsValid() && p.GetExitDescription() != nullptr)
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

	Broadcast(UserOutput(reasonStr, false));
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
	const bool destroyTarget
	)
{
	lldb::SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
		{
		lldb::SBProcess p = t.GetProcess();
		if (itsIsAttachedFlag)
			{
			p.Detach();
			itsIsAttachedFlag = false;
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

bool
LLDBLink::OKToDetachOrKill()
	const
{
	if (itsIsAttachedFlag)
		{
		return JGetUserNotification()->AskUserYes(JGetString("WarnDetachProgram::LLDBLink"));
		}
	else if (IsDebugging())
		{
		return JGetUserNotification()->AskUserYes(JGetString("WarnKillProgram::LLDBLink"));
		}
	else
		{
		return true;
		}
}

/******************************************************************************
 StartDebugger (private)

 *****************************************************************************/

bool
LLDBLink::StartDebugger
	(
	const bool restart
	)
{
	assert( itsDebugger == nullptr );

	itsDebugger = jnew lldb::SBDebugger(lldb::SBDebugger::Create(true, LogLLDBMessage, this));
	if (itsDebugger->IsValid())
		{
		StartListeningForEvents(itsDebugger->GetCommandInterpreter().GetBroadcaster(), kLLDBEventMask);
		StartListeningForEventClass(*itsDebugger, lldb::SBThread::GetBroadcasterClassName(), kLLDBEventMask);

		assert( itsStdoutStream == nullptr );
		assert( itsStderrStream == nullptr );

#ifdef _J_OSX
		itsStdoutStream = fwopen(this, ReceiveLLDBMessageLine);
		assert( itsStdoutStream != nullptr );

		itsStderrStream = fwopen(this, ReceiveLLDBErrorLine);
		assert( itsStderrStream != nullptr );
#else
		cookie_io_functions_t my_fns = { nullptr, ReceiveLLDBMessageLine, nullptr, nullptr };

		itsStdoutStream = fopencookie(this, "w", my_fns);
		assert( itsStdoutStream != nullptr );

		my_fns.write    = ReceiveLLDBErrorLine;
		itsStderrStream = fopencookie(this, "w", my_fns);
		assert( itsStderrStream != nullptr );
#endif

		setvbuf(itsStdoutStream, nullptr, _IOLBF, 0);
		itsDebugger->SetOutputFileHandle(itsStdoutStream, false);

		setvbuf(itsStderrStream, nullptr, _IOLBF, 0);
		itsDebugger->SetErrorFileHandle(itsStderrStream, false);

		const JUtf8Byte* map[] =
			{
			"debugger", itsDebugger->GetVersionString()
			};
		const JString msg = JGetString("Welcome::LLDBLink", map, sizeof(map));

		auto* task = jnew LLDBWelcomeTask(msg, restart);
		assert( task != nullptr );
		task->Go();

		return true;
		}
	else
		{
		JGetStringManager()->ReportError("UnableToStartDebugger::LLDBLink", JString::empty);
		return false;
		}
}

/******************************************************************************
 StopDebugger (private)

 *****************************************************************************/

void
LLDBLink::StopDebugger()
{
	DetachOrKill(true);

	fclose(itsStdoutStream);
	itsStdoutStream = nullptr;

	fclose(itsStderrStream);
	itsStderrStream = nullptr;

	lldb::SBDebugger::Destroy(*itsDebugger);
	jdelete itsDebugger;
	itsDebugger = nullptr;

	InitFlags();
}

/******************************************************************************
 RestartDebugger

 *****************************************************************************/

bool
LLDBLink::RestartDebugger()
{
	const bool symbolsWereLoaded = HasLoadedSymbols();

	StopDebugger();
	return StartDebugger(symbolsWereLoaded);
}

/******************************************************************************
 ChangeDebugger

 *****************************************************************************/

bool
LLDBLink::ChangeDebugger()
{
	return true;
}
