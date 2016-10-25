/******************************************************************************
 LLDBLink.h

	Copyright (C) 2016 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_LLDBLink
#define _H_LLDBLink

#include "CMLink.h"
#include "lldb/API/SBDebugger.h"
#include "lldb/API/SBListener.h"
#include <stdio.h>

class LLDBBreakpointManager;
class LLDBGetStopLocationForLink;
class LLDBGetStopLocationForAsm;

class LLDBLink : public CMLink, public lldb::SBListener
{
public:

	LLDBLink();

	virtual	~LLDBLink();

	virtual JBoolean	DebuggerHasStarted() const;
	virtual JBoolean	HasLoadedSymbols() const;
	virtual JBoolean	IsDebugging() const;
	virtual JBoolean	IsDefiningScript() const;

	virtual JBoolean	ChangeDebugger();
	virtual JBoolean	RestartDebugger();

	virtual JString		GetChooseProgramInstructions() const;
	virtual JBoolean	HasProgram() const;
	virtual JBoolean	GetProgram(JString* fullName) const;
	virtual void		SetProgram(const JCharacter* fullName);
	virtual void		ReloadProgram();
	virtual JBoolean	HasCore() const;
	virtual JBoolean	GetCore(JString* fullName) const;
	virtual void		SetCore(const JCharacter* fullName);
	virtual void		AttachToProcess(const pid_t pid);

	virtual void		RunProgram(const JCharacter* args);
	virtual void		StopProgram();
	virtual void		KillProgram();
	virtual JBoolean	ProgramIsRunning() const;
	virtual JBoolean	ProgramIsStopped() const;
	virtual JBoolean	OKToDetachOrKill() const;

	virtual JBoolean	OKToSendMultipleCommands() const;
	virtual JBoolean	OKToSendCommands(const JBoolean background) const;

	virtual CMBreakpointManager*	GetBreakpointManager();

	virtual void	ShowBreakpointInfo(const JIndex debuggerIndex);
	virtual void	SetBreakpoint(const JCharacter* fileName, const JIndex lineIndex,
								  const JBoolean temporary = kJFalse);
	virtual void	SetBreakpoint(const JCharacter* address,
								  const JBoolean temporary = kJFalse);
	virtual void	RemoveBreakpoint(const JIndex debuggerIndex);
	virtual void	RemoveAllBreakpointsOnLine(const JCharacter* fileName,
										   const JIndex lineIndex);
	virtual void	RemoveAllBreakpointsAtAddress(const JCharacter* addr);
	virtual void	RemoveAllBreakpoints();
	virtual void	SetBreakpointEnabled(const JIndex debuggerIndex, const JBoolean enabled,
									 const JBoolean once = kJFalse);
	virtual void	SetBreakpointCondition(const JIndex debuggerIndex,
									   const JCharacter* condition);
	virtual void	RemoveBreakpointCondition(const JIndex debuggerIndex);
	virtual void	SetBreakpointIgnoreCount(const JIndex debuggerIndex, const JSize count);

	virtual void	WatchExpression(const JCharacter* expr);
	virtual void	WatchLocation(const JCharacter* expr);

	virtual void	SwitchToThread(const JUInt64 id);
	virtual void	SwitchToFrame(const JUInt64 id);
	virtual void	StepOver();
	virtual void	StepInto();
	virtual void	StepOut();
	virtual void	Continue();
	virtual void	RunUntil(const JCharacter* fileName, const JIndex lineIndex);
	virtual void	SetExecutionPoint(const JCharacter* fileName, const JIndex lineIndex);
	virtual void	StepOverAssembly();
	virtual void	StepIntoAssembly();
	virtual void	RunUntil(const JCharacter* addr);
	virtual void	SetExecutionPoint(const JCharacter* addr);
	virtual void	BackupOver();
	virtual void	BackupInto();
	virtual void	BackupOut();
	virtual void	BackupContinue();

	virtual void	SetValue(const JCharacter* name, const JCharacter* value);

	virtual const JString&	GetPrompt()	const;
	virtual const JString&	GetScriptPrompt() const;

	// CMCommand factory

	virtual CMArray2DCommand*		CreateArray2DCommand(CMArray2DDir* dir,
														 JXStringTable* table,
														 JStringTableData* data);
	virtual CMPlot2DCommand*		CreatePlot2DCommand(CMPlot2DDir* dir,
														JArray<JFloat>* x,
														JArray<JFloat>* y);
	virtual CMDisplaySourceForMain*	CreateDisplaySourceForMain(CMSourceDirector* sourceDir);
	virtual CMGetCompletions*		CreateGetCompletions(CMCommandInput* input,
														 CMCommandOutputDisplay* history);
	virtual CMGetFrame*				CreateGetFrame(CMStackWidget* widget);
	virtual CMGetStack*				CreateGetStack(JTree* tree, CMStackWidget* widget);
	virtual CMGetThread*			CreateGetThread(CMThreadsWidget* widget);
	virtual CMGetThreads*			CreateGetThreads(JTree* tree, CMThreadsWidget* widget);
	virtual CMGetFullPath*			CreateGetFullPath(const JCharacter* fileName,
													  const JIndex lineIndex = 0);
	virtual CMGetInitArgs*			CreateGetInitArgs(JXInputField* argInput);
	virtual CMGetLocalVars*			CreateGetLocalVars(CMVarNode* rootNode);
	virtual CMGetSourceFileList*	CreateGetSourceFileList(CMFileListDir* fileList);
	virtual CMVarCommand*			CreateVarValueCommand(const JCharacter* expr);
	virtual CMVarCommand*			CreateVarContentCommand(const JCharacter* expr);
	virtual CMVarNode*				CreateVarNode(const JBoolean shouldUpdate = kJTrue);
	virtual CMVarNode*				CreateVarNode(JTreeNode* parent, const JCharacter* name,
												  const JCharacter* fullName, const JCharacter* value);
	virtual JString					Build1DArrayExpression(const JCharacter* expr,
														   const JInteger index);
	virtual JString					Build2DArrayExpression(const JCharacter* expr,
														   const JInteger rowIndex,
														   const JInteger colIndex);
	virtual CMGetMemory*			CreateGetMemory(CMMemoryDir* dir);
	virtual CMGetAssembly*			CreateGetAssembly(CMSourceDirector* dir);
	virtual CMGetRegisters*			CreateGetRegisters(CMRegistersDir* dir);

	// called by commands

	lldb::SBDebugger*	GetDebugger();

	// called by LLDBWelcomeTask

	void	BroadcastWelcome(const JCharacter* msg, const JBoolean restart);

	// called by LLDBRunBackgroundCommandTask

	void	SendMedicCommandSync(CMCommand* command);

	// called by LLDBSymbolsLoadedTask

	void	SymbolsLoaded(const JCharacter* fullName);

	// called by CMApp

	void	HandleLLDBEvent();

	// only when user types input for program being debugged

	virtual void	SendRaw(const JCharacter* text);

protected:

	virtual void	SendMedicCommand(CMCommand* command);

private:

	lldb::SBDebugger*	itsDebugger;
	FILE*				itsStdoutStream;
	FILE*				itsStderrStream;
	JString				itsLastProgramInput;	// to avoid printing echo

	LLDBBreakpointManager*	itsBPMgr;

	JString		itsPrompt;			// to allow GetPrompt() to return JString&
	JString		itsCoreName;
	JBoolean	itsIsAttachedFlag;	// debugging pre-existing process

private:

	JBoolean	StartDebugger(const JBoolean restart);
	void		InitFlags();
	void		StopDebugger();

	void		ProgramStarted(const pid_t pid);
	JBoolean	ProgramStopped(JString* msg = NULL);
	void		ProgramFinished1();

	void	DetachOrKill(const JBoolean destroyTarget);

	void	Watch(const JCharacter* expr, const JBoolean resolveAddress);

	void	HandleLLDBEvent(const lldb::SBEvent& e);

	static int	ReceiveLLDBMessageLine(void* baton, const char* line, int count);
	static int	ReceiveLLDBErrorLine(void* baton, const char* line, int count);
	static void	LogLLDBMessage(const JCharacter* msg, void* baton);

	// not allowed

	LLDBLink(const LLDBLink& source);
	const LLDBLink& operator=(const LLDBLink& source);
};


/******************************************************************************
 GetDebugger

 *****************************************************************************/

inline lldb::SBDebugger*
LLDBLink::GetDebugger()
{
	return itsDebugger;
}

/******************************************************************************
 BroadcastWelcome

 *****************************************************************************/

inline void
LLDBLink::BroadcastWelcome
	(
	const JCharacter*	msg,
	const JBoolean		restart
	)
{
	if (restart)
		{
		Broadcast(DebuggerRestarted());
		}
	Broadcast(DebuggerStarted());
	Broadcast(DebuggerReadyForInput());

	Broadcast(UserOutput(msg, kJFalse, kJFalse));
}

#endif
