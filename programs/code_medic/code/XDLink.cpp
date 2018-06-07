/******************************************************************************
 XDLink.cpp

	Interface to Xdebug.

	BASE CLASS = CMLink

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#include "XDLink.h"
#include "XDSocket.h"
#include "XDWelcomeTask.h"
#include "XDSetProgramTask.h"
#include "XDCloseSocketTask.h"
#include "XDBreakpointManager.h"

#include "XDArray2DCommand.h"
#include "XDPlot2DCommand.h"
#include "XDDisplaySourceForMain.h"
#include "XDGetCompletions.h"
#include "XDGetFrame.h"
#include "XDGetStack.h"
#include "XDGetThread.h"
#include "XDGetThreads.h"
#include "XDGetFullPath.h"
#include "XDGetInitArgs.h"
#include "XDGetLocalVars.h"
#include "XDGetSourceFileList.h"
#include "XDVarCommand.h"
#include "XDVarNode.h"

#include "CMCommandDirector.h"
#include "CMStackDir.h"
#include "CMStackWidget.h"
#include "CMStackFrameNode.h"
#include "CMMDIServer.h"
#include "cmGlobals.h"

#include <libxml/parser.h>

#include <JXAssert.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jErrno.h>
#include <jAssert.h>

const JIndex kXdebugPort = 9000;

static const JBoolean kFeatures[]=
{
	kJTrue,		// kSetProgram
	kJFalse,	// kSetArgs
	kJFalse,	// kSetCore
	kJFalse,	// kSetProcess
	kJFalse,	// kRunProgram
	kJFalse,	// kStopProgram
	kJFalse,	// kSetExecutionPoint
	kJFalse,	// kExecuteBackwards
	kJFalse,	// kShowBreakpointInfo
	kJFalse,	// kSetBreakpointCondition
	kJFalse,	// kSetBreakpointIgnoreCount
	kJFalse,	// kWatchExpression
	kJFalse,	// kWatchLocation
	kJFalse,	// kExamineMemory
	kJFalse,	// kDisassembleMemory
};

/******************************************************************************
 Constructor

 *****************************************************************************/

XDLink::XDLink()
	:
	CMLink(kFeatures),
	itsAcceptor(nullptr),
	itsLink(nullptr),
	itsParsedDataRoot(nullptr)
{
	InitFlags();

	itsBPMgr = jnew XDBreakpointManager(this);
	assert( itsBPMgr != nullptr );

	itsSourcePathList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsSourcePathList != nullptr );

	StartDebugger();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

XDLink::~XDLink()
{
	StopDebugger();

	jdelete itsAcceptor;
	jdelete itsBPMgr;
	jdelete itsSourcePathList;

	DeleteOneShotCommands();
}

/******************************************************************************
 InitFlags (private)

 *****************************************************************************/

void
XDLink::InitFlags()
{
	itsStackFrameIndex      = (JIndex) -1;
	itsInitFinishedFlag     = kJFalse;
	itsProgramIsStoppedFlag = kJFalse;
	itsDebuggerBusyFlag     = kJTrue;
}

/******************************************************************************
 GetPrompt

 ******************************************************************************/

const JString&
XDLink::GetPrompt()
	const
{
	return JGetString("Prompt::XDLink");
}

/******************************************************************************
 GetScriptPrompt

 ******************************************************************************/

const JString&
XDLink::GetScriptPrompt()
	const
{
	return JGetString("ScriptPrompt::XDLink");
}

/******************************************************************************
 DebuggerHasStarted

 ******************************************************************************/

JBoolean
XDLink::DebuggerHasStarted()
	const
{
	return kJTrue;
}

/******************************************************************************
 GetChooseProgramInstructions

 ******************************************************************************/

JString
XDLink::GetChooseProgramInstructions()
	const
{
	return JGetString("ChooseProgramInstr::XDLink");
}

/******************************************************************************
 HasProgram

 ******************************************************************************/

JBoolean
XDLink::HasProgram()
	const
{
	return JI2B(!itsProgramName.IsEmpty() || itsLink != nullptr);
}

