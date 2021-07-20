/******************************************************************************
 JVMLink.cpp

	Interface to JVM debugging agent.

	BASE CLASS = CMLink

	Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

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
#include <JTreeNode.h>
#include <JStringIterator.h>
#include <JTree.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jErrno.h>
#include <jAssert.h>

const JIndex kJavaPort = 9001;

static const bool kFeatures[]=
{
	true,		// kSetProgram
	true,		// kSetArgs
	false,	// kSetCore
	false,	// kSetProcess
	true,		// kRunProgram
	true,		// kStopProgram
	false,	// kSetExecutionPoint
	false,	// kExecuteBackwards
	true,		// kShowBreakpointInfo
	false,	// kSetBreakpointCondition
	true,		// kSetBreakpointIgnoreCount
	false,	// kWatchExpression
	false,	// kWatchLocation
	false,	// kExamineMemory
	false,	// kDisassembleMemory
};

// JBroadcaster message types

const JUtf8Byte* JVMLink::kIDResolved = "IDResolved::JVMLink";

/******************************************************************************
 Constructor

 *****************************************************************************/

JVMLink::JVMLink()
	:
	CMLink(kFeatures),
	itsAcceptor(nullptr),
	itsDebugLink(nullptr),
	itsProcess(nullptr),
	itsOutputLink(nullptr),
	itsInputLink(nullptr),
	itsLatestMsg(nullptr),
	itsJVMDeathTask(nullptr)
{
	InitFlags();

	itsBPMgr = jnew JVMBreakpointManager(this);
	assert( itsBPMgr != nullptr );

	itsSourcePathList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsSourcePathList != nullptr );

	itsClassByIDList = jnew JArray<ClassInfo>;
	assert( itsClassByIDList != nullptr );
	itsClassByIDList->SetCompareFunction(CompareClassIDs);

	itsClassByNameList = jnew JAliasArray<ClassInfo>(itsClassByIDList, CompareClassNames, JListT::kSortAscending);
	assert( itsClassByNameList != nullptr );

	itsThreadRoot = jnew JVMThreadNode(JVMThreadNode::kGroupType, JVMThreadNode::kRootThreadGroupID);
	assert( itsThreadRoot != nullptr );

	itsThreadTree = jnew JTree(itsThreadRoot);
	assert( itsThreadTree != nullptr );

	itsThreadList = jnew JPtrArray<JVMThreadNode>(JPtrArrayT::kForgetAll);
	assert( itsThreadList != nullptr );
	itsThreadList->SetCompareFunction(JVMThreadNode::CompareID);

	itsCullThreadGroupsTask = jnew JXTimerTask(10000);
	assert( itsCullThreadGroupsTask != nullptr );
	itsCullThreadGroupsTask->Start();
	ListenTo(itsCullThreadGroupsTask);
	itsCullThreadGroupIndex = 1;

	itsFrameList = jnew JArray<FrameInfo>();
	assert( itsFrameList != nullptr );

	StartDebugger();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JVMLink::~JVMLink()
{
	StopDebugger();

	jdelete itsAcceptor;
	jdelete itsBPMgr;
	jdelete itsSourcePathList;
	jdelete itsClassByIDList;
	jdelete itsClassByNameList;
	jdelete itsThreadTree;
	jdelete itsThreadList;
	jdelete itsCullThreadGroupsTask;
	jdelete itsFrameList;
	jdelete itsJVMDeathTask;

	DeleteOneShotCommands();
}

/******************************************************************************
 InitFlags (private)

 *****************************************************************************/

void
JVMLink::InitFlags()
{
	itsInitFinishedFlag     = false;
	itsProgramIsStoppedFlag = false;
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

bool
JVMLink::DebuggerHasStarted()
	const
{
	return true;
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

bool
JVMLink::HasProgram()
	const
{
	return !itsJVMExecArgs.IsEmpty() || itsDebugLink != nullptr;
}

/******************************************************************************
 GetProgram

 ******************************************************************************/

bool
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

		JStringIterator iter(fullName, kJIteratorStartAtEnd);
		if (iter.Prev("."))
			{
			iter.SkipNext();
			iter.RemoveAllPrev();
			}
		}

	return true;
}

/******************************************************************************
 HasCore

 ******************************************************************************/

