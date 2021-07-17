/******************************************************************************
 LLDBLink.h

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#ifndef _H_LLDBLink
#define _H_LLDBLink

// conflict between X11 and LLDB
#undef Status

#include "CMLink.h"
#include "lldb/API/SBDebugger.h"
#include "lldb/API/SBListener.h"
#include <stdio.h>

class LLDBBreakpointManager;
class LLDBGetStopLocationForLink;
class LLDBGetStopLocationForAsm;

#ifdef _J_OSX
typedef int j_lldb_cookie_fn_return;
typedef int j_lldb_cookie_size;
#else
typedef long j_lldb_cookie_fn_return;
typedef unsigned long j_lldb_cookie_size;
#endif

class LLDBLink : public CMLink, public lldb::SBListener
{
public:

	LLDBLink();

	virtual	~LLDBLink();

	virtual bool	DebuggerHasStarted() const override;
	virtual bool	HasLoadedSymbols() const override;
	virtual bool	IsDebugging() const override;
	virtual bool	IsDefiningScript() const override;

	virtual bool	ChangeDebugger() override;
	virtual bool	RestartDebugger() override;

	virtual JString		GetChooseProgramInstructions() const override;
	virtual bool	HasProgram() const override;
	virtual bool	GetProgram(JString* fullName) const override;
	virtual void		SetProgram(const JString& fullName) override;
	virtual void		ReloadProgram() override;
	virtual bool	HasCore() const override;
	virtual bool	GetCore(JString* fullName) const override;
	virtual void		SetCore(const JString& fullName) override;
	virtual void		AttachToProcess(const pid_t pid) override;

	virtual void		RunProgram(const JString& args) override;
	virtual void		StopProgram() override;
	virtual void		KillProgram() override;
	virtual bool	ProgramIsRunning() const override;
	virtual bool	ProgramIsStopped() const override;
	virtual bool	OKToDetachOrKill() const override;

	virtual bool	OKToSendMultipleCommands() const override;
	virtual bool	OKToSendCommands(const bool background) const override;

	virtual CMBreakpointManager*	GetBreakpointManager() override;

	virtual void	ShowBreakpointInfo(const JIndex debuggerIndex) override;
	virtual void	SetBreakpoint(const JString& fileName, const JIndex lineIndex,
								  const bool temporary = false) override;
	virtual void	SetBreakpoint(const JString& address,
								  const bool temporary = false) override;
	virtual void	RemoveBreakpoint(const JIndex debuggerIndex) override;
	virtual void	RemoveAllBreakpointsOnLine(const JString& fileName,
										   const JIndex lineIndex) override;
	virtual void	RemoveAllBreakpointsAtAddress(const JString& addr) override;
	virtual void	RemoveAllBreakpoints() override;
	virtual void	SetBreakpointEnabled(const JIndex debuggerIndex, const bool enabled,
									 const bool once = false) override;
	virtual void	SetBreakpointCondition(const JIndex debuggerIndex,
									   const JString& condition) override;
	virtual void	RemoveBreakpointCondition(const JIndex debuggerIndex) override;
	virtual void	SetBreakpointIgnoreCount(const JIndex debuggerIndex, const JSize count) override;

	virtual void	WatchExpression(const JString& expr) override;
	virtual void	WatchLocation(const JString& expr) override;

	virtual void	SwitchToThread(const JUInt64 id) override;
	virtual void	SwitchToFrame(const JUInt64 id) override;
	virtual void	StepOver() override;
	virtual void	StepInto() override;
	virtual void	StepOut() override;
	virtual void	Continue() override;
	virtual void	RunUntil(const JString& fileName, const JIndex lineIndex) override;
	virtual void	SetExecutionPoint(const JString& fileName, const JIndex lineIndex) override;
	virtual void	StepOverAssembly() override;
	virtual void	StepIntoAssembly() override;
	virtual void	RunUntil(const JString& addr) override;
	virtual void	SetExecutionPoint(const JString& addr) override;
	virtual void	BackupOver() override;
	virtual void	BackupInto() override;
	virtual void	BackupOut() override;
	virtual void	BackupContinue() override;

	virtual void	SetValue(const JString& name, const JString& value) override;

	virtual const JString&	GetPrompt()	const override;
	virtual const JString&	GetScriptPrompt() const override;

	// CMCommand factory

	virtual CMArray2DCommand*		CreateArray2DCommand(CMArray2DDir* dir,
														 JXStringTable* table,
														 JStringTableData* data) override;
	virtual CMPlot2DCommand*		CreatePlot2DCommand(CMPlot2DDir* dir,
														JArray<JFloat>* x,
														JArray<JFloat>* y) override;
	virtual CMDisplaySourceForMain*	CreateDisplaySourceForMain(CMSourceDirector* sourceDir) override;
	virtual CMGetCompletions*		CreateGetCompletions(CMCommandInput* input,
														 CMCommandOutputDisplay* history) override;
	virtual CMGetFrame*				CreateGetFrame(CMStackWidget* widget) override;
	virtual CMGetStack*				CreateGetStack(JTree* tree, CMStackWidget* widget) override;
	virtual CMGetThread*			CreateGetThread(CMThreadsWidget* widget) override;
	virtual CMGetThreads*			CreateGetThreads(JTree* tree, CMThreadsWidget* widget) override;
	virtual CMGetFullPath*			CreateGetFullPath(const JString& fileName,
													  const JIndex lineIndex = 0) override;
	virtual CMGetInitArgs*			CreateGetInitArgs(JXInputField* argInput) override;
	virtual CMGetLocalVars*			CreateGetLocalVars(CMVarNode* rootNode) override;
	virtual CMGetSourceFileList*	CreateGetSourceFileList(CMFileListDir* fileList) override;
	virtual CMVarCommand*			CreateVarValueCommand(const JString& expr) override;
	virtual CMVarCommand*			CreateVarContentCommand(const JString& expr) override;
	virtual CMVarNode*				CreateVarNode(const bool shouldUpdate = true) override;
	virtual CMVarNode*				CreateVarNode(JTreeNode* parent, const JString& name,
												  const JString& fullName, const JString& value) override;
	virtual JString					Build1DArrayExpression(const JString& expr,
														   const JInteger index) override;
	virtual JString					Build2DArrayExpression(const JString& expr,
														   const JInteger rowIndex,
														   const JInteger colIndex) override;
	virtual CMGetMemory*			CreateGetMemory(CMMemoryDir* dir) override;
	virtual CMGetAssembly*			CreateGetAssembly(CMSourceDirector* dir) override;
	virtual CMGetRegisters*			CreateGetRegisters(CMRegistersDir* dir) override;

	// called by commands

	lldb::SBDebugger*	GetDebugger();

	// called by LLDBWelcomeTask

	void	BroadcastWelcome(const JString& msg, const bool restart);

	// called by LLDBRunBackgroundCommandTask

	void	SendMedicCommandSync(CMCommand* command);

	// called by LLDBSymbolsLoadedTask

	void	SymbolsLoaded(const JString& fullName);

	// called by CMApp

	void	HandleLLDBEvent();

	// mostly when user types input for program being debugged

	virtual void	SendRaw(const JString& text) override;

protected:

	virtual void	SendMedicCommand(CMCommand* command) override;

private:

	lldb::SBDebugger*	itsDebugger;
	FILE*				itsStdoutStream;
	FILE*				itsStderrStream;
	JString				itsLastProgramInput;	// to avoid printing echo

	LLDBBreakpointManager*	itsBPMgr;

	JString		itsPrompt;			// to allow GetPrompt() to return JString&
	JString		itsCoreName;
	bool	itsIsAttachedFlag;	// debugging pre-existing process

private:

	bool	StartDebugger(const bool restart);
	void		InitFlags();
	void		StopDebugger();

	void		ProgramStarted(const pid_t pid);
	bool	ProgramStopped(JString* msg = nullptr);
	void		ProgramFinished1();

	void	DetachOrKill(const bool destroyTarget);

	void	Watch(const JString& expr, const bool resolveAddress);

	void	HandleLLDBEvent(const lldb::SBEvent& e);

	static j_lldb_cookie_fn_return	ReceiveLLDBMessageLine(void* baton, const char* line, j_lldb_cookie_size count);
	static j_lldb_cookie_fn_return	ReceiveLLDBErrorLine(void* baton, const char* line, j_lldb_cookie_size count);
	static void						LogLLDBMessage(const JUtf8Byte* msg, void* baton);

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
	const JString&	msg,
	const bool	restart
	)
{
	if (restart)
		{
		Broadcast(DebuggerRestarted());
		}
	Broadcast(DebuggerStarted());
	Broadcast(DebuggerReadyForInput());

	Broadcast(UserOutput(msg, false, false));
}

#endif
