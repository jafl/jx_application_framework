/******************************************************************************
 GDBLink.h

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GDBLink
#define _H_GDBLink

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMLink.h"

#include <j_prep_ace.h>
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include "CMPipe.h"
#include <JStringPtrMap.h>

class JProcess;
class GDBScanner;
class GDBBreakpointManager;
class GDBGetStopLocationForLink;
class GDBGetStopLocationForAsm;
class GDBPingTask;

class GDBLink : public CMLink
{
public:

	GDBLink();

	virtual	~GDBLink();

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
	void				Send(const JCharacter* text);
	void				SendWhenStopped(const JCharacter* text);

	virtual CMBreakpointManager*	GetBreakpointManager();

	virtual void	ShowBreakpointInfo(const JIndex debuggerIndex);
	virtual void	SetBreakpoint(const JCharacter* fileName, const JIndex lineIndex,
								  const JBoolean temporary = kJFalse);
	virtual void	RemoveBreakpoint(const JIndex debuggerIndex);
	virtual void	RemoveAllBreakpointsOnLine(const JCharacter* fileName,
										   const JIndex lineIndex);
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

	// called by GDB commands

	void	SaveProgramName(const JCharacter* fileName);
	void	SaveCoreName(const JCharacter* fileName);

	static JBoolean	ParseList(std::istringstream& stream, JPtrArray<JString>* list, const JCharacter terminator = '}');
	static JBoolean	ParseMap(std::istringstream& stream, JStringPtrMap<JString>* map);
	static JBoolean	ParseMapArray(std::istringstream& stream, JPtrArray< JStringPtrMap<JString> >* list);

	// called by CMChooseProcessDialog

	void	ProgramStarted(const pid_t pid);

	// called by GDBGetStopLocation*

	void	SendProgramStopped(const CMLocation& location);
	void	SendProgramStopped2(const CMLocation& location);

	// called by GDBDisplaySourceForMain

	void	FirstBreakImpossible();

	// called by GDBPingTask

	void	SendPing();

	// only when user types input for program being debugged

	virtual void	SendRaw(const JCharacter* text);

protected:

	virtual void	SendMedicCommand(CMCommand* command);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	typedef CMPipe<ACE_LSOCK_STREAM>	ProcessLink;

private:

	JString			itsDebuggerCmd;
	JProcess*		itsDebuggerProcess;			// can be NULL
	JProcess*		itsChildProcess;			// NULL until aquired pid

	ProcessLink*	itsOutputLink;				// NULL if debugger not started
	ProcessLink*	itsInputLink;				// NULL if debugger not started

	JString			itsProgramName;				// from "info file"
	JString			itsCoreName;				// from "info file"
	JBoolean		itsHasStartedFlag;			// debugger is running
	JBoolean		itsInitFinishedFlag;		// debugger has been fully initialized
	JBoolean		itsSymbolsLoadedFlag;		// debugger has loaded symbols
	JBoolean		itsDebuggerBusyFlag;		// debugger is busy
	JBoolean		itsIgnoreNextMaybeReadyFlag;// ignore next prompt
	JBoolean		itsIsDebuggingFlag;			// a process is being debugged
	JBoolean		itsIsAttachedFlag;			// debugging pre-existing process
	JBoolean		itsProgramIsStoppedFlag;	// the process is stopped
	JBoolean		itsFirstBreakFlag;			// next program stop is to get pid
	JBoolean		itsPrintingOutputFlag;		// output is being printed to GUI
	JBoolean		itsDefiningScriptFlag;		// debugger is printing prompt ">"
	JBoolean		itsWaitingToQuitFlag;		// user stopped the debugger
	JSize			itsContinueCount;			// # of prompts with empty fg Q before "continue"
	JIndex			itsPingID;

	GDBScanner*					itsScanner;
	GDBBreakpointManager*		itsBPMgr;
	GDBGetStopLocationForLink*	itsGetStopLocation;
	GDBGetStopLocationForAsm*	itsGetStopLocation2;
	GDBPingTask*				itsPingTask;

private:

	JBoolean	StartDebugger();
	void		InitDebugger();
	void		InitFlags();
	void		StopDebugger();
	void		ReadFromDebugger();

	void	DetachOrKill();

	void	ProgramFinished1();

	void	DeleteLinks();

	void	PrivateSendProgramStopped(const CMLocation& location);

	// not allowed

	GDBLink(const GDBLink& source);
	const GDBLink& operator=(const GDBLink& source);
};

#endif
