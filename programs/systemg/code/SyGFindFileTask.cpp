/******************************************************************************
 SyGFindFileTask.cpp

	BASE CLASS = JBroadcaster

	Copyright @ 2008-09 by John Lindal.

 ******************************************************************************/

#include "SyGFindFileTask.h"
#include "SyGTreeDir.h"
#include "SyGFileTreeTable.h"
#include "SyGFileTree.h"
#include "SyGGlobals.h"
#include <JProcess.h>
#include <JStringIterator.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Create (static)

 ******************************************************************************/

bool
SyGFindFileTask::Create
	(
	SyGTreeDir*			dir,
	const JString&		path,
	const JString&		expr,
	SyGFindFileTask**	task
	)
{
	JString cmd("find . ");
	cmd += expr;

	JProcess* p;
	int outFD, errFD;
	const JError err = JProcess::Create(&p, path, cmd,
										kJIgnoreConnection, nullptr,
										kJCreatePipe, &outFD,
										kJCreatePipe, &errFD);
	if (err.OK())
		{
		JString relPath = path;
		JStringIterator iter(&relPath);
		iter.SkipNext(dir->GetDirectory().GetCharacterCount());
		iter.RemoveAllPrev();
		iter.Invalidate();

		*task = jnew SyGFindFileTask(dir, relPath, p, outFD, errFD);
		assert( *task != nullptr );
		return true;
		}
	else
		{
		err.ReportIfError();
		*task = nullptr;
		return false;
		}
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

SyGFindFileTask::SyGFindFileTask
	(
	SyGTreeDir*		dir,
	const JString&	relPath,
	JProcess*		process,
	int				outFD,
	int				errFD
	)
	:
	itsDirector(dir),
	itsProcess(process),
	itsFoundFilesFlag(false)
{
	itsMessageLink = new RecordLink(outFD);
	assert( itsMessageLink != nullptr );
	ListenTo(itsMessageLink);

	itsErrorLink = new RecordLink(errFD);
	assert( itsErrorLink != nullptr );
	ListenTo(itsErrorLink);

	itsProcess->ShouldDeleteWhenFinished();
	ListenTo(itsProcess);
	ListenTo(itsDirector);

	itsPathList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsPathList != nullptr );

	SplitPath(relPath, itsPathList);

	itsDirector->GetTable()->GetFileTree()->Update(true);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGFindFileTask::~SyGFindFileTask()
{
	if (itsProcess != nullptr)
		{
		StopListening(itsProcess);
		itsProcess->Kill();
		}
	jdelete itsProcess;

	jdelete itsPathList;
	delete itsMessageLink;
	delete itsErrorLink;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SyGFindFileTask::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsMessageLink && message.Is(JMessageProtocolT::kMessageReady))
		{
		ReceiveMessageLine();
		}
	else if (sender == itsErrorLink && message.Is(JMessageProtocolT::kMessageReady))
		{
		ReceiveErrorLine();
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
SyGFindFileTask::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsDirector)
		{
		jdelete this;
		}
	else if (sender == itsProcess)
		{
		itsProcess = nullptr;
		DisplayErrors();
		jdelete this;
		}
	else
		{
		JBroadcaster::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 ReceiveMessageLine (private)

 ******************************************************************************/

void
SyGFindFileTask::ReceiveMessageLine()
{
	assert( itsMessageLink != nullptr );

	JString path;
	const bool ok = itsMessageLink->GetNextMessage(&path);
	assert( ok );

	if (!path.BeginsWith("." ACE_DIRECTORY_SEPARATOR_STR))
		{
		return;
		}

	JStringIterator iter(&path);
	iter.SkipNext(2);
	iter.RemoveAllPrev();
	iter.Invalidate();

	JPtrArray<JString> pathList(JPtrArrayT::kDeleteAll);
	SplitPath(path, &pathList);

	JString* name = pathList.GetLastElement();
	pathList.RemoveElement(pathList.GetElementCount());

	for (JIndex i=itsPathList->GetElementCount(); i>=1; i--)
		{
		pathList.Prepend(*(itsPathList->GetElement(i)));
		}

	JPoint cell;
	(itsDirector->GetTable())->SelectName(pathList, *name, &cell, false, false);

	jdelete name;
	itsFoundFilesFlag = true;
}

/******************************************************************************
 ReceiveErrorLine (private)

 ******************************************************************************/

void
SyGFindFileTask::ReceiveErrorLine()
{
	assert( itsErrorLink != nullptr );

	JString line;
	const bool ok = itsErrorLink->GetNextMessage(&line);
	assert( ok );

	if (!itsErrors.IsEmpty())
		{
		itsErrors += "\n";
		}
	itsErrors += line;
}

/******************************************************************************
 DisplayErrors (private)

 ******************************************************************************/

void
SyGFindFileTask::DisplayErrors()
{
	if (!itsErrors.IsEmpty())
		{
		JGetUserNotification()->ReportError(itsErrors);
		}
	else if (!itsFoundFilesFlag)
		{
		JGetUserNotification()->DisplayMessage(JGetString("NoMatch::SyGFindFileTask"));
		}
}

/******************************************************************************
 SplitPath (private static)

 ******************************************************************************/

void
SyGFindFileTask::SplitPath
	(
	const JString&		origRelPath,
	JPtrArray<JString>*	pathList
	)
{
	pathList->CleanOut();
	if (origRelPath.IsEmpty())
		{
		return;
		}

	JString relPath = origRelPath;
	JStringIterator iter(&relPath);
	JUtf8Character c;
	while (iter.Next(&c) && c == ACE_DIRECTORY_SEPARATOR_CHAR)
		{
		iter.RemovePrev();
		}
	iter.Invalidate();

	JStripTrailingDirSeparator(&relPath);

	JString p, n;
	while (relPath.Contains(ACE_DIRECTORY_SEPARATOR_STR))
		{
		JSplitPathAndName(relPath, &p, &n);
		pathList->Prepend(n);

		JStripTrailingDirSeparator(&p);
		relPath = p;
		}

	pathList->Prepend(relPath);
}
