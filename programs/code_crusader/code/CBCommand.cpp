/******************************************************************************
 CBCommand.cpp

	Executes a sequence of commands from CBCommandManager.

	If multiple sequences of commands are to be executed, each sequence is
	delimited by a blank (nullptr) command.  This allows us to skip to the
	next complete sequence if a command returns an error.

	BASE CLASS = virtual JBroadcaster

	Copyright © 2002-2018 by John Lindal.

 ******************************************************************************/

#include "CBCommand.h"
#include "CBCommandManager.h"
#include "CBProjectDocument.h"
#include "CBCompileDocument.h"
#include "cbGlobals.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXDeleteObjectTask.h>
#include <JThisProcess.h>
#include <JSimpleProcess.h>
#include <jDirUtil.h>
#include <jAssert.h>

// JBroadcaster message types

const JUtf8Byte* CBCommand::kFinished = "Finished::CBCommand";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCommand::CBCommand
	(
	const JString&		path,
	const bool		refreshVCSStatusWhenFinished,
	const bool		beepWhenFinished,
	CBProjectDocument*	projDoc
	)
	:
	itsProjDoc(projDoc),
	itsCmdPath(path),
	itsOutputDoc(nullptr),
	itsBeepFlag(beepWhenFinished),
	itsRefreshVCSStatusFlag(refreshVCSStatusWhenFinished),
	itsUpdateSymbolDatabaseFlag(false),
	itsInQueueFlag(false),
	itsSuccessFlag(true),
	itsCancelledFlag(false),
	itsMakeDependCmd(nullptr),
	itsBuildOutputDoc(nullptr),
	itsRunOutputDoc(nullptr),
	itsParent(nullptr),
	itsCallParentProcessFinishedFlag(true)
{
	assert( JIsAbsolutePath(path) );

	itsCmdList = jnew JArray<CmdInfo>;
	assert( itsCmdList != nullptr );

	if (itsProjDoc != nullptr)
		{
		ClearWhenGoingAway(itsProjDoc, &itsProjDoc);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCommand::~CBCommand()
{
	if (itsParent != nullptr && itsParent->itsBuildOutputDoc == nullptr)
		{
		itsParent->itsBuildOutputDoc = itsBuildOutputDoc;
		}
	if (itsParent != nullptr && itsParent->itsRunOutputDoc == nullptr)
		{
		itsParent->itsRunOutputDoc = itsRunOutputDoc;
		}

	if (itsUpdateSymbolDatabaseFlag && itsSuccessFlag)
		{
		CBGetDocumentManager()->UpdateSymbolDatabases();
		}

	// only refresh VCS status when all finished, since may be expensive

	if (itsParent != nullptr && itsRefreshVCSStatusFlag)
		{
		itsParent->itsRefreshVCSStatusFlag = true;
		}
	else if (itsRefreshVCSStatusFlag)
		{
		CBGetDocumentManager()->RefreshVCSStatus();
		}

	if (itsParent != nullptr && itsCallParentProcessFinishedFlag)
		{
		itsParent->ProcessFinished(itsSuccessFlag, itsCancelledFlag);
		}
	Broadcast(Finished(itsSuccessFlag, itsCancelledFlag));

	const JSize count = itsCmdList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CmdInfo info = itsCmdList->GetElement(i);
		info.Free(true);
		}
	jdelete itsCmdList;

	if (itsParent == nullptr)
		{
		FinishWindow(&itsBuildOutputDoc);
		FinishWindow(&itsRunOutputDoc);
		}
}

/******************************************************************************
 DeleteThis (private)

 ******************************************************************************/

void
CBCommand::DeleteThis()
{
	JXDeleteObjectTask<CBCommand>::Delete(this);

	if (itsParent == nullptr)
		{
		FinishWindow(&itsBuildOutputDoc);
		FinishWindow(&itsRunOutputDoc);
		}
}

/******************************************************************************
 FinishWindow (private)

 ******************************************************************************/

void
CBCommand::FinishWindow
	(
	CBExecOutputDocument** doc
	)
{
	if (*doc != nullptr)
		{
		(**doc).DecrementUseCount();
		*doc = nullptr;
		}
}

/******************************************************************************
 Add

 ******************************************************************************/

bool
CBCommand::Add
	(
	const JPtrArray<JString>&	cmdArgs,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList,
	CBFunctionStack*			fnStack
	)
{
	const JString* firstArg = cmdArgs.GetFirstElement();
	if (firstArg->GetFirstCharacter() == '&')
		{
		assert( fnStack != nullptr );

		// check for re-used command name

		const JUtf8Byte* cmdName = firstArg->GetBytes()+1;

		const JSize cmdCount = fnStack->GetElementCount();
		for (JIndex j=1; j<=cmdCount; j++)
			{
			if (strcmp(cmdName, fnStack->Peek(j)) == 0)
				{
				ReportInfiniteLoop(*fnStack, j);
				return false;
				}
			}

		// prepare cmd for execution later

		fnStack->Push(cmdName);

		CBCommandManager* mgr =
			(itsProjDoc != nullptr ? itsProjDoc->GetCommandManager() : CBGetCommandManager());
		CBCommand* cmdObj;
		CBCommandManager::CmdInfo* cmdInfo;
		if (mgr->Prepare(JString(cmdName, JString::kNoCopy),
						 itsProjDoc, fullNameList, lineIndexList,
						 &cmdObj, &cmdInfo, fnStack))
			{
			cmdObj->SetParent(this);
			itsCmdList->AppendElement(CmdInfo(nullptr, cmdObj, cmdInfo, false));
			}
		else
			{
			return false;
			}

		fnStack->Pop();
		}
	else
		{
		auto* args = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert( args != nullptr );
		args->CopyObjects(cmdArgs, JPtrArrayT::kDeleteAll, false);

		itsCmdList->AppendElement(CmdInfo(args, nullptr, nullptr, false));
		}

	return true;
}

/******************************************************************************
 Add

 ******************************************************************************/

void
CBCommand::Add
	(
	CBCommand*							subCmd,
	const CBCommandManager::CmdInfo&	cmdInfo
	)
{
	subCmd->SetParent(this);

	auto* info = jnew CBCommandManager::CmdInfo;
	assert( info != nullptr );
	*info = cmdInfo.Copy();

	itsCmdList->AppendElement(CmdInfo(nullptr, subCmd, info, false));
}

/******************************************************************************
 ReportInfiniteLoop (private)

 ******************************************************************************/

void
CBCommand::ReportInfiniteLoop
	(
	const CBFunctionStack&	fnStack,
	const JIndex			startIndex
	)
{
	JString loop;
	for (JIndex i=startIndex; i>=1; i--)
		{
		if (!loop.IsEmpty())
			{
			loop += " \xE2\x86\x92 ";
			}
		loop += fnStack.Peek(i);
		}
	loop += " \xE2\x86\x92 ";
	loop += fnStack.Peek(startIndex);

	const JUtf8Byte* map[] =
		{
		"loop", loop.GetBytes()
		};
	const JString msg = JGetString("InfiniteLoop::CBCommand", map, sizeof(map));
	JGetUserNotification()->ReportError(msg);
}

/******************************************************************************
 MarkEndOfSequence

 ******************************************************************************/

void
CBCommand::MarkEndOfSequence()
{
	CmdInfo info;
	itsCmdList->AppendElement(info);
}

/******************************************************************************
 Start

	Returns false if it deletes us.

 ******************************************************************************/

bool
CBCommand::Start
	(
	const CBCommandManager::CmdInfo& info
	)
{
	CBCommandManager* mgr =
		(itsProjDoc != nullptr ? itsProjDoc->GetCommandManager() : CBGetCommandManager());

	if (info.isMake)
		{
		itsUpdateSymbolDatabaseFlag = true;

		CBCommand* p = itsParent;
		while (p != nullptr)
			{
			if (p->itsBuildOutputDoc != nullptr)
				{
				itsOutputDoc = p->itsBuildOutputDoc;
				break;
				}
			p = p->itsParent;
			}

		if (itsOutputDoc == nullptr)
			{
			itsOutputDoc = mgr->GetCompileDoc(itsProjDoc);
			itsOutputDoc->IncrementUseCount();
			}
		SetCompileDocStrings();

		itsBuildOutputDoc = itsOutputDoc;
		}
	else if (info.useWindow)
		{
		CBCommand* p = itsParent;
		while (p != nullptr)
			{
			if (p->itsRunOutputDoc != nullptr)
				{
				itsOutputDoc = p->itsRunOutputDoc;
				break;
				}
			p = p->itsParent;
			}

		if (itsOutputDoc == nullptr)
			{
			itsOutputDoc = mgr->GetOutputDoc();
			itsOutputDoc->IncrementUseCount();
			}
		itsDontCloseMsg = JGetString("RunCloseMsg::CBCommand");

		itsWindowTitle = JGetString("RunWindowTitlePrefix::CBCommand");
		if (!(info.menuText)->IsEmpty())
			{
			itsWindowTitle += *(info.menuText);
			}
		else
			{
			itsWindowTitle += *(info.cmd);
			}

		itsRunOutputDoc = itsOutputDoc;
		}

	if (info.saveAll)	// ok, now that we have decided that command can be executed
		{
		CBGetDocumentManager()->SaveTextDocuments(false);
		JWait(1.1);		// ensure timestamp is different if any other program modifies the files and then does --revert-all-saved
		}

	if (info.isVCS)
		{
		itsUpdateSymbolDatabaseFlag = true;
		}

	if (itsUpdateSymbolDatabaseFlag)
		{
		CBGetDocumentManager()->CancelUpdateSymbolDatabases();
		}

	// after saving all files, update Makefile

	bool waitForMakeDepend = false;
	if (info.isMake && itsProjDoc != nullptr)
		{
		waitForMakeDepend =
			(itsProjDoc->GetBuildManager())->UpdateMakefile(itsOutputDoc, &itsMakeDependCmd);
		}

	if (waitForMakeDepend)
		{
		if (itsMakeDependCmd != nullptr)
			{
			itsCmdList->PrependElement(CmdInfo(nullptr, itsMakeDependCmd, nullptr, true));
			ListenTo(itsMakeDependCmd);		// many may need to hear; can't use SetParent()
			return true;
			}
		else
			{
			DeleteThis();
			return false;
			}
		}
	else if (StartProcess())
		{
		if (itsOutputDoc != nullptr && info.raiseWindowWhenStart)
			{
			itsOutputDoc->Activate();
			}

		return true;
		}
	else	// we have been deleted
		{
		return false;
		}
}

/******************************************************************************
 StartMakeProcess

 ******************************************************************************/

bool
CBCommand::StartMakeProcess
	(
	CBExecOutputDocument* outputDoc
	)
{
	assert( itsProjDoc != nullptr );

	itsOutputDoc = outputDoc;
	if (itsOutputDoc == nullptr)
		{
		itsOutputDoc = (itsProjDoc->GetCommandManager())->GetCompileDoc(itsProjDoc);
		itsOutputDoc->IncrementUseCount();

		itsBuildOutputDoc = itsOutputDoc;
		}

	SetCompileDocStrings();
	return StartProcess();
}

/******************************************************************************
 SetCompileDocStrings (private)

 ******************************************************************************/

void
CBCommand::SetCompileDocStrings()
{
	itsWindowTitle = JGetString("CompileWindowTitlePrefix::CBCommand");
	if (itsProjDoc != nullptr)
		{
		itsWindowTitle += itsProjDoc->GetName();
		}
	itsDontCloseMsg = JGetString("CompileCloseMsg::CBCommand");
}

/******************************************************************************
 StartProcess (private)

	Returns false if it deletes us.

 ******************************************************************************/

bool
CBCommand::StartProcess()
{
	// check if we are finished

	while (!itsCmdList->IsEmpty() && (itsCmdList->GetElement(1)).IsEndOfSequence())
		{
		CmdInfo info = itsCmdList->GetElement(1);
		info.Free(true);
		itsCmdList->RemoveElement(1);
		}
	if (itsCmdList->IsEmpty())
		{
		if (itsBeepFlag && itsParent == nullptr)
			{
			(JXGetApplication()->GetCurrentDisplay())->Beep();
			}
		DeleteThis();
		return false;
		}

	// check if we can use the window

	itsInQueueFlag = false;
	if (itsOutputDoc != nullptr && itsOutputDoc->ProcessRunning())
		{
		itsInQueueFlag = true;
		ListenTo(itsOutputDoc);
		return true;	// wait for message from itsOutputDoc
		}

	// check if need to run a subroutine

	CmdInfo info = itsCmdList->GetElement(1);
	if (info.cmdObj != nullptr)
		{
		StopListening(itsOutputDoc);	// wait for CBCommand to notify us
		const bool result = (info.cmdObj)->Start(*(info.cmdInfo));
		info.Free(false);
		itsCmdList->RemoveElement(1);
		return result;
		}

	// start process

	assert( info.cmd != nullptr );

	JShouldIncludeCWDOnPath(true);

	JProcess* p;
	int toFD, fromFD;
	JError execErr = JNoError();
	if (itsOutputDoc != nullptr)
		{
		execErr = JProcess::Create(&p, itsCmdPath, *(info.cmd),
								   kJCreatePipe, &toFD,
								   kJCreatePipe, &fromFD,
								   kJAttachToFromFD, nullptr);
		}
	else
		{
		JSimpleProcess* p1;
		execErr = JSimpleProcess::Create(&p1, itsCmdPath, *(info.cmd), true);
		p       = p1;
		}

	JShouldIncludeCWDOnPath(false);

	if (!execErr.OK())
		{
		execErr.ReportIfError();
		DeleteThis();
		return false;
		}

	if (itsOutputDoc != nullptr)
		{
		const JSize count = info.cmd->GetElementCount();
		JString cmd;
		for (JIndex i=1; i<=count; i++)
			{
			cmd += JPrepArgForExec(*info.cmd->GetElement(i));
			cmd += " ";
			}

		itsOutputDoc->SetConnection(p, fromFD, toFD, itsWindowTitle, itsDontCloseMsg,
									itsCmdPath, cmd, true);

		// We can't do this in Start() because we might be waiting for
		// itsMakeDependCmd.  We must not listen to both at the same time.

		ListenTo(itsOutputDoc);
		}
	else
		{
		ListenTo(p);
		JThisProcess::Ignore(p);
		}

	info.Free(true);
	itsCmdList->RemoveElement(1);
	return true;
}

/******************************************************************************
 ProcessFinished (private)

 ******************************************************************************/

void
CBCommand::ProcessFinished
	(
	const bool success,
	const bool cancelled
	)
{
	if (!itsInQueueFlag)
		{
		itsSuccessFlag   = itsSuccessFlag && success;
		itsCancelledFlag = cancelled;
		}

	if (success || itsInQueueFlag)
		{
		if (!itsCmdList->IsEmpty() && (itsCmdList->GetElement(1)).isMakeDepend)
			{
			assert( !itsInQueueFlag );
			CmdInfo info = itsCmdList->GetElement(1);
			info.Free(false);			// don't delete CBCommand because it is deleting itself
			itsCmdList->RemoveElement(1);
			}
		StartProcess();		// may delete us
		}
	else if (!itsCmdList->IsEmpty() && (itsCmdList->GetElement(1)).isMakeDepend)
		{
		CmdInfo info = itsCmdList->GetElement(1);
		info.Free(false);				// don't delete CBCommand because it is deleting itself
		itsCmdList->RemoveElement(1);

		DeleteThis();
		}
	else if (!cancelled)
		{
		while (!itsCmdList->IsEmpty() &&
			   !(itsCmdList->GetElement(1)).IsEndOfSequence())
			{
			CmdInfo info = itsCmdList->GetElement(1);
			info.Free(true);
			itsCmdList->RemoveElement(1);
			}
		StartProcess();		// may delete us
		}
	else
		{
		DeleteThis();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBCommand::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(JProcess::kFinished))
		{
		const auto* info =
			dynamic_cast<const JProcess::Finished*>(&message);
		assert( info != nullptr );
		const bool cancelled = info->GetReason() != kJChildFinished;
		ProcessFinished(info->Successful() && !cancelled, cancelled);
		}
	else if (sender == itsMakeDependCmd && message.Is(CBCommand::kFinished))
		{
		const auto* info =
			dynamic_cast<const CBCommand::Finished*>(&message);
		assert( info != nullptr );
		itsMakeDependCmd     = nullptr;
		const bool cancelled = info->Cancelled();
		ProcessFinished(info->Successful() && !cancelled, cancelled);
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
CBCommand::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	if (sender == itsOutputDoc && message->Is(CBExecOutputDocument::kFinished))
		{
		auto* info =
			dynamic_cast<CBExecOutputDocument::Finished*>(message);
		assert( info != nullptr );
		ProcessFinished(info->Successful(), info->Cancelled());
		}
	else
		{
		JBroadcaster::ReceiveWithFeedback(sender, message);
		}
}

/******************************************************************************
 CmdInfo functions

 ******************************************************************************/

/******************************************************************************
 IsEndOfSequence

 ******************************************************************************/

bool
CBCommand::CmdInfo::IsEndOfSequence()
	const
{
	return cmd == nullptr && cmdObj == nullptr;
}

/******************************************************************************
 IsSubroutine

 ******************************************************************************/

bool
CBCommand::CmdInfo::IsSubroutine()
	const
{
	return cmd == nullptr && cmdObj != nullptr;
}

/******************************************************************************
 Free

 ******************************************************************************/

void
CBCommand::CmdInfo::Free
	(
	const bool deleteCmdObj
	)
{
	jdelete cmd;
	cmd = nullptr;

	if (deleteCmdObj && cmdObj != nullptr)
		{
		cmdObj->itsCallParentProcessFinishedFlag = false;
		jdelete cmdObj;
		cmdObj = nullptr;
		}

	if (cmdInfo != nullptr)
		{
		cmdInfo->Free();
		jdelete cmdInfo;
		cmdInfo = nullptr;
		}
}
