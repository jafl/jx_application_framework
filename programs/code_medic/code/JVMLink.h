/******************************************************************************
 JVMLink.h

	Copyright © 2009 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_JVMLink
#define _H_JVMLink

#include "CMLink.h"
#include "JVMSocket.h"
#include <JProcess.h>
#include <JAliasArray.h>

#include <j_prep_ace.h>
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
	virtual JBoolean	Send(CMCommand* cmd);

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

	JSize	GetFieldIDSize() const;
	JSize	GetMethodIDSize() const;
	JSize	GetObjectIDSize() const;
	JSize	GetReferenceTypeIDSize() const;
	JSize	GetFrameIDSize() const;

	JVMThreadNode*	GetThreadRoot();
	JUInt64			GetCurrentThreadID() const;
	void			ThreadCreated(JVMThreadNode* node);
	void			ThreadDeleted(JVMThreadNode* node);
	JBoolean		FindThread(const JUInt64 id, JVMThreadNode** node) const;

	void	FlushClassList();
	void	AddClass(const JUInt64 id, const JCharacter* signature);
	void	AddMethod(const JUInt64 classID, const JUInt64 methodID, const JCharacter* name);

	JBoolean	GetClassName(const JUInt64 id, JString* name);
	JBoolean	GetClassSourceFile(const JUInt64 id, JString* fullName);
	JBoolean	GetMethodName(const JUInt64 classID, const JUInt64 methodID, JString* name);

	static JString	ClassSignatureToResourcePath(const JCharacter* signature);
	static JString	ClassSignatureToName(const JCharacter* signature);
	static JString	ClassNameToResourcePath(const JCharacter* name);

	const JPtrArray<JString>&	GetSourcePathList() const;

	void	FlushFrameList();
	void	AddFrame(const JUInt64 id, const JUInt64 classID,
					 const JUInt64 methodID, const JUInt64 codeOffset);

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

	// called by JVM commands

	void		Send(const CMCommand* command, const JIndex cmdSet, const JIndex cmd,
					 const unsigned char* data, const JSize count);
	JBoolean	GetLatestMessageFromJVM(const JVMSocket::MessageReady** msg) const;

	// called by JVMDSocket

	void	InitDebugger();
	void	ConnectionEstablished(JVMSocket* socket);
	void	ConnectionFinished(JVMSocket* socket);

	// called by JVMWelcomeTask

	void	BroadcastWelcome(const JCharacter* msg, const JBoolean error);

	// called by JVMSetProgramTask

	void	BroadcastProgramSet();

	// called by JVMGetIDSizes

	void	SetIDSizes(const JSize fieldIDSize, const JSize methodIDSize,
					   const JSize objectIDSize, const JSize refTypeIDSize,
					   const JSize frameIDSize);

	// only when user types input for program being debugged

	virtual void	SendRaw(const JCharacter* text);

protected:

	virtual void	SendMedicCommand(CMCommand* command);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

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
			delete name;
			name = NULL;
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
			delete name;
			name = NULL;

			delete path;
			path = NULL;

			const JSize count = methods->GetElementCount();
			for (JIndex i=1; i<=count; i++)
				{
				methods->GetElement(i).Clean();
				}

			delete methods;
			methods = NULL;
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
	JVMSocket*				itsDebugLink;	// NULL if not connected to JVM
	JProcess*				itsProcess;		// NULL unless we started the JVM
	ProcessLink*			itsOutputLink;	// NULL unless we started the JVM
	ProcessLink*			itsInputLink;	// NULL unless we started the JVM
	JVMBreakpointManager*	itsBPMgr;

	JString itsJVMCWD;
	JString	itsJVMCmd;
	JString	itsJVMExecArgs;
	JString	itsJVMProcessArgs;

	JBoolean	itsInitFinishedFlag;		// debugger has been fully initialized
	JString		itsProgramConfigFileName;	// .medic config file
	JString		itsMainClassName;			// can be empty
	JBoolean	itsProgramIsStoppedFlag;	// the JVM is stopped

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

	JBoolean	StartDebugger();
	void		InitFlags();
	void		SetProcessConnection(const int toFD, const int fromFD);
	void		ReceiveMessageFromJVM(const JVMSocket::MessageReady& info);
	void		DispatchEventsFromJVM(const unsigned char* data, const JSize length);
	void		ReadFromProcess();
	void		StopDebugger();
	void		DeleteAcceptor();
	void		DeleteDebugLink();
	void		DeleteProcessLink();

	void	DetachOrKill();

	void	ProgramFinished1(const JProcess::Finished* info);

	void	CheckNextThreadGroup();

	JBoolean	ClassSignatureToFile(const JCharacter* signature, JString* fullName) const;
	JBoolean	FindSourceForClass(const JCharacter* signature, JString* fullName);
	JBoolean	GetFrame(const JUInt64 id, JIndex* index) const;

	static JOrderedSetT::CompareResult	CompareClassIDs(const ClassInfo& c1, const ClassInfo& c2);
	static JOrderedSetT::CompareResult	CompareClassNames(const ClassInfo& c1, const ClassInfo& c2);
	static JOrderedSetT::CompareResult	CompareMethodIDs(const MethodInfo& m1, const MethodInfo& m2);

	// not allowed

	JVMLink(const JVMLink& source);
	const JVMLink& operator=(const JVMLink& source);

public:

	// JBroadcaster messages

	static const JCharacter* kIDResolved;

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
JVMLink::GetSourcePathList()
	const
{
	return *itsSourcePathList;
}

/******************************************************************************
 GetLatestMessageFromJVM

 *****************************************************************************/

inline JBoolean
JVMLink::GetLatestMessageFromJVM
	(
	const JVMSocket::MessageReady** msg
	)
	const
{
	*msg = itsLatestMsg;
	return JI2B(itsLatestMsg != NULL);
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
