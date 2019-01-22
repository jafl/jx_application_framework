/******************************************************************************
 CBCommand.cpp

	Executes a sequence of commands from CBCommandManager.

	If multiple sequences of commands are to be executed, each sequence is
	delimited by a blank (nullptr) command.  This allows us to skip to the
	next complete sequence if a command returns an error.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2002-2018 by John Lindal.

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

const JCharacter* CBCommand::kFinished = "Finished::CBCommand";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCommand::CBCommand
	(
	const JString&		path,
	const JBoolean		refreshVCSStatusWhenFinished,
	const JBoolean		beepWhenFinished,
	CBProjectDocument*	projDoc
	)
	:
	itsProjDoc(projDoc),
	itsCmdPath(path),
	itsOutputDoc(nullptr),
	itsBeepFlag(beepWhenFinished),
	itsRefreshVCSStatusFlag(refreshVCSStatusWhenFinished),
	itsUpdateSymbolDatabaseFlag(kJFalse),
	itsInQueueFlag(kJFalse),
	itsSuccessFlag(kJTrue),
	itsCancelledFlag(kJFalse),
	itsMakeDependCmd(nullptr),
	itsBuildOutputDoc(nullptr),
	itsRunOutputDoc(nullptr),
	itsParent(nullptr),
	itsCallParentProcessFinishedFlag(kJTrue)
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
		(CBGetDocumentManager())->UpdateSymbolDatabases();
		}

	// only refresh VCS status when all finished, since may be expensive

	if (itsParent != nullptr && itsRefreshVCSStatusFlag)
		{
		itsParent->itsRefreshVCSStatusFlag = kJTrue;
		}
	else if (itsRefreshVCSStatusFlag)
		{
		(CBGetDocumentManager())->RefreshVCSStatus();
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
		info.Free(kJTrue);
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

JBoolean
CBCommand::Add
	(
	const JPtrArray<JString>&	cmdArgs,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList,
	CBFunctionStack*			fnStack
	)
{
	const JString* firstArg = cmdArgs.FirstElement();
	if (firstArg->GetFirstCharacter() == '&')
		{
		assert( fnStack != nullptr );

		// check for re-used command name

		const JCharacter* cmdName = firstArg->GetCString()+1;

		const JSize cmdCount = fnStack->GetElementCount();
		for (JIndex j=1; j<=cmdCount; j++)
			{
			if (strcmp(cmdName, fnStack->Peek(j)) == 0)
				{
				ReportInfiniteLoop(*fnStack, j);
				return kJFalse;
				}
			}

		// prepare cmd for execution later

		fnStack->Push(cmdName);

		CBCommandManager* mgr =
			(itsProjDoc != nullptr ? itsProjDoc->GetCommandManager() : CBGetCommandManager());
		CBCommand* cmdObj;
		CBCommandManager::CmdInfo* cmdInfo;
		if (mgr->Prepare(cmdName, itsProjDoc, fullNameList, lineIndexList,
						 &cmdObj, &cmdInfo, fnStack))
			{
			cmdObj->SetParent(this);
			itsCmdList->AppendElement(CmdInfo(nullptr, cmdObj, cmdInfo, kJFalse));
			}
		else
			{
			return kJFalse;
			}

		fnStack->Pop();
		}
	else
		{
		JPtrArray<JString>* args = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert( args != nullptr );
		args->CopyObjects(cmdArgs, JPtrArrayT::kDeleteAll, kJFalse);

		itsCmdList->AppendElement(CmdInfo(args, nullptr, nullptr, kJFalse));
		}

	return kJTrue;
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

	CBCommandManager::CmdInfo* info = jnew CBCommandManager::CmdInfo;
	assert( info != nullptr );
	*info = cmdInfo.Copy();

	itsCmdList->AppendElement(CmdInfo(nullptr, subCmd, info, kJFalse));
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
	const JSize cmdCount = fnStack.GetElementCount();
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

	const JCharacter* map[] =
		{
		"loop", loop.GetCString()
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

	Returns kJFalse if it deletes us.

 ******************************************************************************/

JBoolean
CBCommand::Start
	(
	const CBCommandManager::CmdInfo& info
	)
{
	CBCommandManager* mgr =
		(itsProjDoc != nullptr ? itsProjDoc->GetCommandManager() : CBGetCommandManager());

	if (info.isMake)
		{
		itsUpdateSymbolDatabaseFlag = kJTrue;

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
		(CBGetDocumentManager())->SaveTextDocuments(kJFalse);
		}

	if (info.isVCS)
		{
		itsUpdateSymbolDatabaseFlag = kJTrue;
		}

	if (itsUpdateSymbolDatabaseFlag)
		{
		(CBGetDocumentManager())->CancelUpdateSymbolDatabases();
		}

	// after saving all files, update Makefile

	JBoolean waitForMakeDepend = kJFalse;
	if (info.isMake && itsProjDoc != nullptr)
		{
		waitForMakeDepend =
			(itsProjDoc->GetBuildManager())->UpdateMakefile(itsOutputDoc, &itsMakeDependCmd);
		}

	if (waitForMakeDepend)
		{
		if (itsMakeDependCmd != nullptr)
			{
			itsCmdList->PrependElement(CmdInfo(nullptr, itsMakeDependCmd, nullptr, kJTrue));
			ListenTo(itsMakeDependCmd);		// many may need to hear; can't use SetParent()
			return kJTrue;
			}
		else
			{
			DeleteThis();
			return kJFalse;
			}
		}
	else if (StartProcess())
		{
		if (itsOutputDoc != nullptr && info.raiseWindowWhenStart)
			{
			itsOutputDoc->Activate();
			}

		return kJTrue;
		}
	else	// we have been deleted
		{
		return kJFalse;
		}
}

