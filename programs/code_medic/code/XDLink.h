/******************************************************************************
 XDLink.h

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#ifndef _H_XDLink
#define _H_XDLink

#include "CMLink.h"
#include <jXMLUtil.h>	// need defn of xmlNode

#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/INET_Addr.h>

class XDBreakpointManager;
class XDSocket;

class XDLink : public CMLink
{
public:

	XDLink();

	virtual	~XDLink();

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

	virtual JBoolean	OKToSendCommands(const JBoolean background) const;
	void				Send(const JCharacter* text);

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

	virtual void	SetValue(const JCharacter* name, const JCharacter* value);

	virtual const JString&	GetPrompt()	const;
	virtual const JString&	GetScriptPrompt() const;

	JIndex	GetStackFrameIndex() const;

	const JPtrArray<JString>&	GetSourcePathList() const;

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

	// called by XD commands

	JBoolean	GetParsedData(xmlNode** root);

	// called by XDSocket

	void	ConnectionEstablished(XDSocket* socket);
	void	ConnectionFinished(XDSocket* socket);

	// called by XDWelcomeTask

	void	BroadcastWelcome(const JCharacter* msg, const JBoolean error);

	// called by XDSetProgramTask

	void	BroadcastProgramSet();

	// only when user types input for program being debugged

	virtual void	SendRaw(const JCharacter* text);

protected:

	virtual void	SendMedicCommand(CMCommand* command);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	typedef ACE_Acceptor<XDSocket, ACE_SOCK_ACCEPTOR>	XDAcceptor;

private:

	XDAcceptor*				itsAcceptor;
	XDSocket*				itsLink;		// NULL if not connected to debugger
	XDBreakpointManager*	itsBPMgr;

	JString		itsIDEKey;
	JString		itsScriptURI;
	JIndex		itsStackFrameIndex;
	JBoolean	itsInitFinishedFlag;		// debugger has been fully initialized
	JBoolean	itsProgramIsStoppedFlag;	// the process is stopped
	JBoolean	itsDebuggerBusyFlag;		// debugger is busy
	JString		itsProgramConfigFileName;	// .medic config file
	JString		itsProgramName;				// can be empty

	xmlNode*	itsParsedDataRoot;

	JPtrArray<JString>*		itsSourcePathList;

private:

	JBoolean	StartDebugger();
	void		InitFlags();
	void		StopDebugger();
	void		ReceiveMessageFromDebugger();

	void	SendProgramStopped(const CMLocation& location);

	// not allowed

	XDLink(const XDLink& source);
	const XDLink& operator=(const XDLink& source);
};


/******************************************************************************
 BroadcastWelcome

 *****************************************************************************/

inline void
XDLink::BroadcastWelcome
	(
	const JCharacter*	msg,
	const JBoolean		error
	)
{
	Broadcast(DebuggerBusy());
	Broadcast(UserOutput(msg, error));
}

/******************************************************************************
 GetSourcePathList

 *****************************************************************************/

inline const JPtrArray<JString>&
XDLink::GetSourcePathList()
	const
{
	return *itsSourcePathList;
}

/******************************************************************************
 GetParsedData

 *****************************************************************************/

inline JBoolean
XDLink::GetParsedData
	(
	xmlNode** root
	)
{
	*root = itsParsedDataRoot;
	return JI2B(itsParsedDataRoot != NULL);
}

/******************************************************************************
 GetStackFrameIndex

 *****************************************************************************/

inline JIndex
XDLink::GetStackFrameIndex()
	const
{
	return itsStackFrameIndex;
}

#endif
