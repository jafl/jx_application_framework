/******************************************************************************
 CMLink.h

	Copyright © 2007 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMLink
#define _H_CMLink

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	CMLink(const JBoolean* features);

	virtual	~CMLink();

	JBoolean	GetFeature(const Feature id) const;

	virtual JBoolean	DebuggerHasStarted() const = 0;
	virtual JBoolean	HasLoadedSymbols() const = 0;
	virtual JBoolean	IsDebugging() const = 0;
	virtual JBoolean	IsDefiningScript() const = 0;

	virtual JBoolean	ChangeDebugger() = 0;
	virtual JBoolean	RestartDebugger() = 0;

	virtual JString		GetChooseProgramInstructions() const = 0;
	virtual JBoolean	HasProgram() const = 0;
	virtual JBoolean	GetProgram(JString* fullName) const = 0;
	virtual void		SetProgram(const JCharacter* fullName) = 0;
	virtual void		ReloadProgram() = 0;
	virtual JBoolean	HasCore() const = 0;
	virtual JBoolean	GetCore(JString* fullName) const = 0;
	virtual void		SetCore(const JCharacter* fullName) = 0;
	virtual void		AttachToProcess(const pid_t pid) = 0;

	virtual void		RunProgram(const JCharacter* args) = 0;
	virtual void		StopProgram() = 0;
	virtual void		KillProgram() = 0;
	virtual JBoolean	ProgramIsRunning() const = 0;
	virtual JBoolean	ProgramIsStopped() const = 0;
	virtual JBoolean	OKToDetachOrKill() const = 0;

	virtual JBoolean	OKToSendMultipleCommands() const;
	virtual JBoolean	OKToSendCommands(const JBoolean background) const = 0;
	virtual JBoolean	Send(CMCommand* cmd);
	JBoolean			HasPendingCommands() const;
	JBoolean			HasForegroundCommands() const;

	virtual CMBreakpointManager*	GetBreakpointManager() = 0;

	virtual void	ShowBreakpointInfo(const JIndex debuggerIndex) = 0;
	void			SetBreakpoint(const CMBreakpoint& bp);
	void			SetBreakpoint(const CMLocation& loc, const JBoolean temporary = kJFalse);
	virtual void	SetBreakpoint(const JCharacter* fileName, const JIndex lineIndex,
								  const JBoolean temporary = kJFalse) = 0;
	virtual void	SetBreakpoint(const JCharacter* address,
								  const JBoolean temporary = kJFalse) = 0;
	void			RemoveBreakpoint(const CMBreakpoint& bp);
	virtual void	RemoveBreakpoint(const JIndex debuggerIndex) = 0;
	virtual void	RemoveAllBreakpointsOnLine(const JCharacter* fileName,
											   const JIndex lineIndex) = 0;
	virtual void	RemoveAllBreakpointsAtAddress(const JCharacter* addr) = 0;
	virtual void	RemoveAllBreakpoints() = 0;
	virtual void	SetBreakpointEnabled(const JIndex debuggerIndex, const JBoolean enabled,
										 const JBoolean once = kJFalse) = 0;
	virtual void	SetBreakpointCondition(const JIndex debuggerIndex,
										   const JCharacter* condition) = 0;
	virtual void	RemoveBreakpointCondition(const JIndex debuggerIndex) = 0;
	virtual void	SetBreakpointIgnoreCount(const JIndex debuggerIndex, const JSize count) = 0;

	virtual void	WatchExpression(const JCharacter* expr) = 0;
	virtual void	WatchLocation(const JCharacter* expr) = 0;

	virtual void	SwitchToThread(const JUInt64 id) = 0;
	virtual void	SwitchToFrame(const JUInt64 id) = 0;
	virtual void	StepOver() = 0;
	virtual void	StepInto() = 0;
	virtual void	StepOut() = 0;
	virtual void	Continue() = 0;
	virtual void	RunUntil(const JCharacter* fileName, const JIndex lineIndex) = 0;
	virtual void	SetExecutionPoint(const JCharacter* fileName, const JIndex lineIndex) = 0;

	// uncommon, so not pure virtual

	virtual void	StepOverAssembly();
	virtual void	StepIntoAssembly();
	virtual void	RunUntil(const JCharacter* addr);
	virtual void	SetExecutionPoint(const JCharacter* addr);
	virtual void	BackupOver();
	virtual void	BackupInto();
	virtual void	BackupOut();
	virtual void	BackupContinue();

	virtual void	SetValue(const JCharacter* name, const JCharacter* value) = 0;

	virtual const JString&	GetPrompt()	const = 0;
	virtual const JString&	GetScriptPrompt() const = 0;

	void		RememberFile(const JCharacter* fileName, const JCharacter* fullName);
	JBoolean	FindFile(const JCharacter* fileName,
						 JBoolean* exists, JString* fullName) const;
	void		ClearFileNameMap();

	static void	Log(const JCharacter* log);
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
	virtual CMGetFullPath*			CreateGetFullPath(const JCharacter* fileName,
													  const JIndex lineIndex = 0) = 0;
	virtual CMGetInitArgs*			CreateGetInitArgs(JXInputField* argInput) = 0;
	virtual CMGetLocalVars*			CreateGetLocalVars(CMVarNode* rootNode) = 0;
	virtual CMGetSourceFileList*	CreateGetSourceFileList(CMFileListDir* fileList) = 0;
	virtual CMVarCommand*			CreateVarValueCommand(const JCharacter* expr) = 0;
	virtual CMVarCommand*			CreateVarContentCommand(const JCharacter* expr) = 0;
	virtual CMVarNode*				CreateVarNode(const JBoolean shouldUpdate = kJTrue) = 0;
	virtual CMVarNode*				CreateVarNode(JTreeNode* parent, const JCharacter* name,
												  const JCharacter* fullName, const JCharacter* value) = 0;
	virtual JString					Build1DArrayExpression(const JCharacter* expr,
														   const JInteger index) = 0;
	virtual JString					Build2DArrayExpression(const JCharacter* expr,
														   const JInteger rowIndex,
														   const JInteger colIndex) = 0;
	virtual CMGetMemory*			CreateGetMemory(CMMemoryDir* dir) = 0;
	virtual CMGetAssembly*			CreateGetAssembly(CMSourceDirector* dir) = 0;
	virtual CMGetRegisters*			CreateGetRegisters(CMRegistersDir* dir) = 0;

	// only when user types input for program being debugged

	virtual void	SendRaw(const JCharacter* text) = 0;

	// called by CMCommand

	void	Cancel(CMCommand* cmd);

protected:

	JIndex	GetNextTransactionID();
	void	RunNextCommand();
	void	HandleCommandRunning(const JIndex cmdID);
	void	CancelAllCommands();
	void	CancelBackgroundCommands();
	void	DeleteOneShotCommands();

	JBoolean	GetRunningCommand(CMCommand** cmd);
	void		SetRunningCommand(CMCommand* cmd);

	virtual void	SendMedicCommand(CMCommand* command) = 0;

	JString	Build1DArrayExpressionForCFamilyLanguage(const JCharacter* expr,
													 const JInteger index);
	JString	Build2DArrayExpressionForCFamilyLanguage(const JCharacter* expr,
													 const JInteger rowIndex,
													 const JInteger colIndex);

private:

	const JBoolean*	itsFeatures;

	CMCommand*		itsRunningCommand;
	JIndex			itsLastCommandID;

	JPtrArray<CMCommand>*	itsForegroundQ;
	JPtrArray<CMCommand>*	itsBackgroundQ;

	JStringPtrMap<JString>*	itsFileNameMap;		// can contain NULL's

private:

	void	DeleteOneShotCommands(JPtrArray<CMCommand>* list);

	// not allowed

	CMLink(const CMLink& source);
	const CMLink& operator=(const CMLink& source);

public:

	// JBroadcaster messages

	static const JCharacter* kUserOutput;
	static const JCharacter* kDebugOutput;

	static const JCharacter* kDebuggerReadyForInput;
	static const JCharacter* kDebuggerBusy;
	static const JCharacter* kDebuggerDefiningScript;

	static const JCharacter* kDebuggerStarted;
	static const JCharacter* kDebuggerRestarted;
	static const JCharacter* kPrepareToLoadSymbols;
	static const JCharacter* kSymbolsLoaded;
	static const JCharacter* kSymbolsReloaded;
	static const JCharacter* kCoreLoaded;
	static const JCharacter* kCoreCleared;
	static const JCharacter* kAttachedToProcess;
	static const JCharacter* kDetachedFromProcess;

	static const JCharacter* kProgramRunning;
	static const JCharacter* kProgramFirstStop;
	static const JCharacter* kProgramStopped;
	static const JCharacter* kProgramStopped2;
	static const JCharacter* kProgramFinished;

	static const JCharacter* kBreakpointsChanged;
	static const JCharacter* kFrameChanged;
	static const JCharacter* kThreadChanged;
	static const JCharacter* kValueChanged;

	static const JCharacter* kThreadListChanged;

	static const JCharacter* kPlugInMessage;

	// text from gdb

	class UserOutput : public JBroadcaster::Message
		{
		public:

			UserOutput(const JCharacter* text, const JBoolean error, const JBoolean fromTarget = kJFalse)
				:
				JBroadcaster::Message(kUserOutput),
				itsText(text),
				itsFromTargetFlag(fromTarget),
				itsErrorFlag(error)
				{ };

			const JCharacter*
			GetText() const
			{
				return itsText;
			}

			JBoolean
			IsFromTarget() const
			{
				return itsFromTargetFlag;
			}

			JBoolean
			IsError() const
			{
				return itsErrorFlag;
			}

		private:

			const JCharacter*	itsText;
			const JBoolean		itsFromTargetFlag;
			const JBoolean		itsErrorFlag;
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

			DebugOutput(const JCharacter* text, const DebugType type)
				:
				JBroadcaster::Message(kDebugOutput),
				itsText(text), itsType(type)
				{ };

			const JCharacter*
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

			const JCharacter*	itsText;
			const DebugType		itsType;
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

			SymbolsLoaded(const JBoolean success, const JString& programName)
				:
				JBroadcaster::Message(kSymbolsLoaded),
				itsSuccessFlag(success), itsProgramName(programName)
				{ };

			JBoolean
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

			const JBoolean	itsSuccessFlag;
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

			ProgramStoppedBase(const JCharacter* type, const CMLocation& location)
				:
				JBroadcaster::Message(type),
				itsLocation(location)
				{ };

			JBoolean
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

	// debugger state changes

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

inline JBoolean
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

inline JBoolean
CMLink::GetRunningCommand
	(
	CMCommand** cmd
	)
{
	*cmd = itsRunningCommand;
	return JI2B(itsRunningCommand != NULL);
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

inline JBoolean
CMLink::HasPendingCommands()
	const
{
	return !itsForegroundQ->IsEmpty();
}

/******************************************************************************
 HasForegroundCommands

 *****************************************************************************/

inline JBoolean
CMLink::HasForegroundCommands()
	const
{
	return !itsForegroundQ->IsEmpty();
}

#endif