/******************************************************************************
 GetProgram

 ******************************************************************************/

JBoolean
XDLink::GetProgram
	(
	JString* fullName
	)
	const
{
	*fullName = (itsProgramName.IsEmpty() ? "PHP" : itsProgramName);
	return kJTrue;
}

/******************************************************************************
 HasCore

 ******************************************************************************/

JBoolean
XDLink::HasCore()
	const
{
	return kJFalse;
}

/******************************************************************************
 GetCore

 ******************************************************************************/

JBoolean
XDLink::GetCore
	(
	JString* fullName
	)
	const
{
	fullName->Clear();
	return kJFalse;
}

/******************************************************************************
 HasLoadedSymbols

 ******************************************************************************/

JBoolean
XDLink::HasLoadedSymbols()
	const
{
	return JI2B(itsLink != nullptr);
}

/******************************************************************************
 IsDebugging

 *****************************************************************************/

JBoolean
XDLink::IsDebugging()
	const
{
	return JI2B(itsLink != nullptr);
}

/******************************************************************************
 ProgramIsRunning

 *****************************************************************************/

JBoolean
XDLink::ProgramIsRunning()
	const
{
	return JI2B(itsLink != nullptr && !itsProgramIsStoppedFlag);
}

/******************************************************************************
 ProgramIsStopped

 *****************************************************************************/

JBoolean
XDLink::ProgramIsStopped()
	const
{
	return JI2B(itsLink != nullptr && itsProgramIsStoppedFlag);
}

/******************************************************************************
 OKToSendCommands

 *****************************************************************************/

JBoolean
XDLink::OKToSendCommands
	(
	const JBoolean background
	)
	const
{
	return kJTrue;
}

/******************************************************************************
 IsDefiningScript

 *****************************************************************************/

JBoolean
XDLink::IsDefiningScript()
	const
{
	return kJFalse;
}

/******************************************************************************
 Receive (virtual protected)

 *****************************************************************************/

