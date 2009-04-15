/******************************************************************************
 SyGFindFileTask.cc

	BASE CLASS = JBroadcaster

	Copyright @ 2008-09 by John Lindal.  All rights reserved.

 ******************************************************************************/

#include <SyGStdInc.h>
#include "SyGFindFileTask.h"
#include "SyGTreeDir.h"
#include "SyGGlobals.h"
#include <JProcess.h>
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
	JString cmd = "find ";
	cmd        += JPrepArgForExec(path);
	cmd        += " ";
	cmd        += expr;

	JProcess* p;
	int outFD, errFD;
	const JError err = JProcess::Create(&p, cmd,
										kJIgnoreConnection, NULL,
										kJCreatePipe, &outFD,
										kJCreatePipe, &errFD);
	if (err.OK())
		{
		*task = new SyGFindFileTask(dir, p, outFD, errFD);
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
	SyGTreeDir*	dir,
	JProcess*	p,
	int			outFD,
	int			errFD
	)
	:
	itsDirector(dir),
	itsProcess(p)
{
	SetConnection(outFD, errFD);

	itsProcess->ShouldDeleteWhenFinished();
	ListenTo(itsProcess);
	ListenTo(itsDirector);
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

	(SyGGetApplication())->OpenDirectory(path, NULL, kJFalse, kJFalse, kJFalse, kJFalse);
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