bool
JVMLink::HasCore()
	const
{
	return false;
}

/******************************************************************************
 GetCore

 ******************************************************************************/

bool
JVMLink::GetCore
	(
	JString* fullName
	)
	const
{
	fullName->Clear();
	return false;
}

/******************************************************************************
 HasLoadedSymbols

 ******************************************************************************/

bool
JVMLink::HasLoadedSymbols()
	const
{
	return itsDebugLink != nullptr;
}

/******************************************************************************
 IsDebugging

 *****************************************************************************/

bool
JVMLink::IsDebugging()
	const
{
	return itsDebugLink != nullptr;
}

/******************************************************************************
 ProgramIsRunning

 *****************************************************************************/

bool
JVMLink::ProgramIsRunning()
	const
{
	return itsDebugLink != nullptr && !itsProgramIsStoppedFlag;
}

/******************************************************************************
 ProgramIsStopped

 *****************************************************************************/

bool
JVMLink::ProgramIsStopped()
	const
{
	return itsDebugLink != nullptr && itsProgramIsStoppedFlag;
}

/******************************************************************************
 OKToSendCommands

 *****************************************************************************/

bool
JVMLink::OKToSendCommands
	(
	const bool background
	)
	const
{
	return true;
}

/******************************************************************************
 IsDefiningScript

	This forces user's typing to be echoed directly.

 *****************************************************************************/