void
XDLink::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && message.Is(JMessageProtocolT::kMessageReady))
		{
		ReceiveMessageFromDebugger();
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveMessageFromDebugger (private)

 *****************************************************************************/

void
XDLink::ReceiveMessageFromDebugger()
{
	itsLink->StopTimer();

	JString data;
	const JBoolean ok = itsLink->GetNextMessage(&data);
	assert( ok );

	if (data.IsEmpty() || data.GetFirstCharacter() != '<')
		{
		return;
		}

	Broadcast(DebugOutput(data, kOutputType));

	if (itsInitFinishedFlag)
		{
		if (!itsProgramIsStoppedFlag)
			{
			itsProgramIsStoppedFlag = kJTrue;
			Broadcast(ProgramStopped(CMLocation("", 1)));
			}

		itsDebuggerBusyFlag = kJFalse;
		Broadcast(DebuggerReadyForInput());
		}

	xmlDoc* doc = xmlReadMemory(data.GetCString(), data.GetLength(),
								nullptr, nullptr, XML_PARSE_NOCDATA);
	if (doc != nullptr)
		{
		xmlNode* root = xmlDocGetRootElement(doc);

		if (root != nullptr && strcmp((char*) root->name, "init") == 0)
			{
			itsIDEKey         = JGetXMLNodeAttr(root, "idekey");
			const JString uri = JGetXMLNodeAttr(root, "fileuri");

			const JCharacter* map[] =
				{
				"idekey", itsIDEKey,
				"uri",    uri
				};
			JString msg = JGetString("ConnectionInfo::XDLink", map, sizeof(map));
			Broadcast(UserOutput(msg, kJFalse));

			Send("feature_set -n show_hidden -v 1");
			Send("step_into");

			JString programName;
			GetProgram(&programName);

			Broadcast(AttachedToProcess());
			Broadcast(SymbolsLoaded(JI2B(uri == itsScriptURI), programName));

			itsInitFinishedFlag = kJTrue;
			itsScriptURI        = uri;
			}
		else if (root != nullptr && strcmp((char*) root->name, "response") == 0)
			{
			const JString status = JGetXMLNodeAttr(root, "status");
			const JString reason = JGetXMLNodeAttr(root, "reason");
			if (status == "break" && reason == "error" &&
				root->children != nullptr && root->children->children != nullptr &&
				strcmp((char*) root->children->name, "error") == 0 &&
				root->children->children->type == XML_TEXT_NODE)
				{
				JString msg            = (char*) root->children->children->content;
				const JString encoding = JGetXMLNodeAttr(root->children, "encoding");
				if (encoding == "base64")
					{
					msg.DecodeBase64(&msg);
					}
				msg += "\n";
				Broadcast(UserOutput(msg, kJTrue));
				}

			const JString idStr = JGetXMLNodeAttr(root, "transaction_id");
			JUInt id;
			if (idStr.ConvertToUInt(&id))
				{
				HandleCommandRunning(id);
				}

			CMCommand* cmd;
			if (GetRunningCommand(&cmd))
				{
				itsParsedDataRoot = root;

				cmd->Finished(JI2B(
					root->children == nullptr || strcmp((char*) root->children->name, "error") != 0));

				itsParsedDataRoot = nullptr;

				SetRunningCommand(nullptr);
				if (!HasForegroundCommands())
					{
					RunNextCommand();
					}
				}

			if (status == "stopping" || status == "stopped")
				{
				CancelAllCommands();

				XDCloseSocketTask* task = jnew XDCloseSocketTask(itsLink);
				assert( task != nullptr );
				task->Go();
				}
			}

		xmlFreeDoc(doc);
		}
}

/******************************************************************************
 SetProgram

 *****************************************************************************/

void
XDLink::SetProgram
	(
	const JCharacter* fileName
	)
{
	Send("detach");

//	StopDebugger();		// avoid broadcasting DebuggerRestarted
//	StartDebugger();

	itsProgramConfigFileName.Clear();
	itsSourcePathList->DeleteAll();

	JString fullName;
	if (!JConvertToAbsolutePath(fileName, nullptr, &fullName) ||
		!JFileReadable(fullName))
		{
		const JString error = JGetString("ConfigFileUnreadable::XDLink");
		Broadcast(UserOutput(error, kJTrue));
		return;
		}
	else if (CMMDIServer::IsBinary(fullName))
		{
		const JString error = JGetString("ConfigFileIsBinary::XDLink");
		Broadcast(UserOutput(error, kJTrue));
		return;
		}

	JString line;
	if (!CMMDIServer::GetLanguage(fullName, &line) ||
		JString::Compare(line, "php", kJFalse) != 0)
		{
		const JString error = JGetString("ConfigFileWrongLanguage::XDLink");
		Broadcast(UserOutput(error, kJTrue));
		return;
		}

	JString path, name, suffix;
	JSplitPathAndName(fullName, &path, &name);
	JSplitRootAndSuffix(name, &itsProgramName, &suffix);

	itsProgramConfigFileName = fullName;

	std::ifstream input(fullName);
	while (1)
		{
		line = JReadLine(input);
		line.TrimWhitespace();

		if (line.BeginsWith("source-path:"))
			{
			line.RemoveSubstring(1, 12);
			line.TrimWhitespace();

			name = JCombinePathAndName(path, line);
			itsSourcePathList->Append(name);
			}
		else if (!line.IsEmpty() && !line.BeginsWith("code-medic:"))
			{
			line.Prepend("Unknown option: ");
			line.AppendCharacter('\n');
			Broadcast(UserOutput(line, kJTrue));
			}

		if (!input.good())
			{
			break;
			}
		}

	XDSetProgramTask* task = jnew XDSetProgramTask();
	assert( task != nullptr );
	task->Go();
}

/******************************************************************************
 BroadcastProgramSet

 *****************************************************************************/

void
XDLink::BroadcastProgramSet()
{
	JString programName;
	GetProgram(&programName);

	Broadcast(SymbolsLoaded(kJFalse, programName));
}

/******************************************************************************
 ReloadProgram

 *****************************************************************************/

void
XDLink::ReloadProgram()
{
}

/******************************************************************************
 SetCore

 *****************************************************************************/

void
XDLink::SetCore
	(
	const JCharacter* fullName
	)
{
}

/******************************************************************************
 AttachToProcess

 *****************************************************************************/

void
XDLink::AttachToProcess
	(
	const pid_t pid
	)
{
}

/******************************************************************************
 RunProgram

 *****************************************************************************/

void
XDLink::RunProgram
	(
	const JCharacter* args
	)
{
}

/******************************************************************************
 GetBreakpointManager

 *****************************************************************************/

CMBreakpointManager*
XDLink::GetBreakpointManager()
{
	return itsBPMgr;
}

/******************************************************************************
 ShowBreakpointInfo

 *****************************************************************************/

void
XDLink::ShowBreakpointInfo
	(
	const JIndex debuggerIndex
	)
{
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
XDLink::SetBreakpoint
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex,
	const JBoolean		temporary
	)
{
	JString cmd = "breakpoint_set -t line -f ";
	cmd        += fileName;
	cmd        += " -n ";
	cmd        += lineIndex;

	if (temporary)
		{
		cmd += " -r 1";
		}

	Send(cmd);

	Broadcast(BreakpointsChanged());
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
XDLink::SetBreakpoint
	(
	const JCharacter*	address,
	const JBoolean		temporary
	)
{
}

/******************************************************************************
 RemoveBreakpoint

 *****************************************************************************/

void
XDLink::RemoveBreakpoint
	(
	const JIndex debuggerIndex
	)
{
	JString cmd = "breakpoint_remove -d ";
	cmd        += debuggerIndex;
	Send(cmd);

	Broadcast(BreakpointsChanged());
}

/******************************************************************************
 RemoveAllBreakpointsOnLine

 *****************************************************************************/

void
XDLink::RemoveAllBreakpointsOnLine
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
	JBoolean changed = kJFalse;

	JPtrArray<CMBreakpoint> list(JPtrArrayT::kForgetAll);
	JString cmd;
	if (itsBPMgr->GetBreakpoints(fileName, &list))
		{
		for (JIndex i=1; i<=list.GetElementCount(); i++)
			{
			CMBreakpoint* bp = list.GetElement(i);
			if (bp->GetLineNumber() == lineIndex)
				{
				cmd  = "breakpoint_remove -d ";
				cmd += bp->GetDebuggerIndex();
				Send(cmd);
				changed = kJTrue;
				}
			}
		}

	if (changed)
		{
		Broadcast(BreakpointsChanged());
		}
}

/******************************************************************************
 RemoveAllBreakpointsAtAddress

 *****************************************************************************/

void
XDLink::RemoveAllBreakpointsAtAddress
	(
	const JCharacter* addr
	)
{
}

/******************************************************************************
 RemoveAllBreakpoints

 *****************************************************************************/

void
XDLink::RemoveAllBreakpoints()
{
	JBoolean changed = kJFalse;

	const JPtrArray<CMBreakpoint>& list = itsBPMgr->GetBreakpoints();
	JString cmd;
	for (JIndex i=1; i<=list.GetElementCount(); i++)
		{
		const CMBreakpoint* bp = list.GetElement(i);

		cmd	 = "breakpoint_remove -d ";
		cmd += bp->GetDebuggerIndex();
		Send(cmd);
		changed = kJTrue;
		}

	if (changed)
		{
		Broadcast(BreakpointsChanged());
		}
}

/******************************************************************************
 SetBreakpointEnabled

 *****************************************************************************/

void
XDLink::SetBreakpointEnabled
	(
	const JIndex	debuggerIndex,
	const JBoolean	enabled,
	const JBoolean	once
	)
{
	JString cmd = "breakpoint_update -d ";
	cmd        += debuggerIndex;
	cmd        += " -s ";
	cmd        += (enabled ? "enabled" : "disabled");
	Send(cmd);

	Broadcast(BreakpointsChanged());
}

/******************************************************************************
 SetBreakpointCondition

 *****************************************************************************/

void
XDLink::SetBreakpointCondition
	(
	const JIndex		debuggerIndex,
	const JCharacter*	condition
	)
{
}

/******************************************************************************
 RemoveBreakpointCondition

 *****************************************************************************/

void
XDLink::RemoveBreakpointCondition
	(
	const JIndex debuggerIndex
	)
{
}

/******************************************************************************
 SetBreakpointIgnoreCount

 *****************************************************************************/

void
XDLink::SetBreakpointIgnoreCount
	(
	const JIndex	debuggerIndex,
	const JSize		count
	)
{
}

/******************************************************************************
 WatchExpression

 *****************************************************************************/

void
XDLink::WatchExpression
	(
	const JCharacter* expr
	)
{
}

/******************************************************************************
 WatchLocation

 *****************************************************************************/

void
XDLink::WatchLocation
	(
	const JCharacter* expr
	)
{
}

/******************************************************************************
 SwitchToThread

 *****************************************************************************/

void
XDLink::SwitchToThread
	(
	const JUInt64 id
	)
{
}

/******************************************************************************
 SwitchToFrame

 *****************************************************************************/

void
XDLink::SwitchToFrame
	(
	const JUInt64 id
	)
{
	if (id != itsStackFrameIndex)
		{
		itsStackFrameIndex = id;
		Broadcast(FrameChanged());
		}

	const CMStackFrameNode* frame;
	JString fileName;
	JIndex lineIndex;
	if (CMGetCommandDirector()->GetStackDir()->GetStackWidget()->GetStackFrame(id, &frame) &&
		frame->GetFile(&fileName, &lineIndex))
		{
		if (fileName.BeginsWith("file://"))
			{
			fileName.RemoveSubstring(1, 7);
			}
		Broadcast(ProgramStopped(CMLocation(fileName, lineIndex)));
		}
}

/******************************************************************************
 StepOver

 *****************************************************************************/

void
XDLink::StepOver()
{
	Send("step_over");
	itsProgramIsStoppedFlag = kJFalse;
	Broadcast(ProgramRunning());
}

/******************************************************************************
 StepInto

 *****************************************************************************/

void
XDLink::StepInto()
{
	Send("step_into");
	itsProgramIsStoppedFlag = kJFalse;
	Broadcast(ProgramRunning());
}

/******************************************************************************
 StepOut

 *****************************************************************************/

void
XDLink::StepOut()
{
	Send("step_out");
	itsProgramIsStoppedFlag = kJFalse;
	Broadcast(ProgramRunning());
}

/******************************************************************************
 Continue

 *****************************************************************************/

void
XDLink::Continue()
{
	Send("run");
	itsProgramIsStoppedFlag = kJFalse;
	Broadcast(ProgramRunning());
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
XDLink::RunUntil
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
	SetBreakpoint(fileName, lineIndex, kJTrue);
	Continue();
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
XDLink::SetExecutionPoint
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
}

/******************************************************************************
 SetValue

 *****************************************************************************/

void
XDLink::SetValue
	(
	const JCharacter* name,
	const JCharacter* value
	)
{
	if (ProgramIsStopped())
		{
		const JString encValue = JString(value).EncodeBase64();

		JString cmd = "property_set @i -n ";
		cmd        += name;
		cmd        += " -l ";
		cmd        += encValue.GetLength();
		cmd        += " -- ";
		cmd        += encValue;
		Send(cmd);

		Broadcast(ValueChanged());
		}
}

/******************************************************************************
 CreateArray2DCommand

 *****************************************************************************/

CMArray2DCommand*
XDLink::CreateArray2DCommand
	(
	CMArray2DDir*		dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
{
	CMArray2DCommand* cmd = jnew XDArray2DCommand(dir, table, data);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreatePlot2DCommand

 *****************************************************************************/

CMPlot2DCommand*
XDLink::CreatePlot2DCommand
	(
	CMPlot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
{
	CMPlot2DCommand* cmd = jnew XDPlot2DCommand(dir, x, y);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateDisplaySourceForMain

 *****************************************************************************/

CMDisplaySourceForMain*
XDLink::CreateDisplaySourceForMain
	(
	CMSourceDirector* sourceDir
	)
{
	CMDisplaySourceForMain* cmd = jnew XDDisplaySourceForMain(sourceDir);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetCompletions

 *****************************************************************************/

CMGetCompletions*
XDLink::CreateGetCompletions
	(
	CMCommandInput*	input,
	CMCommandOutputDisplay*	history
	)
{
	CMGetCompletions* cmd = jnew XDGetCompletions(input, history);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetFrame

 *****************************************************************************/

CMGetFrame*
XDLink::CreateGetFrame
	(
	CMStackWidget* widget
	)
{
	CMGetFrame* cmd = jnew XDGetFrame(widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetStack

 *****************************************************************************/

CMGetStack*
XDLink::CreateGetStack
	(
	JTree*			tree,
	CMStackWidget*	widget
	)
{
	CMGetStack* cmd = jnew XDGetStack(tree, widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetThread

 *****************************************************************************/

CMGetThread*
XDLink::CreateGetThread
	(
	CMThreadsWidget* widget
	)
{
	CMGetThread* cmd = jnew XDGetThread(widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetThreads

 *****************************************************************************/

CMGetThreads*
XDLink::CreateGetThreads
	(
	JTree*				tree,
	CMThreadsWidget*	widget
	)
{
	CMGetThreads* cmd = jnew XDGetThreads(tree, widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetFullPath

 *****************************************************************************/

CMGetFullPath*
XDLink::CreateGetFullPath
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
	CMGetFullPath* cmd = jnew XDGetFullPath(fileName, lineIndex);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetInitArgs

 *****************************************************************************/

CMGetInitArgs*
XDLink::CreateGetInitArgs
	(
	JXInputField* argInput
	)
{
	CMGetInitArgs* cmd = jnew XDGetInitArgs(argInput);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetLocalVars

 *****************************************************************************/

CMGetLocalVars*
XDLink::CreateGetLocalVars
	(
	CMVarNode* rootNode
	)
{
	CMGetLocalVars* cmd = jnew XDGetLocalVars(rootNode);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetSourceFileList

 *****************************************************************************/

CMGetSourceFileList*
XDLink::CreateGetSourceFileList
	(
	CMFileListDir* fileList
	)
{
	CMGetSourceFileList* cmd = jnew XDGetSourceFileList(fileList);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarValueCommand

 *****************************************************************************/

CMVarCommand*
XDLink::CreateVarValueCommand
	(
	const JCharacter* expr
	)
{
	JString s = "property_get -n ";
	s        += expr;
	s        += " -d ";
	s        += itsStackFrameIndex;

	CMVarCommand* cmd = jnew XDVarCommand(s);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarContentCommand

 *****************************************************************************/

CMVarCommand*
XDLink::CreateVarContentCommand
	(
	const JCharacter* expr
	)
{
	return CreateVarValueCommand(expr);
}

/******************************************************************************
 CreateVarNode

 *****************************************************************************/

CMVarNode*
XDLink::CreateVarNode
	(
	const JBoolean shouldUpdate		// kJFalse for Local Variables
	)
{
	CMVarNode* node = jnew XDVarNode(shouldUpdate);
	assert( node != nullptr );
	return node;
}

CMVarNode*
XDLink::CreateVarNode
	(
	JTreeNode*			parent,
	const JCharacter*	name,
	const JCharacter*	fullName,
	const JCharacter*	value
	)
{
	CMVarNode* node = jnew XDVarNode(parent, name, fullName, value);
	assert( node != nullptr );
	return node;
}

/******************************************************************************
 Build1DArrayExpression

 *****************************************************************************/

JString
XDLink::Build1DArrayExpression
	(
	const JCharacter*	origExpr,
	const JInteger		index
	)
{
	JString expr = origExpr;

	const JString indexStr(index, 0);	// must use floating point conversion
	if (expr.Contains("$i"))
		{
		// double literal $'s

		for (JIndex i=expr.GetLength()-1; i>=1; i--)
			{
			if (expr.GetCharacter(i)   == '$' &&
				expr.GetCharacter(i+1) != 'i')
				{
				expr.InsertCharacter('$', i);
				}
			}

		const JCharacter* map[] =
			{
			"i", indexStr.GetCString()
			};
		(JGetStringManager())->Replace(&expr, map, sizeof(map));
		}
	else
		{
		expr.AppendCharacter('[');
		expr += indexStr;
		expr.AppendCharacter(']');
		}

	return expr;
}

/******************************************************************************
 Build2DArrayExpression

 *****************************************************************************/

JString
XDLink::Build2DArrayExpression
	(
	const JCharacter*	origExpr,
	const JInteger		rowIndex,
	const JInteger		colIndex
	)
{
	JString expr = origExpr;

	const JBoolean usesI = expr.Contains("$i");		// row
	const JBoolean usesJ = expr.Contains("$j");		// col

	const JString iStr(rowIndex, 0);	// must use floating point conversion
	const JString jStr(colIndex, 0);	// must use floating point conversion

	// We have to do both at the same time because otherwise we lose a $.

	if (usesI || usesJ)
		{
		// double literal $'s

		for (JIndex i=expr.GetLength()-1; i>=1; i--)
			{
			if (expr.GetCharacter(i)   == '$' &&
				expr.GetCharacter(i+1) != 'i' &&
				expr.GetCharacter(i+1) != 'j')
				{
				expr.InsertCharacter('$', i);
				}
			}

		const JCharacter* map[] =
			{
			"i", iStr.GetCString(),
			"j", jStr.GetCString()
			};
		(JGetStringManager())->Replace(&expr, map, sizeof(map));
		}

	if (!usesI || !usesJ)
		{
		if (expr.GetFirstCharacter() != '(' ||
			expr.GetLastCharacter()  != ')')
			{
			expr.PrependCharacter('(');
			expr.AppendCharacter(')');
			}

		if (!usesI)
			{
			expr.AppendCharacter('[');
			expr += iStr;
			expr.AppendCharacter(']');
			}
		if (!usesJ)
			{
			expr.AppendCharacter('[');
			expr += jStr;
			expr.AppendCharacter(']');
			}
		}

	return expr;
}

/******************************************************************************
 CreateGetMemory

 *****************************************************************************/

CMGetMemory*
XDLink::CreateGetMemory
	(
	CMMemoryDir* dir
	)
{
	return nullptr;
}

/******************************************************************************
 CreateGetAssembly

 *****************************************************************************/

CMGetAssembly*
XDLink::CreateGetAssembly
	(
	CMSourceDirector* dir
	)
{
	return nullptr;
}

/******************************************************************************
 CreateGetRegisters

 *****************************************************************************/

CMGetRegisters*
XDLink::CreateGetRegisters
	(
	CMRegistersDir* dir
	)
{
	return nullptr;
}

/******************************************************************************
 Send

	Sends the given text as command(s) to xdebug.

 *****************************************************************************/

void
XDLink::Send
	(
	const JCharacter* text
	)
{
	if (itsLink != nullptr)
		{
		if (ProgramIsRunning())
			{
			StopProgram();
			}

		JString arg = " -i not_command";

		JString s = text;
		JIndex i;
		if (s.LocateSubstring("@i", &i))
			{
			s.ReplaceSubstring(i, i+1, arg);
			}
		else
			{
			s += arg;
			}

		SendRaw(s);
		}
}

/******************************************************************************
 SendRaw

	Sends the given text as text to whatever is currently accepting text.

 *****************************************************************************/

void
XDLink::SendRaw
	(
	const JCharacter* text
	)
{
	if (itsLink != nullptr)
		{
		JString s = text;
		s.TrimWhitespace();

		JIndex i;
		while (s.LocateSubstring("  ", &i))
			{
			s.ReplaceSubstring(i, i+1, " ");
			}

		itsLink->SendMessage(s);
		itsLink->StartTimer();

		if (!itsDebuggerBusyFlag)
			{
			itsDebuggerBusyFlag = kJTrue;
			Broadcast(DebuggerBusy());
			}

		Broadcast(DebugOutput(s, kCommandType));
		}
}

/******************************************************************************
 SendMedicCommand (virtual protected)

 *****************************************************************************/

void
XDLink::SendMedicCommand
	(
	CMCommand* command
	)
{
	command->Starting();

	JString arg = " -i ";
	arg        += JString(command->GetTransactionID(), JString::kBase10);

	JString s = command->GetCommand();
	JIndex i;
	if (s.LocateSubstring("@i", &i))
		{
		s.ReplaceSubstring(i, i+1, arg);
		}
	else
		{
		s += arg;
		}

	SendRaw(s);
}

/******************************************************************************
 StopProgram

	xdebug 1.0.4 doesn't support break.  It returns an error and treats it
	as "continue"

 *****************************************************************************/

void
XDLink::StopProgram()
{
//	SendRaw("break -i break");
}

/******************************************************************************
 KillProgram

 *****************************************************************************/

void
XDLink::KillProgram()
{
}

/******************************************************************************
 OKToDetachOrKill

 *****************************************************************************/

JBoolean
XDLink::OKToDetachOrKill()
	const
{
	return kJTrue;
}

/******************************************************************************
 StartDebugger (private)

	We cannot send anything to xdebug until it has successfully started.

 *****************************************************************************/

JBoolean
XDLink::StartDebugger()
{
	if (itsAcceptor == nullptr)
		{
		itsAcceptor = jnew XDAcceptor;
		assert( itsAcceptor != nullptr );
		}

	const JString portStr(kXdebugPort, JString::kBase10);
	if (itsAcceptor->open(ACE_INET_Addr(kXdebugPort)) == -1)
		{
		const JString errStr(jerrno(), JString::kBase10);

		const JCharacter* map[] =
			{
			"port",  portStr,
			"errno", errStr
			};
		JString msg = JGetString("ListenError::XDLink", map, sizeof(map));

		XDWelcomeTask* task = jnew XDWelcomeTask(msg, kJTrue);
		assert( task != nullptr );
		task->Go();
		return kJFalse;
		}
	else
		{
		const JCharacter* map[] =
			{
			"port", portStr
			};
		JString msg = JGetString("Welcome::XDLink", map, sizeof(map));

		XDWelcomeTask* task = jnew XDWelcomeTask(msg, kJFalse);
		assert( task != nullptr );
		task->Go();
		return kJTrue;
		}
}

/******************************************************************************
 ChangeDebugger

 *****************************************************************************/

JBoolean
XDLink::ChangeDebugger()
{
	return kJTrue;
}

/******************************************************************************
 RestartDebugger

 *****************************************************************************/

JBoolean
XDLink::RestartDebugger()
{
	StopDebugger();
	const JBoolean ok = StartDebugger();

	if (ok)
		{
		Broadcast(DebuggerRestarted());
		}

	return ok;
}

/******************************************************************************
 StopDebugger (private)

 *****************************************************************************/

void
XDLink::StopDebugger()
{
	Send("detach");

	jdelete itsLink;
	itsLink = nullptr;

	CancelAllCommands();

	InitFlags();
}

/******************************************************************************
 ConnectionEstablished

 *****************************************************************************/

void
XDLink::ConnectionEstablished
	(
	XDSocket* socket
	)
{
	InitFlags();

	itsLink = socket;
	ListenTo(itsLink);

	itsIDEKey.Clear();

	itsAcceptor->close();

	Broadcast(DebuggerStarted());
	Broadcast(UserOutput(JGetString("Connected::XDLink"), kJFalse));
}

/******************************************************************************
 ConnectionFinished

 *****************************************************************************/

void
XDLink::ConnectionFinished
	(
	XDSocket* socket
	)
{
	assert( socket == itsLink );

	itsLink = nullptr;
	itsIDEKey.Clear();

	RestartDebugger();
}
