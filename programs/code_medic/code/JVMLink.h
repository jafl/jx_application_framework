/******************************************************************************
 JVMLink.h

	Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#ifndef _H_JVMLink
#define _H_JVMLink

#include "CMLink.h"
#include "JVMSocket.h"
#include <JProcess.h>
#include <JAliasArray.h>

#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/INET_Addr.h>
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include "CMPipe.h"

class JProcess;
class JXTimerTask;
class JVMSocket;
class JVMBreakpointManager;
class JVMThreadNode;

class JVMLink : public CMLink
{
public:

	enum
	{
		kVirtualMachineCmdSet = 1,
		kReferenceTypeCmdSet,
		kClassTypeCmdSet,
		kArrayTypeCmdSet,
		kInterfaceTypeCmdSet,
		kMethodCmdSet,
		kFieldCmdSet = 8,
		kObjectReferenceCmdSet,
		kStringReferenceCmdSet,
		kThreadReferenceCmdSet,
		kThreadGroupReferenceCmdSet,
		kArrayReferenceCmdSet,
		kClassLoaderReferenceCmdSet,
		kEventRequestCmdSet,
		kStackFrameCmdSet,
		kClassObjectReferenceCmdSet,
		kEventCmdSet = 64,

		// kReferenceTypeCmdSet

		kRTSignatureCmd = 1,
		kRTClassLoaderCmd,
		kRTModifiersCmd,
		kRTFieldsCmd,
		kRTMethodsCmd,
		kRTGetValuesCmd,
		kRTSourceFileCmd,
		kRTNestedTypesCmd,
		kRTStatusCmd,
		kRTInterfacesCmd,
		kRTClassObjectCmd,

		// kVirtualMachineCmdSet

		kVMVersionCmd = 1,
		kVMClassesBySignatureCmd,
		kVMAllClassesCmd,
		kVMAllThreadsCmd,
		kVMTopLevelThreadGroupsCmd,
		kVMDisposeCmd,
		kVMIDSizesCmd,
		kVMSuspendCmd,
		kVMResumeCmd,
		kVMExitCmd,

		// kThreadReferenceCmdSet

		kTNameCmd = 1,
		kTSuspendCmd,
		kTResumeCmd,
		kTStatusCmd,
		kTThreadGroupCmd,
		kTFramesCmd,
		kTFrameCountCmd,
		kTOwnedMonitorsCmd,
		kTCurrentContendedMonitorCmd,
		kTStopCmd,
		kTInterruptCmd,
		kTSuspendCountCmd,
		kTOwnedMonitorsStackDepthInfoCmd,
		kTForceEarlyReturnCmd,

		// kThreadGroupReferenceCmdSet

		kTGNameCmd = 1,
		kTGParentCmd,
		kTGChildrenCmd,

		// kEventRequestCmdSet

		kERSetCmd = 1,
		kERClearCmd,
		kERClearAllBreakpointsCmd,

		// kEventCmdSet

		kECompositeCmd = 100,

		// class status

		kClassVerifiedFlag    = 1,
		kClassPreparedFlag    = 2,
		kClassInitializedFlag = 4,
		kClassErrorFlag       = 8,

		// event types

		kSingleStepEvent = 1,
		kBreakpointEvent,
		kFramePopEvent,
		kExceptionEvent,
		kUserDefinedEvent,
		kThreadStartEvent,
		kThreadDeathEvent,
		kClassPrepareEvent,
		kClassUnloadEvent,
//		kClassLoadEvent,
		kFieldAccessEvent = 20,
		kFieldModificationEvent,
		kExceptionCatchEvent = 30,
		kMethodEntryEvent = 40,
		kMethodExitEvent,
		kMethodExitWithReturnValueEvent,
		kMonitorContendedEnterEvent,
		kMonitorContendedEnteredEvent,
		kMonitorWaitEvent,
		kMonitorWaitedEvent,
		kVMStartEvent = 90,
		kVMDeathEvent = 99,

		// suspend policy

		kSuspendNone = 0,
		kSuspendEventThread,
		kSuspendAllThreads,

		// errors

		kVMDeadErrorCode = 112
	};

	enum ReferenceType
	{
		kClassType = 1,
		kInterfaceType,
		kArrayType
	};

public:

	JVMLink();

	virtual	~JVMLink();

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

	virtual bool	OKToSendCommands(const bool background) const override;
	virtual bool	Send(CMCommand* cmd) override;

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

	virtual void	SetValue(const JString& name, const JString& value) override;

	virtual const JString&	GetPrompt()	const override;
	virtual const JString&	GetScriptPrompt() const override;

	JSize	GetFieldIDSize() const;
	JSize	GetMethodIDSize() const;
	JSize	GetObjectIDSize() const;
	JSize	GetReferenceTypeIDSize() const;
	JSize	GetFrameIDSize() const;

	JVMThreadNode*	GetThreadRoot();
	JUInt64			GetCurrentThreadID() const;
	void			ThreadCreated(JVMThreadNode* node);
	void			ThreadDeleted(JVMThreadNode* node);
	bool		FindThread(const JUInt64 id, JVMThreadNode** node) const;

	void	FlushClassList();
	void	AddClass(const JUInt64 id, const JString& signature);
	void	AddMethod(const JUInt64 classID, const JUInt64 methodID, const JString& name);

	bool	GetClassName(const JUInt64 id, JString* name);
	bool	GetClassSourceFile(const JUInt64 id, JString* fullName);
	bool	GetMethodName(const JUInt64 classID, const JUInt64 methodID, JString* name);

	static JString	ClassSignatureToResourcePath(const JString& signature);
	static JString	ClassSignatureToName(const JString& signature);
	static JString	ClassNameToResourcePath(const JString& name);

	const JPtrArray<JString>&	GetSourcePathList() const;

	void	FlushFrameList();
	void	AddFrame(const JUInt64 id, const JUInt64 classID,
					 const JUInt64 methodID, const JUInt64 codeOffset);

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

	// called by JVM commands

	void		Send(const CMCommand* command, const JIndex cmdSet, const JIndex cmd,
					 const unsigned char* data, const JSize count);
	bool	GetLatestMessageFromJVM(const JVMSocket::MessageReady** msg) const;

	// called by JVMDSocket

	void	InitDebugger();
	void	ConnectionEstablished(JVMSocket* socket);
	void	ConnectionFinished(JVMSocket* socket);

	// called by JVMWelcomeTask

	void	BroadcastWelcome(const JString& msg, const bool error);

	// called by JVMSetProgramTask

	void	BroadcastProgramSet();

	// called by JVMGetIDSizes

	void	SetIDSizes(const JSize fieldIDSize, const JSize methodIDSize,
					   const JSize objectIDSize, const JSize refTypeIDSize,
					   const JSize frameIDSize);

	// only when user types input for program being debugged

	virtual void	SendRaw(const JString& text) override;

protected:

	virtual void	SendMedicCommand(CMCommand* command) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	typedef ACE_Acceptor<JVMSocket, ACE_SOCK_ACCEPTOR>	JVMAcceptor;
	typedef CMPipe<ACE_LSOCK_STREAM>					ProcessLink;

public:

	struct MethodInfo
	{
		JUInt64		id;
		JString*	name;

		// remember to update AddMethod()

		void Clean()
		{
			jdelete name;
			name = nullptr;
		}
	};

	struct ClassInfo
	{
		JUInt64				id;
//		ReferenceType		type;	// can't get this via kReferenceTypeCmdSet
		JString*			name;
		JString*			path;	// can be be null
		JArray<MethodInfo>*	methods;

		// remember to update AddClass()

		void Clean()
		{
			jdelete name;
			name = nullptr;

			jdelete path;
			path = nullptr;

			const JSize count = methods->GetElementCount();
			for (JIndex i=1; i<=count; i++)
				{
				methods->GetElement(i).Clean();
				}

			jdelete methods;
			methods = nullptr;
		}
	};

	struct FrameInfo
	{
		JUInt64		id;
		JUInt64		classID;
		JUInt64		methodID;
		JUInt64		codeOffset;

		// remember to update AddFrame()
	};

private:

	JVMAcceptor*			itsAcceptor;
	JVMSocket*				itsDebugLink;	// nullptr if not connected to JVM
	JProcess*				itsProcess;		// nullptr unless we started the JVM
	ProcessLink*			itsOutputLink;	// nullptr unless we started the JVM
	ProcessLink*			itsInputLink;	// nullptr unless we started the JVM
	JVMBreakpointManager*	itsBPMgr;

	JString itsJVMCWD;
	JString	itsJVMCmd;
	JString	itsJVMExecArgs;
	JString	itsJVMProcessArgs;

	bool	itsInitFinishedFlag;		// debugger has been fully initialized
	JString		itsProgramConfigFileName;	// .medic config file
	JString		itsMainClassName;			// can be empty
	bool	itsProgramIsStoppedFlag;	// the JVM is stopped

	const JVMSocket::MessageReady*	itsLatestMsg;

	JSize	itsFieldIDSize;
	JSize	itsMethodIDSize;
	JSize	itsObjectIDSize;
	JSize	itsReferenceTypeIDSize;
	JSize	itsFrameIDSize;

	JPtrArray<JString>*		itsSourcePathList;
	JArray<ClassInfo>*		itsClassByIDList;
	JAliasArray<ClassInfo>*	itsClassByNameList;

	JTree*						itsThreadTree;
	JVMThreadNode*				itsThreadRoot;
	JUInt64						itsCurrentThreadID;
	JPtrArray<JVMThreadNode>*	itsThreadList;
	JXTimerTask*				itsCullThreadGroupsTask;
	JIndex						itsCullThreadGroupIndex;

	JArray<FrameInfo>*	itsFrameList;

	JXTimerTask*	itsJVMDeathTask;

private:

	bool	StartDebugger();
	void		InitFlags();
	void		ReceiveMessageFromJVM(const JVMSocket::MessageReady& info);
	void		DispatchEventsFromJVM(const unsigned char* data, const JSize length);
	void		ReadFromProcess();
	void		StopDebugger();
	void		DeleteProcessLink();

	void	DetachOrKill();

	void	ProgramFinished1(const JProcess::Finished* info);

	void	CheckNextThreadGroup();

	bool	ClassSignatureToFile(const JString& signature, JString* fullName) const;
	bool	FindSourceForClass(const JString& signature, JString* fullName);
	bool	GetFrame(const JUInt64 id, JIndex* index) const;

	static JListT::CompareResult	CompareClassIDs(const ClassInfo& c1, const ClassInfo& c2);
	static JListT::CompareResult	CompareClassNames(const ClassInfo& c1, const ClassInfo& c2);
	static JListT::CompareResult	CompareMethodIDs(const MethodInfo& m1, const MethodInfo& m2);

	// not allowed

	JVMLink(const JVMLink& source);
	const JVMLink& operator=(const JVMLink& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kIDResolved;

	class IDResolved : public JBroadcaster::Message
		{
		public:

			IDResolved(const JUInt64 id)
				:
				JBroadcaster::Message(kIDResolved),
				itsID(id)
				{ };

			JUInt64
			GetID()
				const
			{
				return itsID;
			};

		private:

			const JUInt64 itsID;
		};
};


/******************************************************************************
 BroadcastWelcome

 *****************************************************************************/

