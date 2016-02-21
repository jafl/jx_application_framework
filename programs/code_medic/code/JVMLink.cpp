/******************************************************************************
 JVMLink.cpp

	Interface to JVM debugging agent.

	BASE CLASS = CMLink

	Copyright © 2009 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
#include "JVMLink.h"
#include "JVMWelcomeTask.h"
#include "JVMSetProgramTask.h"
#include "JVMBreakpointManager.h"
#include "JVMThreadNode.h"

#include "JVMArray2DCommand.h"
#include "JVMPlot2DCommand.h"
#include "JVMDisplaySourceForMain.h"
#include "JVMGetCompletions.h"
#include "JVMGetFrame.h"
#include "JVMGetStack.h"
#include "JVMGetThread.h"
#include "JVMGetThreads.h"
#include "JVMGetThreadGroups.h"
#include "JVMGetThreadName.h"
#include "JVMGetThreadParent.h"
#include "JVMGetClassInfo.h"
#include "JVMGetClassMethods.h"
#include "JVMGetFullPath.h"
#include "JVMGetIDSizes.h"
#include "JVMGetInitArgs.h"
#include "JVMGetLocalVars.h"
#include "JVMGetSourceFileList.h"
#include "JVMVarCommand.h"
#include "JVMVarNode.h"

#include "CMCommandDirector.h"
#include "CMSourceDirector.h"
#include "CMMDIServer.h"
#include "cmGlobals.h"

#include <JXTimerTask.h>
#include <JXAssert.h>
#include <JTree.h>
#include <JTreeNode.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jErrno.h>
#include <jAssert.h>

const JIndex kJavaPort = 9001;

static const JBoolean kFeatures[]=
{
	kJTrue,		// kSetProgram
	kJTrue,		// kSetArgs
	kJFalse,	// kSetCore
	kJFalse,	// kSetProcess
	kJTrue,		// kRunProgram
	kJTrue,		// kStopProgram
	kJFalse,	// kSetExecutionPoint
	kJFalse,	// kExecuteBackwards
	kJTrue,		// kShowBreakpointInfo
	kJFalse,	// kSetBreakpointCondition
	kJTrue,		// kSetBreakpointIgnoreCount
	kJFalse,	// kWatchExpression
	kJFalse,	// kWatchLocation
	kJFalse,	// kExamineMemory
	kJFalse,	// kDisassembleMemory
};

// JBroadcaster message types

const JCharacter* JVMLink::kIDResolved = "IDResolved::JVMLink";

/******************************************************************************
 Constructor

 *****************************************************************************/

JVMLink::JVMLink()
	:
	CMLink(kFeatures),
	itsAcceptor(NULL),
	itsDebugLink(NULL),
	itsProcess(NULL),
	itsOutputLink(NULL),
	itsInputLink(NULL),
	itsLatestMsg(NULL),
	itsJVMDeathTask(NULL)
{
	InitFlags();

	itsBPMgr = new JVMBreakpointManager(this);
	assert( itsBPMgr != NULL );

	itsSourcePathList = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsSourcePathList != NULL );

	itsClassByIDList = new JArray<ClassInfo>;
	assert( itsClassByIDList != NULL );
	itsClassByIDList->SetCompareFunction(CompareClassIDs);

	itsClassByNameList = new JAliasArray<ClassInfo>(itsClassByIDList, CompareClassNames, JOrderedSetT::kSortAscending);
	assert( itsClassByNameList != NULL );

	itsThreadRoot = new JVMThreadNode(JVMThreadNode::kGroupType, JVMThreadNode::kRootThreadGroupID);
	assert( itsThreadRoot != NULL );

	itsThreadTree = new JTree(itsThreadRoot);
	assert( itsThreadTree != NULL );

	itsThreadList = new JPtrArray<JVMThreadNode>(JPtrArrayT::kForgetAll);
	assert( itsThreadList != NULL );
	itsThreadList->SetCompareFunction(JVMThreadNode::CompareID);

	itsCullThreadGroupsTask = new JXTimerTask(10000);
	assert( itsCullThreadGroupsTask != NULL );
	itsCullThreadGroupsTask->Start();
	ListenTo(itsCullThreadGroupsTask);
	itsCullThreadGroupIndex = 1;

	itsFrameList = new JArray<FrameInfo>();
	assert( itsFrameList != NULL );

	StartDebugger();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JVMLink::~JVMLink()
{
	StopDebugger();
	DeleteAcceptor();

	delete itsBPMgr;
	delete itsSourcePathList;
	delete itsClassByIDList;
	delete itsClassByNameList;
	delete itsThreadTree;
	delete itsThreadList;
	delete itsCullThreadGroupsTask;
	delete itsFrameList;
	delete itsJVMDeathTask;

	DeleteOneShotCommands();
}

/******************************************************************************
 InitFlags (private)

 *****************************************************************************/

void
JVMLink::InitFlags()
{
	itsInitFinishedFlag     = kJFalse;
	itsProgramIsStoppedFlag = kJFalse;
	itsCurrentThreadID      = JVMThreadNode::kRootThreadGroupID;
}

/******************************************************************************
 GetPrompt

 ******************************************************************************/

const JString&
JVMLink::GetPrompt()
	const
{
	return JGetString("Prompt::JVMLink");
}

/******************************************************************************
 GetScriptPrompt

 ******************************************************************************/

const JString&
JVMLink::GetScriptPrompt()
	const
{
	return JGetString("ScriptPrompt::JVMLink");
}

/******************************************************************************
 DebuggerHasStarted

 ******************************************************************************/

JBoolean
JVMLink::DebuggerHasStarted()
	const
{
	return kJTrue;
}

/******************************************************************************
 GetChooseProgramInstructions

 ******************************************************************************/

JString
JVMLink::GetChooseProgramInstructions()
	const
{
	return JGetString("ChooseProgramInstr::JVMLink");
}

/******************************************************************************
 HasProgram

 ******************************************************************************/

JBoolean
JVMLink::HasProgram()
	const
{
	return JI2B(!itsJVMExecArgs.IsEmpty() || itsDebugLink != NULL);
}