/******************************************************************************
 StartMakeProcess

 ******************************************************************************/

JBoolean
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

	Returns kJFalse if it deletes us.

 ******************************************************************************/

JBoolean
CBCommand::StartProcess()
{
	// check if we are finished

	while (!itsCmdList->IsEmpty() && (itsCmdList->GetElement(1)).IsEndOfSequence())
		{
		CmdInfo info = itsCmdList->GetElement(1);
		info.Free(kJTrue);
		itsCmdList->RemoveElement(1);
		}
	if (itsCmdList->IsEmpty())
		{
		if (itsBeepFlag && itsParent == nullptr)
			{
			(JXGetApplication()->GetCurrentDisplay())->Beep();
			}
		DeleteThis();
		return kJFalse;
		}

	// check if we can use the window

	itsInQueueFlag = kJFalse;
	if (itsOutputDoc != nullptr && itsOutputDoc->ProcessRunning())
		{
		itsInQueueFlag = kJTrue;
		ListenTo(itsOutputDoc);
		return kJTrue;	// wait for message from itsOutputDoc
		}

	// check if need to run a subroutine

	CmdInfo info = itsCmdList->GetElement(1);
	if (info.cmdObj != nullptr)
		{
		StopListening(itsOutputDoc);	// wait for CBCommand to notify us
		const JBoolean result = (info.cmdObj)->Start(*(info.cmdInfo));
		info.Free(kJFalse);
		itsCmdList->RemoveElement(1);
		return result;
		}

	// start process

	assert( info.cmd != nullptr );

	JShouldIncludeCWDOnPath(kJTrue);

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
		execErr = JSimpleProcess::Create(&p1, itsCmdPath, *(info.cmd), kJTrue);
		p       = p1;
		}

	JShouldIncludeCWDOnPath(kJFalse);

	if (!execErr.OK())
		{
		execErr.ReportIfError();
		DeleteThis();
		return kJFalse;
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
									itsCmdPath, cmd, kJTrue);

		// We can't do this in Start() because we might be waiting for
		// itsMakeDependCmd.  We must not listen to both at the same time.

		ListenTo(itsOutputDoc);
		}
	else
		{
		ListenTo(p);
		JThisProcess::Ignore(p);
		}

	info.Free(kJTrue);
	itsCmdList->RemoveElement(1);
	return kJTrue;
}

/******************************************************************************
 ProcessFinished (private)

 ******************************************************************************/

void
CBCommand::ProcessFinished
	(
	const JBoolean success,
	const JBoolean cancelled
	)
{
	if (!itsInQueueFlag)
		{
		itsSuccessFlag   = JI2B(itsSuccessFlag && success);
		itsCancelledFlag = cancelled;
		}

	if (success || itsInQueueFlag)
		{
		if (!itsCmdList->IsEmpty() && (itsCmdList->GetElement(1)).isMakeDepend)
			{
			assert( !itsInQueueFlag );
			CmdInfo info = itsCmdList->GetElement(1);
			info.Free(kJFalse);			// don't delete CBCommand because it is deleting itself
			itsCmdList->RemoveElement(1);
			}
		StartProcess();		// may delete us
		}
	else if (!itsCmdList->IsEmpty() && (itsCmdList->GetElement(1)).isMakeDepend)
		{
		CmdInfo info = itsCmdList->GetElement(1);
		info.Free(kJFalse);				// don't delete CBCommand because it is deleting itself
		itsCmdList->RemoveElement(1);

		DeleteThis();
		}
	else if (!cancelled)
		{
		while (!itsCmdList->IsEmpty() &&
			   !(itsCmdList->GetElement(1)).IsEndOfSequence())
			{
			CmdInfo info = itsCmdList->GetElement(1);
			info.Free(kJTrue);
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
		const JProcess::Finished* info =
			dynamic_cast<const JProcess::Finished*>(&message);
		assert( info != nullptr );
		const JBoolean cancelled = JI2B(info->GetReason() != kJChildFinished);
		ProcessFinished(JI2B(info->Successful() && !cancelled), cancelled);
		}
	else if (sender == itsMakeDependCmd && message.Is(CBCommand::kFinished))
		{
		const CBCommand::Finished* info =
			dynamic_cast<const CBCommand::Finished*>(&message);
		assert( info != nullptr );
		itsMakeDependCmd         = nullptr;
		const JBoolean cancelled = info->Cancelled();
		ProcessFinished(JI2B(info->Successful() && !cancelled), cancelled);
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
		CBExecOutputDocument::Finished* info =
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

JBoolean
CBCommand::CmdInfo::IsEndOfSequence()
	const
{
	return JI2B( cmd == nullptr && cmdObj == nullptr );
}

/******************************************************************************
 IsSubroutine

 ******************************************************************************/

JBoolean
CBCommand::CmdInfo::IsSubroutine()
	const
{
	return JI2B( cmd == nullptr && cmdObj != nullptr );
}

/******************************************************************************
 Free

 ******************************************************************************/

void
CBCommand::CmdInfo::Free
	(
	const JBoolean deleteCmdObj
	)
{
	jdelete cmd;
	cmd = nullptr;

	if (deleteCmdObj && cmdObj != nullptr)
		{
		cmdObj->itsCallParentProcessFinishedFlag = kJFalse;
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
