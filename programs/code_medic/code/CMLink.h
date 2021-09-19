/******************************************************************************
 CMLink.h

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMLink
#define _H_CMLink

#include <JBroadcaster.h>
#include <JStringPtrMap.h>
#include <JPtrArray-JString.h>
#include "CMLocation.h"

class CMCommand;
class CMBreakpoint;
class CMBreakpointManager;

// for CMCommand factory
class JTree;
class JTreeNode;
class JStringTableData;
class JXDirector;
class JXStringTable;
class JXInputField;
class CMArray2DDir;
class CMPlot2DDir;
class CMMemoryDir;
class CMRegistersDir;
class CMSourceDirector;
class CMCommandInput;
class CMCommandOutputDisplay;
class CMStackWidget;
class CMThreadsWidget;
class CMVarNode;
class CMFileListDir;
class CMArray2DCommand;
class CMPlot2DCommand;
class CMDisplaySourceForMain;
class CMGetCompletions;
class CMGetFrame;
class CMGetStack;
class CMGetThread;
class CMGetThreads;
class CMGetMemory;
class CMGetAssembly;
class CMGetRegisters;
class CMGetFullPath;
class CMGetInitArgs;
class CMGetLocalVars;
class CMGetSourceFileList;
class CMVarCommand;
class CMVarNode;

class CMLink : virtual public JBroadcaster
{
public:

	enum Feature
	{
		kSetProgram,
		kSetArgs,
		kSetCore,
		kSetProcess,
		kRunProgram,
		kStopProgram,
		kSetExecutionPoint,
		kExecuteBackwards,
		kShowBreakpointInfo,
		kSetBreakpointCondition,
		kSetBreakpointIgnoreCount,
		kWatchExpression,
		kWatchLocation,
		kExamineMemory,
		kDisassembleMemory
	};

public:

	CMLink(const bool* features);

	virtual	~CMLink();

	bool	GetFeature(const Feature id) const;

	virtual bool	DebuggerHasStarted() const = 0;
	virtual bool	HasLoadedSymbols() const = 0;
	virtual bool	IsDebugging() const = 0;
	virtual bool	IsDefiningScript() const = 0;

	virtual bool	ChangeDebugger() = 0;
	virtual bool	RestartDebugger() = 0;

	virtual JString	GetChooseProgramInstructions() const = 0;
	virtual bool	HasProgram() const = 0;
	virtual bool	GetProgram(JString* fullName) const = 0;
	virtual void	SetProgram(const JString& fullName) = 0;
	virtual void	ReloadProgram() = 0;
	virtual bool	HasCore() const = 0;
	virtual bool	GetCore(JString* fullName) const = 0;
	virtual void	SetCore(const JString& fullName) = 0;
	virtual void	AttachToProcess(const pid_t pid) = 0;

	virtual void	RunProgram(const JString& args) = 0;
	virtual void	StopProgram() = 0;
	virtual void	KillProgram() = 0;
	virtual bool	ProgramIsRunning() const = 0;
	virtual bool	ProgramIsStopped() const = 0;
	virtual bool	OKToDetachOrKill() const = 0;

	virtual bool	OKToSendMultipleCommands() const;
	virtual bool	OKToSendCommands(const bool background) const = 0;
	virtual bool	Send(CMCommand* cmd);
	bool			HasPendingCommands() const;
	bool			HasForegroundCommands() const;

	virtual CMBreakpointManager*	GetBreakpointManager() = 0;

	virtual void	ShowBreakpointInfo(const JIndex debuggerIndex) = 0;
	void			SetBreakpoint(const CMBreakpoint& bp);
	void			SetBreakpoint(const CMLocation& loc, const bool temporary = false);
	virtual void	SetBreakpoint(const JString& fileName, const JIndex lineIndex,
								  const bool temporary = false) = 0;
	virtual void	SetBreakpoint(const JString& address,
								  const bool temporary = false) = 0;
	void			RemoveBreakpoint(const CMBreakpoint& bp);
	virtual void	RemoveBreakpoint(const JIndex debuggerIndex) = 0;
	virtual void	RemoveAllBreakpointsOnLine(const JString& fileName,
											   const JIndex lineIndex) = 0;
	virtual void	RemoveAllBreakpointsAtAddress(const JString& addr) = 0;
	virtual void	RemoveAllBreakpoints() = 0;
	virtual void	SetBreakpointEnabled(const JIndex debuggerIndex, const bool enabled,
										 const bool once = false) = 0;
	virtual void	SetBreakpointCondition(const JIndex debuggerIndex,
										   const JString& condition) = 0;
	virtual void	RemoveBreakpointCondition(const JIndex debuggerIndex) = 0;
	virtual void	SetBreakpointIgnoreCount(const JIndex debuggerIndex, const JSize count) = 0;

	virtual void	WatchExpression(const JString& expr) = 0;
	virtual void	WatchLocation(const JString& expr) = 0;

	virtual void	SwitchToThread(const JUInt64 id) = 0;
	virtual void	SwitchToFrame(const JUInt64 id) = 0;
	virtual void	StepOver() = 0;
	virtual void	StepInto() = 0;
	virtual void	StepOut() = 0;
	virtual void	Continue() = 0;
	virtual void	RunUntil(const JString& fileName, const JIndex lineIndex) = 0;
	virtual void	SetExecutionPoint(const JString& fileName, const JIndex lineIndex) = 0;

	// uncommon, so not pure virtual

	virtual void	StepOverAssembly();
	virtual void	StepIntoAssembly();
	virtual void	RunUntil(const JString& addr);
	virtual void	SetExecutionPoint(const JString& addr);
	virtual void	BackupOver();
	virtual void	BackupInto();
	virtual void	BackupOut();
	virtual void	BackupContinue();

	virtual void	SetValue(const JString& name, const JString& value) = 0;

	virtual const JString&	GetPrompt()	const = 0;
	virtual const JString&	GetScriptPrompt() const = 0;

	void	RememberFile(const JString& fileName, const JString& fullName);
	bool	FindFile(const JString& fileName,
					 bool* exists, JString* fullName) const;
	void	ClearFileNameMap();

	static void	NotifyUser(const JString& msg, const bool error);
	static void	Log(const JUtf8Byte* log);
	static void	Log(std::ostringstream& log);

	// CMCommand factory

	virtual CMArray2DCommand*		CreateArray2DCommand(CMArray2DDir* dir,
														 JXStringTable* table,
														 JStringTableData* data) = 0;
	virtual CMPlot2DCommand*		CreatePlot2DCommand(CMPlot2DDir* dir,
														JArray<JFloat>* x,
														JArray<JFloat>* y) = 0;
	virtual CMDisplaySourceForMain*	CreateDisplaySourceForMain(CMSourceDirector* sourceDir) = 0;
	virtual CMGetCompletions*		CreateGetCompletions(CMCommandInput* input,
														 CMCommandOutputDisplay* history) = 0;
	virtual CMGetFrame*				CreateGetFrame(CMStackWidget* widget) = 0;
	virtual CMGetStack*				CreateGetStack(JTree* tree, CMStackWidget* widget) = 0;
	virtual CMGetThread*			CreateGetThread(CMThreadsWidget* widget) = 0;
	virtual CMGetThreads*			CreateGetThreads(JTree* tree, CMThreadsWidget* widget) = 0;
	virtual CMGetFullPath*			CreateGetFullPath(const JString& fileName,
													  const JIndex lineIndex = 0) = 0;
	virtual CMGetInitArgs*			CreateGetInitArgs(JXInputField* argInput) = 0;
	virtual CMGetLocalVars*			CreateGetLocalVars(CMVarNode* rootNode) = 0;
	virtual CMGetSourceFileList*	CreateGetSourceFileList(CMFileListDir* fileList) = 0;
	virtual CMVarCommand*			CreateVarValueCommand(const JString& expr) = 0;
	virtual CMVarCommand*			CreateVarContentCommand(const JString& expr) = 0;
	virtual CMVarNode*				CreateVarNode(const bool shouldUpdate = true) = 0;
	virtual CMVarNode*				CreateVarNode(JTreeNode* parent, const JString& name,
												  const JString& fullName, const JString& value) = 0;
	virtual JString					Build1DArrayExpression(const JString& expr,
														   const JInteger index) = 0;
	virtual JString					Build2DArrayExpression(const JString& expr,
														   const JInteger rowIndex,
														   const JInteger colIndex) = 0;
	virtual CMGetMemory*			CreateGetMemory(CMMemoryDir* dir) = 0;
	virtual CMGetAssembly*			CreateGetAssembly(CMSourceDirector* dir) = 0;
	virtual CMGetRegisters*			CreateGetRegisters(CMRegistersDir* dir) = 0;

	// only when user types input for program being debugged

	virtual void	SendRaw(const JString& text) = 0;

	// called by CMCommand

	void	Cancel(CMCommand* cmd);

protected:

	JIndex	GetNextTransactionID();
	void	RunNextCommand();
	void	HandleCommandRunning(const JIndex cmdID);
	void	CancelAllCommands();
	void	CancelBackgroundCommands();
	void	DeleteOneShotCommands();

	bool	GetRunningCommand(CMCommand** cmd);
	void	SetRunningCommand(CMCommand* cmd);

	virtual void	SendMedicCommand(CMCommand* command) = 0;

	JString	Build1DArrayExpressionForCFamilyLanguage(const JString& expr,
													 const JInteger index);
	JString	Build2DArrayExpressionForCFamilyLanguage(const JString& expr,
													 const JInteger rowIndex,
													 const JInteger colIndex);

private:

	const bool*	itsFeatures;

	CMCommand*		itsRunningCommand;
	JIndex			itsLastCommandID;

	JPtrArray<CMCommand>*	itsForegroundQ;
	JPtrArray<CMCommand>*	itsBackgroundQ;

	JStringPtrMap<JString>*	itsFileNameMap;		// can contain nullptr's

private:

	void	DeleteOneShotCommands(JPtrArray<CMCommand>* list);

	// not allowed

	CMLink(const CMLink& source);
	const CMLink& operator=(const CMLink& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kUserOutput;
	static const JUtf8Byte* kDebugOutput;

	static const JUtf8Byte* kDebuggerReadyForInput;
	static const JUtf8Byte* kDebuggerBusy;
	static const JUtf8Byte* kDebuggerDefiningScript;

	static const JUtf8Byte* kDebuggerStarted;
	static const JUtf8Byte* kDebuggerRestarted;
	static const JUtf8Byte* kPrepareToLoadSymbols;
	static const JUtf8Byte* kSymbolsLoaded;
	static const JUtf8Byte* kSymbolsReloaded;
	static const JUtf8Byte* kCoreLoaded;
	static const JUtf8Byte* kCoreCleared;
	static const JUtf8Byte* kAttachedToProcess;
	static const JUtf8Byte* kDetachedFromProcess;

	static const JUtf8Byte* kProgramRunning;
	static const JUtf8Byte* kProgramFirstStop;
	static const JUtf8Byte* kProgramStopped;
	static const JUtf8Byte* kProgramStopped2;
	static const JUtf8Byte* kProgramFinished;

	static const JUtf8Byte* kBreakpointsChanged;
	static const JUtf8Byte* kFrameChanged;
	static const JUtf8Byte* kThreadChanged;
	static const JUtf8Byte* kValueChanged;

	static const JUtf8Byte* kThreadListChanged;

	static const JUtf8Byte* kPlugInMessage;

	// text from gdb

	class UserOutput : public JBroadcaster::Message
		{
		public:

			UserOutput(const JString& text, const bool error, const bool fromTarget = false)
				:
				JBroadcaster::Message(kUserOutput),
				itsText(text),
				itsFromTargetFlag(fromTarget),
				itsErrorFlag(error)
				{ };

			const JString&
			GetText() const
			{
				return itsText;
			}

			bool
			IsFromTarget() const
			{
				return itsFromTargetFlag;
			}

			bool
			IsError() const
			{
				return itsErrorFlag;
			}

		private:

			const JString	itsText;
			const bool	itsFromTargetFlag;
			const bool	itsErrorFlag;
		};

	enum DebugType
	{
		kOutputType,
		kCommandType,
		kLogType
	};

	class DebugOutput : public JBroadcaster::Message
		{
		public:

			DebugOutput(const JString& text, const DebugType type)
				:
				JBroadcaster::Message(kDebugOutput),
				itsText(text), itsType(type)
				{ };

			const JString&
			GetText() const
			{
				return itsText;
			}

			DebugType
			GetType() const
			{
				return itsType;
			}

		private:

			const JString&	itsText;
			const DebugType	itsType;
		};

	// command line state

	class DebuggerReadyForInput : public JBroadcaster::Message
		{
		public:

			DebuggerReadyForInput()
				:
				JBroadcaster::Message(kDebuggerReadyForInput)
				{ };
		};

	class DebuggerBusy : public JBroadcaster::Message
		{
		public:

			DebuggerBusy()
				:
				JBroadcaster::Message(kDebuggerBusy)
				{ };
		};

	class DebuggerDefiningScript : public JBroadcaster::Message
		{
		public:

			DebuggerDefiningScript()
				:
				JBroadcaster::Message(kDebuggerDefiningScript)
				{ };
		};

	// debugger state

	class DebuggerStarted : public JBroadcaster::Message
		{
		public:

			DebuggerStarted()
				:
				JBroadcaster::Message(kDebuggerStarted)
				{ };
		};

	class DebuggerRestarted : public JBroadcaster::Message
		{
		public:

			DebuggerRestarted()
				:
				JBroadcaster::Message(kDebuggerRestarted)
				{ };
		};

	class PrepareToLoadSymbols : public JBroadcaster::Message
		{
		public:

			PrepareToLoadSymbols()
				:
				JBroadcaster::Message(kPrepareToLoadSymbols)
				{ };
		};

	class SymbolsLoaded : public JBroadcaster::Message
		{
		public:

			SymbolsLoaded(const bool success, const JString& programName)
				:
				JBroadcaster::Message(kSymbolsLoaded),
				itsSuccessFlag(success), itsProgramName(programName)
				{ };

			bool
			Successful() const
			{
				return itsSuccessFlag;
			}

			const JString&
			GetProgramName() const
			{
				return itsProgramName;
			}

		private:

			const bool	itsSuccessFlag;
			const JString&	itsProgramName;
		};

	class SymbolsReloaded : public JBroadcaster::Message
		{
		public:

			SymbolsReloaded()
				:
				JBroadcaster::Message(kSymbolsReloaded)
				{ };
		};

	class CoreLoaded : public JBroadcaster::Message
		{
		public:

			CoreLoaded()
				:
				JBroadcaster::Message(kCoreLoaded)
				{ };
		};

	class CoreCleared : public JBroadcaster::Message
		{
		public:

			CoreCleared()
				:
				JBroadcaster::Message(kCoreCleared)
				{ };
		};

	class AttachedToProcess : public JBroadcaster::Message
		{
		public:

			AttachedToProcess()
				:
				JBroadcaster::Message(kAttachedToProcess)
				{ };
		};

	class DetachedFromProcess : public JBroadcaster::Message
		{
		public:

			DetachedFromProcess()
				:
				JBroadcaster::Message(kDetachedFromProcess)
				{ };
		};

	// program state

	class ProgramRunning : public JBroadcaster::Message
		{
		public:

			ProgramRunning()
				:
				JBroadcaster::Message(kProgramRunning)
				{ };
		};

	class ProgramFirstStop : public JBroadcaster::Message
		{
		public:

			ProgramFirstStop()
				:
				JBroadcaster::Message(kProgramFirstStop)
				{ };
		};

	class ProgramStoppedBase : public JBroadcaster::Message
		{
		public:

			ProgramStoppedBase(const JUtf8Byte* type, const CMLocation& location)
				:
				JBroadcaster::Message(type),
				itsLocation(location)
				{ };

			bool
			GetLocation
				(
				const CMLocation** loc
				)
				const
			{
				*loc = &itsLocation;
				return (itsLocation.GetFileID()).IsValid();
			};

		private:

			const CMLocation& itsLocation;
		};

	class ProgramStopped : public ProgramStoppedBase
		{
		public:

			ProgramStopped(const CMLocation& location)
				:
				ProgramStoppedBase(kProgramStopped, location)
				{ };
		};

	class ProgramStopped2 : public ProgramStoppedBase
		{
		public:

			ProgramStopped2(const CMLocation& location)
				:
				ProgramStoppedBase(kProgramStopped2, location)
				{ };
		};

	class ProgramFinished : public JBroadcaster::Message
		{
		public:

			ProgramFinished()
				:
				JBroadcaster::Message(kProgramFinished)
				{ };
		};

	// debugger state

	class BreakpointsChanged : public JBroadcaster::Message
		{
		public:

			BreakpointsChanged()
				:
				JBroadcaster::Message(kBreakpointsChanged)
				{ };
		};

	class FrameChanged : public JBroadcaster::Message
		{
		public:

			FrameChanged()
				:
				JBroadcaster::Message(kFrameChanged)
				{ };
		};

	class ThreadChanged : public JBroadcaster::Message
		{
		public:

			ThreadChanged()
				:
				JBroadcaster::Message(kThreadChanged)
				{ };
		};

	class ValueChanged : public JBroadcaster::Message
		{
		public:

			ValueChanged()
				:
				JBroadcaster::Message(kValueChanged)
				{ };
		};

	class ThreadListChanged : public JBroadcaster::Message
		{
		public:

			ThreadListChanged()
				:
				JBroadcaster::Message(kThreadListChanged)
				{ };
		};

	// plug-in messages

	class PlugInMessage : public JBroadcaster::Message
		{
		public:

			PlugInMessage(const JString& message)
				:
				JBroadcaster::Message(kPlugInMessage),
				itsMessage(message)
				{ };

			const JString&
			GetMessage() const
			{
				return itsMessage;
			}

		private:

			const JString& itsMessage;
		};
};


/******************************************************************************
 GetFeature

 *****************************************************************************/

inline bool
CMLink::GetFeature
	(
	const Feature id
	)
	const
{
	return itsFeatures[ id ];
}

/******************************************************************************
 GetRunningCommand (protected)

 *****************************************************************************/

inline bool
CMLink::GetRunningCommand
	(
	CMCommand** cmd
	)
{
	*cmd = itsRunningCommand;
	return itsRunningCommand != nullptr;
}

/******************************************************************************
 SetRunningCommand (protected)

 *****************************************************************************/

inline void
CMLink::SetRunningCommand
	(
	CMCommand* cmd
	)
{
	itsRunningCommand = cmd;
}

/******************************************************************************
 HasPendingCommands

 *****************************************************************************/

inline bool
CMLink::HasPendingCommands()
	const
{
	return !itsForegroundQ->IsEmpty();
}

/******************************************************************************
 HasForegroundCommands

 *****************************************************************************/

inline bool
CMLink::HasForegroundCommands()
	const
{
	return !itsForegroundQ->IsEmpty();
}

#endif