/******************************************************************************
 GetProgram

 ******************************************************************************/

JBoolean
JVMLink::GetProgram
	(
	JString* fullName
	)
	const
{
	if (itsMainClassName.IsEmpty())
		{
		*fullName = "Java Process";
		}
	else
		{
		*fullName = itsMainClassName;

		JIndex i;
		if (fullName->LocateLastSubstring(".", &i))
			{
			fullName->RemoveSubstring(1, i);
			}
		}

	return kJTrue;
}

/******************************************************************************
 HasCore

 ******************************************************************************/

JBoolean
JVMLink::HasCore()
	const
{
	return kJFalse;
}

/******************************************************************************
 GetCore

 ******************************************************************************/

JBoolean
JVMLink::GetCore
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
JVMLink::HasLoadedSymbols()
	const
{
	return JI2B(itsDebugLink != NULL);
}

/******************************************************************************
 IsDebugging

 *****************************************************************************/

JBoolean
JVMLink::IsDebugging()
	const
{
	return JI2B(itsDebugLink != NULL);
}

/******************************************************************************
 ProgramIsRunning

 *****************************************************************************/

JBoolean
JVMLink::ProgramIsRunning()
	const
{
	return JI2B(itsDebugLink != NULL && !itsProgramIsStoppedFlag);
}

/******************************************************************************
 ProgramIsStopped

 *****************************************************************************/

JBoolean
JVMLink::ProgramIsStopped()
	const
{
	return JI2B(itsDebugLink != NULL && itsProgramIsStoppedFlag);
}

/******************************************************************************
 OKToSendCommands

 *****************************************************************************/

JBoolean
JVMLink::OKToSendCommands
	(
	const JBoolean background
	)
	const
{
	return kJTrue;
}

/******************************************************************************
 IsDefiningScript

	This forces user's typing to be echoed directly.

 *****************************************************************************/

JBoolean
JVMLink::IsDefiningScript()
	const
{
	return kJTrue;
}

/******************************************************************************
 Receive (virtual protected)

 *****************************************************************************/