bool
JVMLink::IsDefiningScript()
	const
{
	return true;
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
		const auto* info =
			dynamic_cast<const JVMSocket::MessageReady*>(&message);
		assert( info != nullptr );
		ReceiveMessageFromJVM(*info);
		}
	else if (sender == itsInputLink && message.Is(CMPipeT::kReadReady))
		{
		ReadFromProcess();
		}

	else if (sender == itsProcess && message.Is(JProcess::kFinished))
		{
		const auto* info =
			dynamic_cast<const JProcess::Finished*>(&message);
		assert( info != nullptr );
		ProgramFinished1(info);

		jdelete itsJVMDeathTask;
		itsJVMDeathTask = nullptr;

		jdelete itsProcess;
		itsProcess = nullptr;
		}
	else if (sender == itsJVMDeathTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		ProgramFinished1(nullptr);
		itsJVMDeathTask = nullptr;
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

			cmd->Finished(info.GetErrorCode() == 0);

			itsLatestMsg = nullptr;

			SetRunningCommand(nullptr);
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
			itsJVMDeathTask = jnew JXTimerTask(1000, true);
			assert( itsJVMDeathTask != nullptr );
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
			if (itsClassByNameList->SearchSorted(info, JListT::kAnyMatch, &j))
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
				node = jnew JVMThreadNode(JVMThreadNode::kThreadType, threadID);
				assert( node != nullptr );
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
				jdelete node;
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
	log << "field id size: " << itsFieldIDSize << std::endl;
	log << "method id size: " << itsFieldIDSize << std::endl;
	log << "object id size: " << itsFieldIDSize << std::endl;
	log << "reference type id size: " << itsFieldIDSize << std::endl;
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

bool
JVMLink::FindThread
	(
	const JUInt64	id,
	JVMThreadNode**	node
	)
	const
{
	JVMThreadNode target(id);
	JIndex i;
	if (itsThreadList->SearchSorted(&target, JListT::kAnyMatch, &i))
		{
		*node = itsThreadList->GetElement(i);
		return true;
		}
	else
		{
		*node = nullptr;
		return false;
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
	while (true)
		{
		node = itsThreadList->GetElement(itsCullThreadGroupIndex);
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

	CMCommand* cmd = jnew JVMGetThreadParent(node, true);
	assert( cmd != nullptr );

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
	const JUInt64	id,
	const JString&	signature
	)
{
	ClassInfo info;
	info.id = id;

	JIndex i;
	if (itsClassByIDList->SearchSorted(info, JListT::kAnyMatch, &i))
		{
		return;
		}

	info.name = jnew JString(ClassSignatureToName(signature));
	assert( info.name != nullptr );

	JString path;
	if (ClassSignatureToFile(signature, &path))
		{
		info.path = jnew JString(path);
		assert( info.path != nullptr );
		}
	else
		{
		info.path = nullptr;
		}

	info.methods = jnew JArray<MethodInfo>();
	assert( info.methods != nullptr );
	info.methods->SetCompareFunction(CompareMethodIDs);

	itsClassByIDList->InsertSorted(info);

	std::ostringstream log;
	log << "AddClass: " << id << ' ' << signature;
	Log(log);

	Broadcast(IDResolved(id));

	CMCommand* cmd = jnew JVMGetClassMethods(id);
	assert( cmd != nullptr );
}

/******************************************************************************
 GetClassName

 *****************************************************************************/

bool
JVMLink::GetClassName
	(
	const JUInt64	id,
	JString*		name
	)
{
	ClassInfo target;
	target.id = id;
	JIndex i;
	if (itsClassByIDList->SearchSorted(target, JListT::kAnyMatch, &i))
		{
		target = itsClassByIDList->GetElement(i);
		*name  = *(target.name);
		return true;
		}
	else
		{
		CMCommand* cmd = jnew JVMGetClassInfo(id);
		assert( cmd != nullptr );

		name->Clear();
		return false;
		}
}

/******************************************************************************
 GetClassSourceFile

 *****************************************************************************/

bool
JVMLink::GetClassSourceFile
	(
	const JUInt64	id,
	JString*		fullName
	)
{
	ClassInfo target;
	target.id = id;
	JIndex i;
	if (itsClassByIDList->SearchSorted(target, JListT::kAnyMatch, &i))
		{
		target = itsClassByIDList->GetElement(i);
		if (target.path != nullptr)
			{
			*fullName = *(target.path);
			return true;
			}
		}

	fullName->Clear();
	return false;
}

/******************************************************************************
 AddMethod

 *****************************************************************************/

void
JVMLink::AddMethod
	(
	const JUInt64	classID,
	const JUInt64	methodID,
	const JString&	name
	)
{
	ClassInfo target;
	target.id = classID;
	JIndex i;
	const bool found = itsClassByIDList->SearchSorted(target, JListT::kAnyMatch, &i);
	assert( found );

	target = itsClassByIDList->GetElement(i);

	MethodInfo info;
	info.id = methodID;

	info.name = jnew JString(name);
	assert( info.name != nullptr );

	target.methods->InsertSorted(info);

	std::ostringstream log;
	log << "AddMethod: " << *(target.name) << ": " << methodID << ' ' << name;
	Log(log);

	Broadcast(IDResolved(methodID));
}

/******************************************************************************
 GetMethodName

 *****************************************************************************/

bool
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
	if (!itsClassByIDList->SearchSorted(target1, JListT::kAnyMatch, &i))
		{
		name->Clear();
		return false;
		}

	target1 = itsClassByIDList->GetElement(i);

	MethodInfo target2;
	target2.id = methodID;
	if (target1.methods->SearchSorted(target2, JListT::kAnyMatch, &i))
		{
		target2 = target1.methods->GetElement(i);
		*name   = *(target2.name);
		return true;
		}
	else
		{
		name->Clear();
		return false;
		}
}

/******************************************************************************
 ClassSignatureToResourcePath (static)

 ******************************************************************************/

JString
JVMLink::ClassSignatureToResourcePath
	(
	const JString& signature
	)
{
	JString path = signature;

	JStringIterator iter(&path);
	if (path.GetFirstCharacter() == 'L')
		{
		iter.RemoveNext();
		}

	if (iter.Next("$"))
		{
		iter.SkipPrev();
		iter.RemoveAllNext();
		}
	else if (path.GetLastCharacter() == ';')
		{
		iter.MoveTo(kJIteratorStartAtEnd, 0);
		iter.RemovePrev();
		}

	return path;
}

/******************************************************************************
 ClassSignatureToName (static)

 ******************************************************************************/

JString
JVMLink::ClassSignatureToName
	(
	const JString& signature
	)
{
	JString name = ClassSignatureToResourcePath(signature);

	JStringIterator iter(&name);
	while (iter.Next(ACE_DIRECTORY_SEPARATOR_STR))
		{
		iter.ReplaceLastMatch("."); 
		}

	return name;
}

/******************************************************************************
 ClassNameToResourcePath (static)

 ******************************************************************************/

JString
JVMLink::ClassNameToResourcePath
	(
	const JString& name
	)
{
	JString file = name;

	JStringIterator iter(&file);
	while (iter.Next("."))
		{
		iter.ReplaceLastMatch(ACE_DIRECTORY_SEPARATOR_STR); 
		}

	return file;
}

/******************************************************************************
 ClassSignatureToFile (private)

 ******************************************************************************/

bool
JVMLink::ClassSignatureToFile
	(
	const JString&	signature,
	JString*		fullName
	)
	const
{
	JString file = ClassSignatureToResourcePath(signature);
	file        += ".java";

	const JSize pathCount = itsSourcePathList->GetElementCount();
	for (JIndex i=1; i<=pathCount; i++)
		{
		const JString* path = itsSourcePathList->GetElement(i);
		*fullName           = JCombinePathAndName(*path, file);
		if (JFileReadable(*fullName))
			{
			return true;
			}
		}

	fullName->Clear();
	return false;
}

/******************************************************************************
 CompareClassIDs (private static)

 ******************************************************************************/

JListT::CompareResult
JVMLink::CompareClassIDs
	(
	const ClassInfo& c1,
	const ClassInfo& c2
	)
{
	if (c1.id > c2.id)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (c1.id < c2.id)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return JListT::kFirstEqualSecond;
		}
}

/******************************************************************************
 CompareClassNames (private static)

 ******************************************************************************/

JListT::CompareResult
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

JListT::CompareResult
JVMLink::CompareMethodIDs
	(
	const MethodInfo& m1,
	const MethodInfo& m2
	)
{
	if (m1.id > m2.id)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (m1.id < m2.id)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return JListT::kFirstEqualSecond;
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

bool
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
			return true;
			}
		}

	*index = 0;
	return false;
}

