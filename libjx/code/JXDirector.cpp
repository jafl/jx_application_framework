/******************************************************************************
 JXDirector.cpp

	Stores a list of directors.

	BASE CLASS = virtual JBroadcaster

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDirector.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	supervisor can be NULL, but only JXApplication should do this because
	everybody else should be owned by somebody (the application, at least).

 ******************************************************************************/

JXDirector::JXDirector
	(
	JXDirector* supervisor
	)
{
	itsSupervisor   = supervisor;
	itsSubdirectors = NULL;
	itsActiveFlag   = kJFalse;
	itsSuspendCount = 0;
	itsClosingFlag  = kJFalse;

	if (itsSupervisor != NULL)
		{
		itsSupervisor->AddDirector(this);
		itsSuspendCount = itsSupervisor->itsSuspendCount;
		}
}

/******************************************************************************
 Destructor

	Close() should be called first.

 ******************************************************************************/

JXDirector::~JXDirector()
{
	assert( itsClosingFlag );
	assert( itsSubdirectors == NULL );

	if (itsSupervisor != NULL)
		{
		itsSupervisor->RemoveDirector(this);
		}
}

/******************************************************************************
 IsWindowDirector (virtual)

 ******************************************************************************/

JBoolean
JXDirector::IsWindowDirector()
	const
{
	return kJFalse;
}

/******************************************************************************
 Close (virtual)

	Close all sub-directors and delete ourselves.

 ******************************************************************************/

JBoolean
JXDirector::Close()
{
	assert( !itsClosingFlag );
	itsClosingFlag = kJTrue;

	if (CloseAllSubdirectors())
		{
		delete this;
		return kJTrue;
		}
	else
		{
		itsClosingFlag = kJFalse;
		return kJFalse;
		}
}

/******************************************************************************
 CloseAllSubdirectors (protected)

	Returns kJTrue if all subdirectors closed successfully.

 ******************************************************************************/

JBoolean
JXDirector::CloseAllSubdirectors()
{
	while (itsSubdirectors != NULL && !itsSubdirectors->IsEmpty())
		{
		JXDirector* theDirector = itsSubdirectors->FirstElement();
		if (!theDirector->Close())
			{
			return kJFalse;
			}
		else if (itsSubdirectors != NULL && !itsSubdirectors->IsEmpty() &&
				 theDirector == itsSubdirectors->FirstElement())
			{
			// Since one JXDocument can keep another one open,
			// if the owned document is in front, we have to shove it to
			// the end so we close the owner first.

			itsSubdirectors->MoveElementToIndex(1, itsSubdirectors->GetElementCount());
			}
		}

	// falling through means everybody is closed

	return kJTrue;
}

/******************************************************************************
 Activate (virtual)

	We do not propagate the Activate message to subdirectors because
	each derived class must decide whether or not this is appropriate.

 ******************************************************************************/

void
JXDirector::Activate()
{
	itsActiveFlag = kJTrue;
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

JBoolean
JXDirector::Deactivate()
{
	if (itsActiveFlag && itsSubdirectors != NULL)
		{
		const JSize dirCount = itsSubdirectors->GetElementCount();
		for (JIndex i=1; i<=dirCount; i++)
			{
			if (!(itsSubdirectors->NthElement(i))->Deactivate())
				{
				return kJFalse;
				}
			}
		}
	itsActiveFlag = kJFalse;
	return kJTrue;
}

/******************************************************************************
 Suspend (virtual)

 ******************************************************************************/

void
JXDirector::Suspend()
{
	if (itsSubdirectors != NULL)
		{
		const JSize dirCount = itsSubdirectors->GetElementCount();
		for (JIndex i=1; i<=dirCount; i++)
			{
			(itsSubdirectors->NthElement(i))->Suspend();
			}
		}
	itsSuspendCount++;
}

/******************************************************************************
 Resume (virtual)

 ******************************************************************************/

void
JXDirector::Resume()
{
	if (itsSuspendCount > 0)
		{
		itsSuspendCount--;
		}

	if (itsSubdirectors != NULL)
		{
		const JSize dirCount = itsSubdirectors->GetElementCount();
		for (JIndex i=1; i<=dirCount; i++)
			{
			(itsSubdirectors->NthElement(i))->Resume();
			}
		}
}

/******************************************************************************
 AddDirector (private)

 ******************************************************************************/

void
JXDirector::AddDirector
	(
	JXDirector* theDirector
	)
{
	if (itsSubdirectors == NULL)
		{
		itsSubdirectors = new JPtrArray<JXDirector>(JPtrArrayT::kForgetAll);
		assert( itsSubdirectors != NULL );
		}

	if (!itsSubdirectors->Includes(theDirector))
		{
		itsSubdirectors->Append(theDirector);
		}
}

/*****************************************************************************
 RemoveDirector (private)

 ******************************************************************************/

void
JXDirector::RemoveDirector
	(
	JXDirector* theDirector
	)
{
	if (itsSubdirectors != NULL)
		{
		itsSubdirectors->Remove(theDirector);
		if (itsSubdirectors->IsEmpty())
			{
			delete itsSubdirectors;
			itsSubdirectors = NULL;
			}
		DirectorClosed(theDirector);
		}
}

/*****************************************************************************
 DirectorClosed (virtual protected)

	The only safe way to know whether a subdirector still exists is
	to listen for this message.

	One does not need this for dialogs because they close immediately
	after broadcasting the Deactivated message.

 ******************************************************************************/

void
JXDirector::DirectorClosed
	(
	JXDirector* theDirector
	)
{
}

#define JTemplateType JXDirector
#include <JPtrArray.tmpls>
#undef JTemplateType