void
JVMLink::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDebugLink && message.Is(JVMSocket::kMessageReady))
		{
		const JVMSocket::MessageReady* info =
			dynamic_cast<const JVMSocket::MessageReady*>(&message);
		assert( info != NULL );
		ReceiveMessageFromJVM(*info);
		}
	else if (sender == itsInputLink && message.Is(CMPipeT::kReadReady))
		{
		ReadFromProcess();
		}

	else if (sender == itsProcess && message.Is(JProcess::kFinished))
		{
		const JProcess::Finished* info =
			dynamic_cast<const JProcess::Finished*>(&message);
		assert( info != NULL );
		ProgramFinished1(info);

		delete itsJVMDeathTask;
		itsJVMDeathTask = NULL;

		delete itsProcess;
		itsProcess = NULL;
		}
	else if (sender == itsJVMDeathTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		ProgramFinished1(NULL);
		itsJVMDeathTask = NULL;
		}

	else if (sender == itsThreadTree)
		{
		Broadcast(ThreadListChanged());
		}
	else if (sender == itsCullThreadGroupsTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		CheckNextThreadGroup();
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveMessageFromJVM (private)

 *****************************************************************************/

void
JVMLink::ReceiveMessageFromJVM
	(
	const JVMSocket::MessageReady& info
	)
{
	if (info.IsReply())
		{
		std::ostringstream log;
		log << "reply: " << info.GetID() << ' ' << info.GetErrorCode();
		Log(log);

		HandleCommandRunning(info.GetID());

		CMCommand* cmd;
		if (GetRunningCommand(&cmd))
			{
			itsLatestMsg = &info;

			cmd->Finished(JI2B(info.GetErrorCode() == 0));

			itsLatestMsg = NULL;

			SetRunningCommand(NULL);
			if (!HasForegroundCommands())
				{
				RunNextCommand();
				}
			}
		}
	else if (info.GetCommandSet() == kEventCmdSet &&
			 info.GetCommand()    == kECompositeCmd)
		{
		DispatchEventsFromJVM(info.GetData(), info.GetDataLength());
		}
}

/******************************************************************************
 DispatchEventsFromJVM (private)

 *****************************************************************************/

void
JVMLink::DispatchEventsFromJVM
	(
	const unsigned char*	data,
	const JSize				length
	)
{
	const JIndex suspendPolicy = *data;
	data++;

	const JSize count = JVMSocket::Unpack4(data);
	data += 4;

	for (JIndex i=1; i<=1; i++)		// cannot loop until implement unpack for *all* events
		{
		const JIndex type = *data;
		data++;

		const JIndex requestID = JVMSocket::Unpack4(data);
		data += 4;

		std::ostringstream log;
		log << "event: " << type;
		Log(log);

		if (type == kVMDeathEvent)
			{
			itsJVMDeathTask = new JXTimerTask(1000, kJTrue);
			assert( itsJVMDeathTask != NULL );
			itsJVMDeathTask->Start();
			ListenTo(itsJVMDeathTask);
			}
		else if (type == kClassUnloadEvent)
			{
			JSize count;
			const JString signature = JVMSocket::UnpackString(data, &count);
			data += count;

			const JString name = ClassSignatureToName(signature);

			std::ostringstream log2;
			log2 << "unload class: " << name;
			Log(log2);

			ClassInfo info;
			info.name = const_cast<JString*>(&name);
			JIndex j;
			if (itsClassByNameList->SearchSorted(info, JOrderedSetT::kAnyMatch, &j))
				{
				info = itsClassByNameList->GetElement(j);
				info.Clean();
				itsClassByIDList->RemoveElement(itsClassByNameList->GetElementIndex(j));
				}
			}
		else if (type == kThreadStartEvent)
			{
			const JUInt64 threadID = JVMSocket::Unpack(itsObjectIDSize, data);
			data += itsObjectIDSize;

			std::ostringstream log2;
			log2 << "thread started: " << threadID;
			Log(log2);

			JVMThreadNode* node;
			if (!FindThread(threadID, &node))	// might be created by JVMGetThreadGroups
				{
				node = new JVMThreadNode(JVMThreadNode::kThreadType, threadID);
				assert( node != NULL );
				}
			}
		else if (type == kThreadDeathEvent)
			{
			const JUInt64 threadID = JVMSocket::Unpack(itsObjectIDSize, data);
			data += itsObjectIDSize;

			std::ostringstream log2;
			log2 << "thread finished: " << threadID;
			Log(log2);

			JVMThreadNode* node;
			if (FindThread(threadID, &node))
				{
				delete node;
				}
			}
		}
}

/******************************************************************************
 SetIDSizes

 *****************************************************************************/

void
JVMLink::SetIDSizes
	(
	const JSize fieldIDSize,
	const JSize methodIDSize,
	const JSize objectIDSize,
	const JSize refTypeIDSize,
	const JSize frameIDSize
	)
{
	itsFieldIDSize         = fieldIDSize;
	itsMethodIDSize        = methodIDSize;
	itsObjectIDSize        = objectIDSize;
	itsReferenceTypeIDSize = refTypeIDSize;
	itsFrameIDSize         = frameIDSize;

	std::ostringstream log;
	log << "field id size: " << itsFieldIDSize << endl;
	log << "method id size: " << itsFieldIDSize << endl;
	log << "object id size: " << itsFieldIDSize << endl;
	log << "reference type id size: " << itsFieldIDSize << endl;
	log << "frame id size: " << itsFieldIDSize;
	Log(log);
}

/******************************************************************************
 ThreadCreated

 *****************************************************************************/

void
JVMLink::ThreadCreated
	(
	JVMThreadNode* node
	)
{
	itsThreadList->InsertSorted(node);
}

/******************************************************************************
 ThreadDeleted

 *****************************************************************************/

void
JVMLink::ThreadDeleted
	(
	JVMThreadNode* node
	)
{
	JIndex i;
	if (itsThreadList->Find(node, &i))
		{
		itsThreadList->RemoveElement(i);

		if (itsCurrentThreadID == node->GetID())
			{
			itsCurrentThreadID = JVMThreadNode::kRootThreadGroupID;
			}
		}
}

/******************************************************************************
 FindThread

 *****************************************************************************/

JBoolean
JVMLink::FindThread
	(
	const JUInt64	id,
	JVMThreadNode**	node
	)
	const
{
	JVMThreadNode target(id);
	JIndex i;
	if (itsThreadList->SearchSorted(&target, JOrderedSetT::kAnyMatch, &i))
		{
		*node = itsThreadList->NthElement(i);
		return kJTrue;
		}
	else
		{
		*node = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 CheckNextThreadGroup (private)

 *****************************************************************************/

void
JVMLink::CheckNextThreadGroup()
{
	if (itsThreadList->IsEmpty())
		{
		return;
		}

	if (!itsThreadList->IndexValid(itsCullThreadGroupIndex))
		{
		itsCullThreadGroupIndex = 1;
		}

	JVMThreadNode* node;
	while (1)
		{
		node = itsThreadList->NthElement(itsCullThreadGroupIndex);
		if (node->GetType() == JVMThreadNode::kGroupType)
			{
			break;
			}

		itsCullThreadGroupIndex++;
		if (!itsThreadList->IndexValid(itsCullThreadGroupIndex))
			{
			return;
			}
		}

	CMCommand* cmd = new JVMGetThreadParent(node, kJTrue);
	assert( cmd != NULL );

	itsCullThreadGroupIndex++;
}

/******************************************************************************
 FlushClassList

 *****************************************************************************/

void
JVMLink::FlushClassList()
{
	const JSize count = itsClassByIDList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		ClassInfo info = itsClassByIDList->GetElement(i);
		info.Clean();
		}

	itsClassByIDList->RemoveAll();

	Log("Flush class list");
}

/******************************************************************************
 AddClass

 *****************************************************************************/

void
JVMLink::AddClass
	(
	const JUInt64		id,
	const JCharacter*	signature
	)
{
	ClassInfo info;
	info.id = id;

	JIndex i;
	if (itsClassByIDList->SearchSorted(info, JOrderedSetT::kAnyMatch, &i))
		{
		return;
		}

	info.name = new JString(ClassSignatureToName(signature));
	assert( info.name != NULL );

	JString path;
	if (ClassSignatureToFile(signature, &path))
		{
		info.path = new JString(path);
		assert( info.path != NULL );
		}
	else
		{
		info.path = NULL;
		}

	info.methods = new JArray<MethodInfo>();
	assert( info.methods != NULL );
	info.methods->SetCompareFunction(CompareMethodIDs);

	itsClassByIDList->InsertSorted(info);

	std::ostringstream log;
	log << "AddClass: " << id << ' ' << signature;
	Log(log);

	Broadcast(IDResolved(id));

	CMCommand* cmd = new JVMGetClassMethods(id);
	assert( cmd != NULL );
}

/******************************************************************************
 GetClassName

 *****************************************************************************/

JBoolean
JVMLink::GetClassName
	(
	const JUInt64	id,
	JString*		name
	)
{
	ClassInfo target;
	target.id = id;
	JIndex i;
	if (itsClassByIDList->SearchSorted(target, JOrderedSetT::kAnyMatch, &i))
		{
		target = itsClassByIDList->GetElement(i);
		*name  = *(target.name);
		return kJTrue;
		}
	else
		{
		CMCommand* cmd = new JVMGetClassInfo(id);
		assert( cmd != NULL );

		name->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 GetClassSourceFile

 *****************************************************************************/

JBoolean
JVMLink::GetClassSourceFile
	(
	const JUInt64	id,
	JString*		fullName
	)
{
	ClassInfo target;
	target.id = id;
	JIndex i;
	if (itsClassByIDList->SearchSorted(target, JOrderedSetT::kAnyMatch, &i))
		{
		target = itsClassByIDList->GetElement(i);
		if (target.path != NULL)
			{
			*fullName = *(target.path);
			return kJTrue;
			}
		}

	fullName->Clear();
	return kJFalse;
}

/******************************************************************************
 AddMethod

 *****************************************************************************/

void
JVMLink::AddMethod
	(
	const JUInt64		classID,
	const JUInt64		methodID,
	const JCharacter*	name
	)
{
	ClassInfo target;
	target.id = classID;
	JIndex i;
	const JBoolean found = itsClassByIDList->SearchSorted(target, JOrderedSetT::kAnyMatch, &i);
	assert( found );

	target = itsClassByIDList->GetElement(i);

	MethodInfo info;
	info.id = methodID;

	info.name = new JString(name);
	assert( info.name != NULL );

	target.methods->InsertSorted(info);

	std::ostringstream log;
	log << "AddMethod: " << *(target.name) << ": " << methodID << ' ' << name;
	Log(log);

	Broadcast(IDResolved(methodID));
}

/******************************************************************************
 GetMethodName

 *****************************************************************************/

JBoolean
JVMLink::GetMethodName
	(
	const JUInt64	classID,
	const JUInt64	methodID,
	JString*		name
	)
{
	ClassInfo target1;
	target1.id = classID;
	JIndex i;
	if (!itsClassByIDList->SearchSorted(target1, JOrderedSetT::kAnyMatch, &i))
		{
		name->Clear();
		return kJFalse;
		}

	target1 = itsClassByIDList->GetElement(i);

	MethodInfo target2;
	target2.id = methodID;
	if (target1.methods->SearchSorted(target2, JOrderedSetT::kAnyMatch, &i))
		{
		target2 = target1.methods->GetElement(i);
		*name   = *(target2.name);
		return kJTrue;
		}
	else
		{
		name->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 ClassSignatureToResourcePath (static)

 ******************************************************************************/

JString
JVMLink::ClassSignatureToResourcePath
	(
	const JCharacter* signature
	)
{
	JString path = signature;
	if (path.GetFirstCharacter() == 'L')
		{
		path.RemoveSubstring(1,1);
		}

	JIndex i;
	if (path.LocateSubstring("$", &i))
		{
		path.RemoveSubstring(i, path.GetLength());
		}
	else if (path.GetLastCharacter() == ';')
		{
		path.RemoveSubstring(path.GetLength(), path.GetLength());
		}

	return path;
}

/******************************************************************************
 ClassSignatureToName (static)

 ******************************************************************************/

JString
JVMLink::ClassSignatureToName
	(
	const JCharacter* signature
	)
{
	JString name = ClassSignatureToResourcePath(signature);

	const JSize length = name.GetLength();
	for (JIndex j=1; j<=length; j++)
		{
		if (name.GetCharacter(j) == ACE_DIRECTORY_SEPARATOR_CHAR)
			{
			name.SetCharacter(j, '.');
			}
		}

	return name;
}

/******************************************************************************
 ClassNameToResourcePath (static)

 ******************************************************************************/

JString
JVMLink::ClassNameToResourcePath
	(
	const JCharacter* name
	)
{
	JString file = name;

	const JSize length = file.GetLength();
	for (JIndex j=1; j<=length; j++)
		{
		if (file.GetCharacter(j) == '.')
			{
			file.SetCharacter(j, ACE_DIRECTORY_SEPARATOR_CHAR);
			}
		}

	return file;
}

/******************************************************************************
 ClassSignatureToFile (private)

 ******************************************************************************/

JBoolean
JVMLink::ClassSignatureToFile
	(
	const JCharacter*	signature,
	JString*			fullName
	)
	const
{
	JString file = ClassSignatureToResourcePath(signature);
	file        += ".java";

	const JSize pathCount = itsSourcePathList->GetElementCount();
	for (JIndex i=1; i<=pathCount; i++)
		{
		const JString* path = itsSourcePathList->NthElement(i);
		*fullName           = JCombinePathAndName(*path, file);
		if (JFileReadable(*fullName))
			{
			return kJTrue;
			}
		}

	fullName->Clear();
	return kJFalse;
}

/******************************************************************************
 CompareClassIDs (private static)

 ******************************************************************************/

JOrderedSetT::CompareResult
JVMLink::CompareClassIDs
	(
	const ClassInfo& c1,
	const ClassInfo& c2
	)
{
	if (c1.id > c2.id)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (c1.id < c2.id)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}

/******************************************************************************
 CompareClassNames (private static)

 ******************************************************************************/

JOrderedSetT::CompareResult
JVMLink::CompareClassNames
	(
	const ClassInfo& c1,
	const ClassInfo& c2
	)
{
	return JCompareStringsCaseSensitive(c1.name, c2.name);
}

/******************************************************************************
 CompareMethodIDs (private static)

 ******************************************************************************/

JOrderedSetT::CompareResult
JVMLink::CompareMethodIDs
	(
	const MethodInfo& m1,
	const MethodInfo& m2
	)
{
	if (m1.id > m2.id)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (m1.id < m2.id)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}

/******************************************************************************
 FlushFrameList

 *****************************************************************************/

void
JVMLink::FlushFrameList()
{
	itsFrameList->RemoveAll();
}

/******************************************************************************
 AddFrame

 *****************************************************************************/

void
JVMLink::AddFrame
	(
	const JUInt64 id,
	const JUInt64 classID,
	const JUInt64 methodID,
	const JUInt64 codeOffset
	)
{
	FrameInfo info;
	info.id         = id;
	info.classID    = classID;
	info.methodID   = methodID;
	info.codeOffset = codeOffset;

	itsFrameList->AppendElement(info);
}

/******************************************************************************
 GetFrame (private)

	Normally not enough frames to make it worth doing binary search.

 *****************************************************************************/

JBoolean
JVMLink::GetFrame
	(
	const JUInt64	id,
	JIndex*			index
	)
	const
{
	const JSize count = itsFrameList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		FrameInfo info = itsFrameList->GetElement(i);
		if (info.id == id)
			{
			*index = i;
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}

/******************************************************************************
 ReadFromProcess (private)

 *****************************************************************************/

void
JVMLink::ReadFromProcess()
{
	JString data;
	itsInputLink->Read(&data);
	Broadcast(UserOutput(data, kJFalse, kJTrue));
}

/******************************************************************************
 SetProgram

 *****************************************************************************/

void
JVMLink::SetProgram
	(
	const JCharacter* fileName
	)
{
	DetachOrKill();

//	StopDebugger();		// avoid broadcasting DebuggerRestarted
//	StartDebugger();

	itsProgramConfigFileName.Clear();
	itsMainClassName.Clear();
	itsJVMExecArgs.Clear();
	itsSourcePathList->DeleteAll();

	// delete all directories in itsJarPathList

	JString fullName;
	if (!JConvertToAbsolutePath(fileName, NULL, &fullName) ||
		!JFileReadable(fullName))
		{
		const JString error = JGetString("ConfigFileUnreadable::JVMLink");
		Broadcast(UserOutput(error, kJTrue));
		return;
		}
	else if (CMMDIServer::IsBinary(fullName))
		{
		const JString error = JGetString("ConfigFileIsBinary::JVMLink");
		Broadcast(UserOutput(error, kJTrue));
		return;
		}

	JString line;
	if (!CMMDIServer::GetLanguage(fullName, &line) ||
		JStringCompare(line, "java", kJFalse) != 0)
		{
		const JString error = JGetString("ConfigFileWrongLanguage::JVMLink");
		Broadcast(UserOutput(error, kJTrue));
		return;
		}

	JString path, name;
	JSplitPathAndName(fullName, &path, &name);
	itsJVMCWD = path;

	itsProgramConfigFileName = fullName;

	ifstream input(fullName);
	while (1)
		{
		line = JReadLine(input);
		line.TrimWhitespace();

		if (line.BeginsWith("main-class:"))
			{
			line.RemoveSubstring(1, 11);
			line.TrimWhitespace();
			itsMainClassName = line;
			}
		else if (line.BeginsWith("java-path:"))
			{
			line.RemoveSubstring(1, 10);
			line.TrimWhitespace();
			itsJVMCWD = line;
			}
		else if (line.BeginsWith("java-exec:"))
			{
			line.RemoveSubstring(1, 10);
			line.TrimWhitespace();
			itsJVMExecArgs = line;
			}
		else if (line.BeginsWith("source-path:"))
			{
			line.RemoveSubstring(1, 12);
			line.TrimWhitespace();

			name = JCombinePathAndName(path, line);
			itsSourcePathList->Append(name);
			}
		else if (line.BeginsWith("source-jar:"))
			{
			line.RemoveSubstring(1, 11);
			line.TrimWhitespace();
			// unpack to /tmp
			// add to itsJarPathList (for deletion)
			// add to itsSourcePathList
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

	if (itsJVMExecArgs.IsEmpty())
		{
		itsJVMExecArgs = itsMainClassName;
		}

	JVMSetProgramTask* task = new JVMSetProgramTask();
	assert( task != NULL );
	task->Go();
}

/******************************************************************************
 BroadcastProgramSet

 *****************************************************************************/

void
JVMLink::BroadcastProgramSet()
{
	JString programName;
	GetProgram(&programName);

	Broadcast(SymbolsLoaded(kJFalse, programName));
}

/******************************************************************************
 ReloadProgram

 *****************************************************************************/

void
JVMLink::ReloadProgram()
{
	SetProgram(itsProgramConfigFileName);
}

/******************************************************************************
 SetCore

 *****************************************************************************/

void
JVMLink::SetCore
	(
	const JCharacter* fullName
	)
{
}

/******************************************************************************
 AttachToProcess

 *****************************************************************************/

void
JVMLink::AttachToProcess
	(
	const pid_t pid
	)
{
}

/******************************************************************************
 RunProgram

 *****************************************************************************/

void
JVMLink::RunProgram
	(
	const JCharacter* args
	)
{
	DetachOrKill();

	itsJVMCmd         = (CMGetPrefsManager())->GetJVMCommand();
	itsJVMProcessArgs = args;

	JString cmd = itsJVMCmd;

	if (strstr(itsJVMCmd,      "-agentlib:jdwp") == NULL &&
		strstr(itsJVMExecArgs, "-agentlib:jdwp") == NULL &&
		strstr(itsJVMCmd,      "-Xrunjdwp")      == NULL &&
		strstr(itsJVMExecArgs, "-Xrunjdwp")      == NULL)
		{
		cmd += " -agentlib:jdwp=transport=dt_socket,address=localhost:";
		cmd += JString(kJavaPort, JString::kBase10);
		}

	cmd += " ";
	cmd += itsJVMExecArgs;
	cmd += " ";
	cmd += itsJVMProcessArgs;

	int toFD, fromFD;
	const JError err = JProcess::Create(&itsProcess, itsJVMCWD, cmd,
										kJCreatePipe, &toFD,
										kJCreatePipe, &fromFD,
										kJAttachToFromFD, NULL);
	if (err.OK())
		{
		ListenTo(itsProcess);
		SetProcessConnection(toFD, fromFD);
		}
	else
		{
		err.ReportIfError();
		}
}

/******************************************************************************
 GetBreakpointManager

 *****************************************************************************/

CMBreakpointManager*
JVMLink::GetBreakpointManager()
{
	return itsBPMgr;
}

/******************************************************************************
 ShowBreakpointInfo

 *****************************************************************************/

void
JVMLink::ShowBreakpointInfo
	(
	const JIndex debuggerIndex
	)
{
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
JVMLink::SetBreakpoint
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex,
	const JBoolean		temporary
	)
{
	JString data;
	data += 0x02;			// breakpoint
	data += 0x02;			// all
	data += 0x01;			// 1 condition
		data += 0x07;		// LocationOnly
			data += 0x01;	// CLASS
			// classID
			// methodID
			// code index (8 bytes)

//	itsDebugLink->Send(GetNextTransactionID(), kEventRequestCmdSet, kERSetCmd,
//					   (unsigned char*) data.GetCString(), data.GetLength());
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
JVMLink::SetBreakpoint
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
JVMLink::RemoveBreakpoint
	(
	const JIndex debuggerIndex
	)
{
}

/******************************************************************************
 RemoveAllBreakpointsOnLine

 *****************************************************************************/

void
JVMLink::RemoveAllBreakpointsOnLine
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
}

/******************************************************************************
 RemoveAllBreakpointsAtAddress

 *****************************************************************************/

void
JVMLink::RemoveAllBreakpointsAtAddress
	(
	const JCharacter* addr
	)
{
}

/******************************************************************************
 RemoveAllBreakpoints

 *****************************************************************************/

void
JVMLink::RemoveAllBreakpoints()
{
	itsDebugLink->Send(GetNextTransactionID(), kEventRequestCmdSet, kERClearAllBreakpointsCmd, NULL, 0);
}

/******************************************************************************
 SetBreakpointEnabled

 *****************************************************************************/

void
JVMLink::SetBreakpointEnabled
	(
	const JIndex	debuggerIndex,
	const JBoolean	enabled,
	const JBoolean	once
	)
{
}

/******************************************************************************
 SetBreakpointCondition

 *****************************************************************************/

void
JVMLink::SetBreakpointCondition
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
JVMLink::RemoveBreakpointCondition
	(
	const JIndex debuggerIndex
	)
{
}

/******************************************************************************
 SetBreakpointIgnoreCount

 *****************************************************************************/

void
JVMLink::SetBreakpointIgnoreCount
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
JVMLink::WatchExpression
	(
	const JCharacter* expr
	)
{
}

/******************************************************************************
 WatchLocation

 *****************************************************************************/

void
JVMLink::WatchLocation
	(
	const JCharacter* expr
	)
{
}

/******************************************************************************
 SwitchToThread

 *****************************************************************************/

void
JVMLink::SwitchToThread
	(
	const JUInt64 id
	)
{
	if (id != itsCurrentThreadID)
		{
		itsCurrentThreadID = id;
		Broadcast(ThreadChanged());
		}
}

/******************************************************************************
 SwitchToFrame

 *****************************************************************************/

void
JVMLink::SwitchToFrame
	(
	const JUInt64 id
	)
{
	JIndex i;
	if (GetFrame(id, &i))
		{
		FrameInfo info = itsFrameList->GetElement(i);

		JString fullName;
		if (GetClassSourceFile(info.classID, &fullName))
			{
			Broadcast(ProgramStopped(CMLocation(fullName, 1)));
			}
		}
}

/******************************************************************************
 StepOver

 *****************************************************************************/

void
JVMLink::StepOver()
{
}

/******************************************************************************
 StepInto

 *****************************************************************************/

void
JVMLink::StepInto()
{
}

/******************************************************************************
 StepOut

 *****************************************************************************/

void
JVMLink::StepOut()
{
}

/******************************************************************************
 Continue

 *****************************************************************************/

void
JVMLink::Continue()
{
	itsProgramIsStoppedFlag = kJFalse;
	itsDebugLink->Send(GetNextTransactionID(), kVirtualMachineCmdSet, kVMResumeCmd, NULL, 0);
	Broadcast(ProgramRunning());
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
JVMLink::RunUntil
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
JVMLink::SetExecutionPoint
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
JVMLink::SetValue
	(
	const JCharacter* name,
	const JCharacter* value
	)
{
}

/******************************************************************************
 CreateArray2DCommand

 *****************************************************************************/

CMArray2DCommand*
JVMLink::CreateArray2DCommand
	(
	CMArray2DDir*		dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
{
	CMArray2DCommand* cmd = new JVMArray2DCommand(dir, table, data);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreatePlot2DCommand

 *****************************************************************************/

CMPlot2DCommand*
JVMLink::CreatePlot2DCommand
	(
	CMPlot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
{
	CMPlot2DCommand* cmd = new JVMPlot2DCommand(dir, x, y);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateDisplaySourceForMain

 *****************************************************************************/

CMDisplaySourceForMain*
JVMLink::CreateDisplaySourceForMain
	(
	CMSourceDirector* sourceDir
	)
{
	CMDisplaySourceForMain* cmd = new JVMDisplaySourceForMain(sourceDir);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetCompletions

 *****************************************************************************/

CMGetCompletions*
JVMLink::CreateGetCompletions
	(
	CMCommandInput*	input,
	CMCommandOutputDisplay*	history
	)
{
	CMGetCompletions* cmd = new JVMGetCompletions(input, history);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetFrame

 *****************************************************************************/

CMGetFrame*
JVMLink::CreateGetFrame
	(
	CMStackWidget* widget
	)
{
	CMGetFrame* cmd = new JVMGetFrame(widget);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetStack

 *****************************************************************************/

CMGetStack*
JVMLink::CreateGetStack
	(
	JTree*			tree,
	CMStackWidget*	widget
	)
{
	CMGetStack* cmd = new JVMGetStack(tree, widget);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetThread

 *****************************************************************************/

CMGetThread*
JVMLink::CreateGetThread
	(
	CMThreadsWidget* widget
	)
{
	CMGetThread* cmd = new JVMGetThread(widget);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetThreads

 *****************************************************************************/

CMGetThreads*
JVMLink::CreateGetThreads
	(
	JTree*				tree,
	CMThreadsWidget*	widget
	)
{
	CMGetThreads* cmd = new JVMGetThreads(tree, widget);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetFullPath

 *****************************************************************************/

CMGetFullPath*
JVMLink::CreateGetFullPath
	(
	const JCharacter*	fileName,
	const JIndex		lineIndex
	)
{
	CMGetFullPath* cmd = new JVMGetFullPath(fileName, lineIndex);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetInitArgs

 *****************************************************************************/

CMGetInitArgs*
JVMLink::CreateGetInitArgs
	(
	JXInputField* argInput
	)
{
	CMGetInitArgs* cmd = new JVMGetInitArgs(argInput);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetLocalVars

 *****************************************************************************/

CMGetLocalVars*
JVMLink::CreateGetLocalVars
	(
	CMVarNode* rootNode
	)
{
	CMGetLocalVars* cmd = new JVMGetLocalVars(rootNode);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateGetSourceFileList

 *****************************************************************************/

CMGetSourceFileList*
JVMLink::CreateGetSourceFileList
	(
	CMFileListDir* fileList
	)
{
	CMGetSourceFileList* cmd = new JVMGetSourceFileList(fileList);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateVarValueCommand

 *****************************************************************************/

CMVarCommand*
JVMLink::CreateVarValueCommand
	(
	const JCharacter* expr
	)
{
	JString s = "print ";
	s        += expr;

	CMVarCommand* cmd = new JVMVarCommand(s);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateVarContentCommand

 *****************************************************************************/

CMVarCommand*
JVMLink::CreateVarContentCommand
	(
	const JCharacter* expr
	)
{
	JString s = "print *(";
	s        += expr;
	s        += ")";

	CMVarCommand* cmd = new JVMVarCommand(s);
	assert( cmd != NULL );
	return cmd;
}

/******************************************************************************
 CreateVarNode

 *****************************************************************************/

CMVarNode*
JVMLink::CreateVarNode
	(
	const JBoolean shouldUpdate		// kJFalse for Local Variables
	)
{
	CMVarNode* node = new JVMVarNode(shouldUpdate);
	assert( node != NULL );
	return node;
}

CMVarNode*
JVMLink::CreateVarNode
	(
	JTreeNode*			parent,
	const JCharacter*	name,
	const JCharacter*	fullName,
	const JCharacter*	value
	)
{
	CMVarNode* node = new JVMVarNode(parent, name, fullName, value);
	assert( node != NULL );
	return node;
}

/******************************************************************************
 Build1DArrayExpression

 *****************************************************************************/

JString
JVMLink::Build1DArrayExpression
	(
	const JCharacter*	expr,
	const JInteger		index
	)
{
	return Build1DArrayExpressionForCFamilyLanguage(expr, index);
}

/******************************************************************************
 Build2DArrayExpression

 *****************************************************************************/

JString
JVMLink::Build2DArrayExpression
	(
	const JCharacter*	expr,
	const JInteger		rowIndex,
	const JInteger		colIndex
	)
{
	return Build2DArrayExpressionForCFamilyLanguage(expr, rowIndex, colIndex);
}

/******************************************************************************
 CreateGetMemory

 *****************************************************************************/

CMGetMemory*
JVMLink::CreateGetMemory
	(
	CMMemoryDir* dir
	)
{
	return NULL;
}

/******************************************************************************
 CreateGetAssembly

 *****************************************************************************/

CMGetAssembly*
JVMLink::CreateGetAssembly
	(
	CMSourceDirector* dir
	)
{
	return NULL;
}

/******************************************************************************
 CreateGetRegisters

 *****************************************************************************/

CMGetRegisters*
JVMLink::CreateGetRegisters
	(
	CMRegistersDir* dir
	)
{
	return NULL;
}

/******************************************************************************
 SendRaw

	Sends the given text to the process being debugged.

 *****************************************************************************/

void
JVMLink::SendRaw
	(
	const JCharacter* text
	)
{
	if (itsOutputLink != NULL)
		{
		itsOutputLink->Write(text);
		}
}

/******************************************************************************
 Send

	Process NOP immediately.

 *****************************************************************************/

JBoolean
JVMLink::Send
	(
	CMCommand* command
	)
{
	if (command->GetCommand() == "NOP")
		{
		command->SetTransactionID(GetNextTransactionID());
		SendMedicCommand(command);
		return kJTrue;
		}
	else
		{
		return CMLink::Send(command);
		}
}

/******************************************************************************
 SendMedicCommand (virtual protected)

 *****************************************************************************/

void
JVMLink::SendMedicCommand
	(
	CMCommand* command
	)
{
	command->Starting();

	if (command->GetCommand() == "NOP")
		{
		command->Finished(kJTrue);
		Cancel(command);

		if (!HasForegroundCommands())
			{
			RunNextCommand();
			}
		}
}

/******************************************************************************
 Send

 ******************************************************************************/

void
JVMLink::Send
	(
	const CMCommand*		command,
	const JIndex			cmdSet,
	const JIndex			cmd,
	const unsigned char*	data,
	const JSize				count
	)
{
	if (itsDebugLink != NULL)
		{
		itsDebugLink->Send(command->GetTransactionID(), cmdSet, cmd, data, count);
		}
}

/******************************************************************************
 ProgramFinished1 (private)

	It would be nice to detect "program finished" by *only* listening to
	itsProcess, but this doesn't work for remote debugging.

 *****************************************************************************/

void
JVMLink::ProgramFinished1
	(
	const JProcess::Finished* info
	)
{
	if (IsDebugging())
		{
		StopDebugger();

		JString reasonStr;
		if (info != NULL)
			{
			int result;
			const JChildExitReason reason = info->GetReason(&result);
			reasonStr = JPrintChildExitReason(reason, result);
			}
		else
			{
			reasonStr = JGetString("ProgramFinished::JVMLink");
			}
		reasonStr += "\n\n";

		Broadcast(UserOutput(reasonStr, kJFalse));
		Broadcast(ProgramFinished());

		StartDebugger();
		}
	else
		{
		delete itsProcess;
		itsProcess = NULL;
		}
}

/******************************************************************************
 StopProgram

 *****************************************************************************/

void
JVMLink::StopProgram()
{
	itsProgramIsStoppedFlag = kJTrue;
	itsDebugLink->Send(GetNextTransactionID(), kVirtualMachineCmdSet, kVMSuspendCmd, NULL, 0);
	Broadcast(ProgramStopped(CMLocation("", 1)));
}

/******************************************************************************
 KillProgram

 *****************************************************************************/

void
JVMLink::KillProgram()
{
	if (itsProcess != NULL)
		{
		itsProcess->Kill();
		}
	else if (itsDebugLink != NULL)
		{
		unsigned char data[4];
		JVMSocket::Pack4(1, data);
		itsDebugLink->Send(GetNextTransactionID(), kVirtualMachineCmdSet, kVMExitCmd, data, sizeof(data));

		itsJVMDeathTask = new JXTimerTask(1000, kJTrue);
		assert( itsJVMDeathTask != NULL );
		itsJVMDeathTask->Start();
		ListenTo(itsJVMDeathTask);
		}
}

/******************************************************************************
 DetachOrKill (private)

 *****************************************************************************/

void
JVMLink::DetachOrKill()
{
	KillProgram();

	DeleteProcessLink();	// doesn't hurt to do it always
	DeleteDebugLink();
}

/******************************************************************************
 OKToDetachOrKill

 *****************************************************************************/

JBoolean
JVMLink::OKToDetachOrKill()
	const
{
	if (itsProcess != NULL)
		{
		return (JGetUserNotification())->AskUserYes(JGetString("WarnKillProgram::JVMLink"));
		}
	else if (itsDebugLink != NULL)
		{
		return (JGetUserNotification())->AskUserYes(JGetString("WarnDetachProgram::JVMLink"));
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 InitDebugger

 *****************************************************************************/

void
JVMLink::InitDebugger()
{
	itsInitFinishedFlag = kJTrue;
}

/******************************************************************************
 ChangeDebugger

	This actually changes the JVM command for launching programs.

 *****************************************************************************/

JBoolean
JVMLink::ChangeDebugger()
{
	CMPrefsManager* mgr = CMGetPrefsManager();
	if (itsJVMCmd != mgr->GetJVMCommand() && itsProcess != NULL)
		{
		const JBoolean ok = RestartDebugger();
		if (ok)
			{
			RunProgram(itsJVMProcessArgs);
			}

		return ok;
		}

	return kJTrue;
}

/******************************************************************************
 RestartDebugger

 *****************************************************************************/

JBoolean
JVMLink::RestartDebugger()
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
JVMLink::StopDebugger()
{
	StopListening(itsThreadTree);

	DetachOrKill();
	CancelAllCommands();

	InitFlags();
}

/******************************************************************************
 ConnectionEstablished

 *****************************************************************************/

void
JVMLink::ConnectionEstablished
	(
	JVMSocket* socket
	)
{
	InitFlags();
	itsProgramIsStoppedFlag = kJTrue;

	itsDebugLink = socket;
	ListenTo(itsDebugLink);

	itsAcceptor->close();

	CMCommand* cmd = new JVMGetIDSizes();
	assert( cmd != NULL );

	// listen for class unload

	unsigned char data[ 1+1+4 ];
	data[0] = kClassUnloadEvent;
	data[1] = kSuspendNone;
	JVMSocket::Pack4(0, data+2);

	itsDebugLink->Send(GetNextTransactionID(), kEventRequestCmdSet, kERSetCmd, data, sizeof(data));

	// listen for thread creation/death

	data[0] = kThreadStartEvent;
	itsDebugLink->Send(GetNextTransactionID(), kEventRequestCmdSet, kERSetCmd, data, sizeof(data));

	data[0] = kThreadDeathEvent;
	itsDebugLink->Send(GetNextTransactionID(), kEventRequestCmdSet, kERSetCmd, data, sizeof(data));

	// build initial tree of threads

	StopListening(itsThreadTree);
	itsThreadList->CleanOut();
	itsThreadRoot->DeleteAllChildren();
	ListenTo(itsThreadTree);

	cmd = new JVMGetThreadGroups(itsThreadRoot, NULL);
	assert( cmd != NULL );

	// trigger commands

	JString programName;
	GetProgram(&programName);

	Broadcast(DebuggerReadyForInput());
	Broadcast(UserOutput(JGetString("Connected::JVMLink"), kJFalse));
	Broadcast(SymbolsLoaded(kJTrue, programName));
	Broadcast(ProgramStopped(CMLocation("", 1)));

	// show main()

	if (!itsMainClassName.IsEmpty())
		{
		JString fullName = ClassNameToResourcePath(itsMainClassName);
		if (ClassSignatureToFile(fullName, &fullName))
			{
			((CMGetCommandDirector())->GetCurrentSourceDir())->DisplayFile(fullName);
			}
		}
}

/******************************************************************************
 ConnectionFinished

 *****************************************************************************/

void
JVMLink::ConnectionFinished
	(
	JVMSocket* socket
	)
{
	assert( socket == itsDebugLink );

	itsDebugLink = NULL;
	DeleteProcessLink();

	RestartDebugger();
}

/******************************************************************************
 StartDebugger (private)

 *****************************************************************************/

// This function has to be last so JCore::new works for everything else.

#undef new

JBoolean
JVMLink::StartDebugger()
{
	if (itsAcceptor == NULL)
		{
		itsAcceptor = new JVMAcceptor;
		assert( itsAcceptor != NULL );
		}

	const JString portStr(kJavaPort, JString::kBase10);
	if (itsAcceptor->open(ACE_INET_Addr(kJavaPort)) == -1)
		{
		const JString errStr(jerrno(), JString::kBase10);

		const JCharacter* map[] =
			{
			"port",  portStr,
			"errno", errStr
			};
		JString msg = JGetString("ListenError::JVMLink", map, sizeof(map));

		JVMWelcomeTask* task = new JVMWelcomeTask(msg, kJTrue);
		assert( task != NULL );
		task->Go();
		return kJFalse;
		}
	else
		{
		const JCharacter* map[] =
			{
			"port", portStr
			};
		JString msg = JGetString("Welcome::JVMLink", map, sizeof(map));

		JVMWelcomeTask* task = new JVMWelcomeTask(msg, kJFalse);
		assert( task != NULL );
		task->Go();
		return kJTrue;
		}
}

void
JVMLink::SetProcessConnection
	(
	const int toFD,
	const int fromFD
	)
{
	itsOutputLink = new ProcessLink(toFD);
	assert( itsOutputLink != NULL );

	itsInputLink = new ProcessLink(fromFD);
	assert( itsInputLink != NULL );
	ListenTo(itsInputLink);
}

/******************************************************************************
 Delete*Link (private)

 ******************************************************************************/

// This function has to be last so JCore::delete works for everything else.

#undef delete

void
JVMLink::DeleteAcceptor()
{
	delete itsAcceptor;
	itsAcceptor = NULL;
}

void
JVMLink::DeleteDebugLink()
{
	delete itsDebugLink;
	itsDebugLink = NULL;

	FlushClassList();
}

void
JVMLink::DeleteProcessLink()
{
	delete itsOutputLink;
	itsOutputLink = NULL;

	delete itsInputLink;
	itsInputLink = NULL;
}