/******************************************************************************
 ReadFromProcess (private)

 *****************************************************************************/

void
JVMLink::ReadFromProcess()
{
	JString data;
	itsInputLink->Read(&data);
	Broadcast(UserOutput(data, false, true));
}

/******************************************************************************
 SetProgram

 *****************************************************************************/

void
JVMLink::SetProgram
	(
	const JString& fileName
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
	if (!JConvertToAbsolutePath(fileName, JString::empty, &fullName) ||
		!JFileReadable(fullName))
		{
		const JString error = JGetString("ConfigFileUnreadable::JVMLink");
		Broadcast(UserOutput(error, true));
		return;
		}
	else if (CMMDIServer::IsBinary(fullName))
		{
		const JString error = JGetString("ConfigFileIsBinary::JVMLink");
		Broadcast(UserOutput(error, true));
		return;
		}

	JString line;
	if (!CMMDIServer::GetLanguage(fullName, &line) ||
		JString::Compare(line, "java", JString::kIgnoreCase) != 0)
		{
		const JString error = JGetString("ConfigFileWrongLanguage::JVMLink");
		Broadcast(UserOutput(error, true));
		return;
		}

	JString path, name;
	JSplitPathAndName(fullName, &path, &name);
	itsJVMCWD = path;

	itsProgramConfigFileName = fullName;

	std::ifstream input(fullName.GetBytes());
	while (true)
		{
		line = JReadLine(input);
		line.TrimWhitespace();

		if (line.BeginsWith("main-class:"))
			{
			JStringIterator iter(&line);
			iter.RemoveNext(11);
			line.TrimWhitespace();
			itsMainClassName = line;
			}
		else if (line.BeginsWith("java-path:"))
			{
			JStringIterator iter(&line);
			iter.RemoveNext(10);
			line.TrimWhitespace();
			itsJVMCWD = line;
			}
		else if (line.BeginsWith("java-exec:"))
			{
			JStringIterator iter(&line);
			iter.RemoveNext(10);
			line.TrimWhitespace();
			itsJVMExecArgs = line;
			}
		else if (line.BeginsWith("source-path:"))
			{
			JStringIterator iter(&line);
			iter.RemoveNext(12);
			line.TrimWhitespace();

			name = JCombinePathAndName(path, line);
			itsSourcePathList->Append(name);
			}
		else if (line.BeginsWith("source-jar:"))
			{
			JStringIterator iter(&line);
			iter.RemoveNext(11);
			line.TrimWhitespace();
			// unpack to /tmp
			// add to itsJarPathList (for deletion)
			// add to itsSourcePathList
			}
		else if (!line.IsEmpty() && !line.BeginsWith("code-medic:"))
			{
			line.Prepend("Unknown option: ");
			line.Append("\n");
			Broadcast(UserOutput(line, true));
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

	auto* task = jnew JVMSetProgramTask();
	assert( task != nullptr );
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

	Broadcast(SymbolsLoaded(false, programName));
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
	const JString& fullName
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
	const JString& args
	)
{
	DetachOrKill();

	itsJVMCmd         = CMGetPrefsManager()->GetJVMCommand();
	itsJVMProcessArgs = args;

	JString cmd = itsJVMCmd;

	if (!itsJVMCmd.Contains("-agentlib:jdwp")      &&
		!itsJVMExecArgs.Contains("-agentlib:jdwp") &&
		!itsJVMCmd.Contains("-Xrunjdwp")           &&
		!itsJVMExecArgs.Contains("-Xrunjdwp"))
		{
		cmd += " -agentlib:jdwp=transport=dt_socket,address=localhost:";
		cmd += JString((JUInt64) kJavaPort);
		}

	cmd += " ";
	cmd += itsJVMExecArgs;
	cmd += " ";
	cmd += itsJVMProcessArgs;

	int toFD, fromFD;
	const JError err = JProcess::Create(&itsProcess, itsJVMCWD, cmd,
										kJCreatePipe, &toFD,
										kJCreatePipe, &fromFD,
										kJAttachToFromFD, nullptr);
	if (err.OK())
		{
		ListenTo(itsProcess);

		itsOutputLink = new ProcessLink(toFD);
		assert( itsOutputLink != nullptr );

		itsInputLink = new ProcessLink(fromFD);
		assert( itsInputLink != nullptr );
		ListenTo(itsInputLink);
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
	const JString&	fileName,
	const JIndex	lineIndex,
	const bool	temporary
	)
{
	JArray<unsigned char> data;
	data.AppendElement(0x02);			// breakpoint
	data.AppendElement(0x02);			// all
	data.AppendElement(0x01);			// 1 condition
		data.AppendElement(0x07);		// LocationOnly
			data.AppendElement(0x01);	// CLASS
			// classID
			// methodID
			// code index (8 bytes)

//	itsDebugLink->Send(GetNextTransactionID(), kEventRequestCmdSet, kERSetCmd,
//					   data.GetCArray(), data.GetElementCount());
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
JVMLink::SetBreakpoint
	(
	const JString&	address,
	const bool	temporary
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
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
}

/******************************************************************************
 RemoveAllBreakpointsAtAddress

 *****************************************************************************/

void
JVMLink::RemoveAllBreakpointsAtAddress
	(
	const JString& addr
	)
{
}

/******************************************************************************
 RemoveAllBreakpoints

 *****************************************************************************/

void
JVMLink::RemoveAllBreakpoints()
{
	itsDebugLink->Send(GetNextTransactionID(), kEventRequestCmdSet, kERClearAllBreakpointsCmd, nullptr, 0);
}

/******************************************************************************
 SetBreakpointEnabled

 *****************************************************************************/

void
JVMLink::SetBreakpointEnabled
	(
	const JIndex	debuggerIndex,
	const bool	enabled,
	const bool	once
	)
{
}

/******************************************************************************
 SetBreakpointCondition

 *****************************************************************************/

void
JVMLink::SetBreakpointCondition
	(
	const JIndex	debuggerIndex,
	const JString&	condition
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
	const JString& expr
	)
{
}

/******************************************************************************
 WatchLocation

 *****************************************************************************/

void
JVMLink::WatchLocation
	(
	const JString& expr
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
	itsProgramIsStoppedFlag = false;
	itsDebugLink->Send(GetNextTransactionID(), kVirtualMachineCmdSet, kVMResumeCmd, nullptr, 0);
	Broadcast(ProgramRunning());
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
JVMLink::RunUntil
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	SetBreakpoint(fileName, lineIndex, true);
	Continue();
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
JVMLink::SetExecutionPoint
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
}

/******************************************************************************
 SetValue

 *****************************************************************************/

void
JVMLink::SetValue
	(
	const JString& name,
	const JString& value
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
	CMArray2DCommand* cmd = jnew JVMArray2DCommand(dir, table, data);
	assert( cmd != nullptr );
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
	CMPlot2DCommand* cmd = jnew JVMPlot2DCommand(dir, x, y);
	assert( cmd != nullptr );
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
	CMDisplaySourceForMain* cmd = jnew JVMDisplaySourceForMain(sourceDir);
	assert( cmd != nullptr );
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
	CMGetCompletions* cmd = jnew JVMGetCompletions(input, history);
	assert( cmd != nullptr );
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
	CMGetFrame* cmd = jnew JVMGetFrame(widget);
	assert( cmd != nullptr );
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
	CMGetStack* cmd = jnew JVMGetStack(tree, widget);
	assert( cmd != nullptr );
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
	CMGetThread* cmd = jnew JVMGetThread(widget);
	assert( cmd != nullptr );
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
	CMGetThreads* cmd = jnew JVMGetThreads(tree, widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetFullPath

 *****************************************************************************/

CMGetFullPath*
JVMLink::CreateGetFullPath
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	CMGetFullPath* cmd = jnew JVMGetFullPath(fileName, lineIndex);
	assert( cmd != nullptr );
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
	CMGetInitArgs* cmd = jnew JVMGetInitArgs(argInput);
	assert( cmd != nullptr );
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
	CMGetLocalVars* cmd = jnew JVMGetLocalVars(rootNode);
	assert( cmd != nullptr );
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
	CMGetSourceFileList* cmd = jnew JVMGetSourceFileList(fileList);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarValueCommand

 *****************************************************************************/

CMVarCommand*
JVMLink::CreateVarValueCommand
	(
	const JString& expr
	)
{
	JString s("print ");
	s += expr;

	CMVarCommand* cmd = jnew JVMVarCommand(s);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarContentCommand

 *****************************************************************************/

CMVarCommand*
JVMLink::CreateVarContentCommand
	(
	const JString& expr
	)
{
	JString s("print *(");
	s += expr;
	s += ")";

	CMVarCommand* cmd = jnew JVMVarCommand(s);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarNode

 *****************************************************************************/

CMVarNode*
JVMLink::CreateVarNode
	(
	const bool shouldUpdate		// false for Local Variables
	)
{
	CMVarNode* node = jnew JVMVarNode(shouldUpdate);
	assert( node != nullptr );
	return node;
}

CMVarNode*
JVMLink::CreateVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	fullName,
	const JString&	value
	)
{
	CMVarNode* node = jnew JVMVarNode(parent, name, fullName, value);
	assert( node != nullptr );
	return node;
}

/******************************************************************************
 Build1DArrayExpression

 *****************************************************************************/

JString
JVMLink::Build1DArrayExpression
	(
	const JString&	expr,
	const JInteger	index
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
	const JString&	expr,
	const JInteger	rowIndex,
	const JInteger	colIndex
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
	return nullptr;
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
	return nullptr;
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
	return nullptr;
}

/******************************************************************************
 SendRaw

	Sends the given text to the process being debugged.

 *****************************************************************************/

void
JVMLink::SendRaw
	(
	const JString& text
	)
{
	if (itsOutputLink != nullptr)
		{
		itsOutputLink->Write(text);
		}
}

/******************************************************************************
 Send

	Process NOP immediately.

 *****************************************************************************/

bool
JVMLink::Send
	(
	CMCommand* command
	)
{
	if (command->GetCommand() == "NOP")
		{
		command->SetTransactionID(GetNextTransactionID());
		SendMedicCommand(command);
		return true;
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
		command->Finished(true);
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
	if (itsDebugLink != nullptr)
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
		if (info != nullptr)
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

		Broadcast(UserOutput(reasonStr, false));
		Broadcast(ProgramFinished());

		StartDebugger();
		}
	else
		{
		jdelete itsProcess;
		itsProcess = nullptr;
		}
}

/******************************************************************************
 StopProgram

 *****************************************************************************/

void
JVMLink::StopProgram()
{
	itsProgramIsStoppedFlag = true;
	itsDebugLink->Send(GetNextTransactionID(), kVirtualMachineCmdSet, kVMSuspendCmd, nullptr, 0);
	Broadcast(ProgramStopped(CMLocation(JString::empty, 1)));
}

/******************************************************************************
 KillProgram

 *****************************************************************************/

void
JVMLink::KillProgram()
{
	if (itsProcess != nullptr)
		{
		itsProcess->Kill();
		}
	else if (itsDebugLink != nullptr)
		{
		unsigned char data[4];
		JVMSocket::Pack4(1, data);
		itsDebugLink->Send(GetNextTransactionID(), kVirtualMachineCmdSet, kVMExitCmd, data, sizeof(data));

		itsJVMDeathTask = jnew JXTimerTask(1000, true);
		assert( itsJVMDeathTask != nullptr );
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

	jdelete itsDebugLink;
	itsDebugLink = nullptr;

	FlushClassList();
}

/******************************************************************************
 OKToDetachOrKill

 *****************************************************************************/

bool
JVMLink::OKToDetachOrKill()
	const
{
	if (itsProcess != nullptr)
		{
		return JGetUserNotification()->AskUserYes(JGetString("WarnKillProgram::JVMLink"));
		}
	else if (itsDebugLink != nullptr)
		{
		return JGetUserNotification()->AskUserYes(JGetString("WarnDetachProgram::JVMLink"));
		}
	else
		{
		return true;
		}
}

/******************************************************************************
 StartDebugger (private)

 *****************************************************************************/

bool
JVMLink::StartDebugger()
{
	if (itsAcceptor == nullptr)
		{
		itsAcceptor = jnew JVMAcceptor;
		assert( itsAcceptor != nullptr );
		}

	const JString portStr((JUInt64) kJavaPort);
	if (itsAcceptor->open(ACE_INET_Addr(kJavaPort)) == -1)
		{
		const JString errStr((JUInt64) jerrno());

		const JUtf8Byte* map[] =
			{
			"port",  portStr.GetBytes(),
			"errno", errStr.GetBytes()
			};
		const JString msg = JGetString("ListenError::JVMLink", map, sizeof(map));

		auto* task = jnew JVMWelcomeTask(msg, true);
		assert( task != nullptr );
		task->Go();
		return false;
		}
	else
		{
		const JUtf8Byte* map[] =
			{
			"port", portStr.GetBytes()
			};
		JString msg = JGetString("Welcome::JVMLink", map, sizeof(map));

		auto* task = jnew JVMWelcomeTask(msg, false);
		assert( task != nullptr );
		task->Go();
		return true;
		}
}

/******************************************************************************
 InitDebugger

 *****************************************************************************/

void
JVMLink::InitDebugger()
{
	itsInitFinishedFlag = true;
}

/******************************************************************************
 ChangeDebugger

	This actually changes the JVM command for launching programs.

 *****************************************************************************/

bool
JVMLink::ChangeDebugger()
{
	CMPrefsManager* mgr = CMGetPrefsManager();
	if (itsJVMCmd != mgr->GetJVMCommand() && itsProcess != nullptr)
		{
		const bool ok = RestartDebugger();
		if (ok)
			{
			RunProgram(itsJVMProcessArgs);
			}

		return ok;
		}

	return true;
}

/******************************************************************************
 RestartDebugger

 *****************************************************************************/

bool
JVMLink::RestartDebugger()
{
	StopDebugger();
	const bool ok = StartDebugger();

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
	itsProgramIsStoppedFlag = true;

	itsDebugLink = socket;
	ListenTo(itsDebugLink);

	itsAcceptor->close();

	CMCommand* cmd = jnew JVMGetIDSizes();
	assert( cmd != nullptr );

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

	cmd = jnew JVMGetThreadGroups(itsThreadRoot, nullptr);
	assert( cmd != nullptr );

	// trigger commands

	JString programName;
	GetProgram(&programName);

	Broadcast(DebuggerReadyForInput());
	Broadcast(UserOutput(JGetString("Connected::JVMLink"), false));
	Broadcast(SymbolsLoaded(true, programName));
	Broadcast(ProgramStopped(CMLocation(JString::empty, 1)));

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

	itsDebugLink = nullptr;
	DeleteProcessLink();

	RestartDebugger();
}

/******************************************************************************
 DeleteProcessLink (private)

 ******************************************************************************/

void
JVMLink::DeleteProcessLink()
{
	delete itsOutputLink;
	itsOutputLink = nullptr;

	delete itsInputLink;
	itsInputLink = nullptr;
}
