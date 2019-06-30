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

	virtual JBoolean	DebuggerHasStarted() const override;
	virtual JBoolean	HasLoadedSymbols() const override;
	virtual JBoolean	IsDebugging() const override;
	virtual JBoolean	IsDefiningScript() const override;

	virtual JBoolean	ChangeDebugger() override;
	virtual JBoolean	RestartDebugger() override;

	virtual JString		GetChooseProgramInstructions() const override;
	virtual JBoolean	HasProgram() const override;
	virtual JBoolean	GetProgram(JString* fullName) const override;
	virtual void		SetProgram(const JString& fullName) override;
	virtual void		ReloadProgram() override;
	virtual JBoolean	HasCore() const override;
	virtual JBoolean	GetCore(JString* fullName) const override;
	virtual void		SetCore(const JString& fullName) override;
	virtual void		AttachToProcess(const pid_t pid) override;

	virtual void		RunProgram(const JString& args) override;
	virtual void		StopProgram() override;
	virtual void		KillProgram() override;
	virtual JBoolean	ProgramIsRunning() const override;
	virtual JBoolean	ProgramIsStopped() const override;
	virtual JBoolean	OKToDetachOrKill() const override;

	virtual JBoolean	OKToSendCommands(const JBoolean background) const override;
	void				Send(const JUtf8Byte* text);
	void				Send(const JString& text);

	virtual CMBreakpointManager*	GetBreakpointManager() override;

	virtual void	ShowBreakpointInfo(const JIndex debuggerIndex) override;
	virtual void	SetBreakpoint(const JString& fileName, const JIndex lineIndex,
								  const JBoolean temporary = kJFalse) override;
	virtual void	SetBreakpoint(const JString& address,
								  const JBoolean temporary = kJFalse) override;
	virtual void	RemoveBreakpoint(const JIndex debuggerIndex) override;
	virtual void	RemoveAllBreakpointsOnLine(const JString& fileName,
										   const JIndex lineIndex) override;
	virtual void	RemoveAllBreakpointsAtAddress(const JString& addr) override;
	virtual void	RemoveAllBreakpoints() override;
	virtual void	SetBreakpointEnabled(const JIndex debuggerIndex, const JBoolean enabled,
									 const JBoolean once = kJFalse) override;
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

	virtual void	SetValue(const JString& name, const JString& value) override;

	virtual const JString&	GetPrompt()	const override;
	virtual const JString&	GetScriptPrompt() const override;

	JIndex	GetStackFrameIndex() const;

	const JPtrArray<JString>&	GetSourcePathList() const;

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
	virtual CMVarNode*				CreateVarNode(const JBoolean shouldUpdate = kJTrue) override;
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

	// called by XD commands

	JBoolean	GetParsedData(xmlNode** root);

	// called by XDSocket

	void	ConnectionEstablished(XDSocket* socket);
	void	ConnectionFinished(XDSocket* socket);

	// called by XDWelcomeTask

	void	BroadcastWelcome(const JString& msg, const JBoolean error);

	// called by XDSetProgramTask

	void	BroadcastProgramSet();

	// only when user types input for program being debugged

	virtual void	SendRaw(const JString& text) override;

protected:

	virtual void	SendMedicCommand(CMCommand* command) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	typedef ACE_Acceptor<XDSocket, ACE_SOCK_ACCEPTOR>	XDAcceptor;

private:

	XDAcceptor*				itsAcceptor;
	XDSocket*				itsLink;		// nullptr if not connected to debugger
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
	const JString&	msg,
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
	return JI2B(itsParsedDataRoot != nullptr);
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

/******************************************************************************
 Send

	Sends the given text as command(s) to xdebug.

 *****************************************************************************/

inline void
XDLink::Send
	(
	const JString& text
	)
{
	Send(text.GetBytes());
}

#endif
