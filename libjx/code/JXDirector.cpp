/******************************************************************************
 JXDirector.cpp

	Stores a list of directors.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXDirector.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

	supervisor can be nullptr, but only JXApplication should do this because
	everybody else should be owned by somebody (the application, at least).

 ******************************************************************************/

JXDirector::JXDirector
	(
	JXDirector* supervisor
	)
{
	itsSupervisor   = supervisor;
	itsSubdirectors = nullptr;
	itsActiveFlag   = false;
	itsSuspendCount = 0;
	itsClosingFlag  = false;

	if (itsSupervisor != nullptr)
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
	assert( itsSubdirectors == nullptr );

	if (itsSupervisor != nullptr)
		{
		itsSupervisor->RemoveDirector(this);
		}
}

/******************************************************************************
 IsWindowDirector (virtual)

 ******************************************************************************/

bool
JXDirector::IsWindowDirector()
	const
{
	return false;
}

/******************************************************************************
 Close (virtual)

	Close all sub-directors and delete ourselves.

 ******************************************************************************/

bool
JXDirector::Close()
{
	assert( !itsClosingFlag );
	itsClosingFlag = true;

	if (CloseAllSubdirectors())
		{
		jdelete this;
		return true;
		}
	else
		{
		itsClosingFlag = false;
		return false;
		}
}

/******************************************************************************
 CloseAllSubdirectors (protected)

	Returns true if all subdirectors closed successfully.

 ******************************************************************************/

bool
JXDirector::CloseAllSubdirectors()
{
	while (itsSubdirectors != nullptr && !itsSubdirectors->IsEmpty())
		{
		JXDirector* theDirector = itsSubdirectors->GetFirstElement();
		if (!theDirector->Close())
			{
			return false;
			}
		else if (itsSubdirectors != nullptr && !itsSubdirectors->IsEmpty() &&
				 theDirector == itsSubdirectors->GetFirstElement())
			{
			// Since one JXDocument can keep another one open,
			// if the owned document is in front, we have to shove it to
			// the end so we close the owner first.

			itsSubdirectors->MoveElementToIndex(1, itsSubdirectors->GetElementCount());
			}
		}

	// falling through means everybody is closed

	return true;
}

/******************************************************************************
 Activate (virtual)

	We do not propagate the Activate message to subdirectors because
	each derived class must decide whether or not this is appropriate.

 ******************************************************************************/

void
JXDirector::Activate()
{
	itsActiveFlag = true;
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

bool
JXDirector::Deactivate()
{
	if (itsActiveFlag && itsSubdirectors != nullptr)
		{
		for (JXDirector* dir : *itsSubdirectors)
			{
			if (!dir->Deactivate())
				{
				return false;
				}
			}
		}
	itsActiveFlag = false;
	return true;
}

/******************************************************************************
 Suspend (virtual)

 ******************************************************************************/

void
JXDirector::Suspend()
{
	if (itsSubdirectors != nullptr)
		{
		for (JXDirector* dir : *itsSubdirectors)
			{
			dir->Suspend();
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

	if (itsSubdirectors != nullptr)
		{
		for (JXDirector* dir : *itsSubdirectors)
			{
			dir->Resume();
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
	if (itsSubdirectors == nullptr)
		{
		itsSubdirectors = jnew JPtrArray<JXDirector>(JPtrArrayT::kForgetAll);
		assert( itsSubdirectors != nullptr );
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
	if (itsSubdirectors != nullptr)
		{
		itsSubdirectors->Remove(theDirector);
		if (itsSubdirectors->IsEmpty())
			{
			jdelete itsSubdirectors;
			itsSubdirectors = nullptr;
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
