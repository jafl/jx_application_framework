/******************************************************************************
 SyGFindFileTask.cc

	BASE CLASS = JBroadcaster

	Copyright @ 2008-09 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include <SyGStdInc.h>
#include "SyGFindFileTask.h"
#include "SyGTreeDir.h"
#include "SyGFileTreeTable.h"
#include "SyGFileTree.h"
#include "SyGGlobals.h"
#include <JProcess.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Create (static)

 ******************************************************************************/

JBoolean
SyGFindFileTask::Create
	(
	SyGTreeDir*			dir,
	const JCharacter*	path,
	const JCharacter*	expr,
	SyGFindFileTask**	task
	)
{
	JString cmd = "find . ";
	cmd        += expr;

	JProcess* p;
	int outFD, errFD;
	const JError err = JProcess::Create(&p, path, cmd,
										kJIgnoreConnection, NULL,
										kJCreatePipe, &outFD,
										kJCreatePipe, &errFD);
	if (err.OK())
		{
		JString relPath = path;
		relPath.RemoveSubstring(1, dir->GetDirectory().GetLength());

		*task = new SyGFindFileTask(dir, relPath, p, outFD, errFD);
		assert( *task != NULL );
		return kJTrue;
		}
	else
		{
		err.ReportIfError();
		*task = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

SyGFindFileTask::SyGFindFileTask
	(
	SyGTreeDir*			dir,
	const JCharacter*	relPath,
	JProcess*			process,
	int					outFD,
	int					errFD
	)
	:
	itsDirector(dir),
	itsProcess(process),
	itsFoundFilesFlag(kJFalse)
{
	SetConnection(outFD, errFD);

	itsProcess->ShouldDeleteWhenFinished();
	ListenTo(itsProcess);
	ListenTo(itsDirector);

	itsPathList = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsPathList != NULL );

	SplitPath(relPath, itsPathList);

	itsDirector->GetTable()->GetFileTree()->Update(kJTrue);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGFindFileTask::~SyGFindFileTask()
{
	if (itsProcess != NULL)
		{
		StopListening(itsProcess);
		itsProcess->Kill();
		}
	delete itsProcess;

	delete itsPathList;
	DeleteLinks();
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
		delete this;
		}
	else if (sender == itsProcess)
		{
		itsProcess = NULL;
		DisplayErrors();
		delete this;
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
	assert( itsMessageLink != NULL );

	JString path;
	const JBoolean ok = itsMessageLink->GetNextMessage(&path);
	assert( ok );

	if (!path.BeginsWith("." ACE_DIRECTORY_SEPARATOR_STR))
		{
		return;
		}
	path.RemoveSubstring(1, 2);

	JPtrArray<JString> pathList(JPtrArrayT::kDeleteAll);
	SplitPath(path, &pathList);

	JString* name = pathList.LastElement();
	pathList.RemoveElement(pathList.GetElementCount());

	for (JIndex i=itsPathList->GetElementCount(); i>=1; i--)
		{
		pathList.Prepend(*(itsPathList->NthElement(i)));
		}

	JPoint cell;
	(itsDirector->GetTable())->SelectName(pathList, *name, &cell, kJFalse, kJFalse);

	delete name;
	itsFoundFilesFlag = kJTrue;
}

/******************************************************************************
 ReceiveErrorLine (private)

 ******************************************************************************/

void
SyGFindFileTask::ReceiveErrorLine()
{
	assert( itsErrorLink != NULL );

	JString line;
	const JBoolean ok = itsErrorLink->GetNextMessage(&line);
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
		(JGetUserNotification())->ReportError(itsErrors);
		}
	else if (!itsFoundFilesFlag)
		{
		(JGetUserNotification())->DisplayMessage(JGetString("NoMatch::SyGFindFileTask"));
		}
}

/******************************************************************************
 SplitPath (private static)

 ******************************************************************************/

void
SyGFindFileTask::SplitPath
	(
	const JCharacter*	origRelPath,
	JPtrArray<JString>*	pathList
	)
{
	pathList->CleanOut();
	if (JStringEmpty(origRelPath))
		{
		return;
		}

	JString relPath = origRelPath;
	while (relPath.BeginsWith(ACE_DIRECTORY_SEPARATOR_STR))
		{
		relPath.RemoveSubstring(1,1);
		}
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

/******************************************************************************
 SetConnection (private)

 ******************************************************************************/

// This function has to be last so JCore::new works for everything else.

#undef new

void
SyGFindFileTask::SetConnection
	(
	const int outFD,
	const int errFD
	)
{
	itsMessageLink = new RecordLink(outFD);
	assert( itsMessageLink != NULL );
	ListenTo(itsMessageLink);

	itsErrorLink = new RecordLink(errFD);
	assert( itsErrorLink != NULL );
	ListenTo(itsErrorLink);
}

/******************************************************************************
 DeleteLinks (private)

 ******************************************************************************/

// This function has to be last so JCore::delete works for everything else.

#undef delete

void
SyGFindFileTask::DeleteLinks()
{
	delete itsMessageLink;
	itsMessageLink = NULL;

	delete itsErrorLink;
	itsErrorLink = NULL;
}