inline void
JVMLink::BroadcastWelcome
	(
	const JString&	msg,
	const bool		error
	)
{
	Broadcast(DebuggerBusy());
	Broadcast(UserOutput(msg, error));
}

/******************************************************************************
 GetSourcePathList

 *****************************************************************************/

inline const JPtrArray<JString>&
JVMLink::GetSourcePathList()
	const
{
	return *itsSourcePathList;
}

/******************************************************************************
 GetLatestMessageFromJVM

 *****************************************************************************/

inline bool
JVMLink::GetLatestMessageFromJVM
	(
	const JVMSocket::MessageReady** msg
	)
	const
{
	*msg = itsLatestMsg;
	return itsLatestMsg != nullptr;
}

/******************************************************************************
 Get*IDSize

 *****************************************************************************/

inline JSize
JVMLink::GetFieldIDSize()
	const
{
	return itsFieldIDSize;
}

inline JSize
JVMLink::GetMethodIDSize()
	const
{
	return itsMethodIDSize;
}

inline JSize
JVMLink::GetObjectIDSize()
	const
{
	return itsObjectIDSize;
}

inline JSize
JVMLink::GetReferenceTypeIDSize()
	const
{
	return itsReferenceTypeIDSize;
}

inline JSize
JVMLink::GetFrameIDSize()
	const
{
	return itsFrameIDSize;
}

/******************************************************************************
 GetThreadRoot

 *****************************************************************************/

inline JVMThreadNode*
JVMLink::GetThreadRoot()
{
	return itsThreadRoot;
}

/******************************************************************************
 GetCurrentThreadID

 *****************************************************************************/

inline JUInt64
JVMLink::GetCurrentThreadID()
	const
{
	return itsCurrentThreadID;
}

#endif
