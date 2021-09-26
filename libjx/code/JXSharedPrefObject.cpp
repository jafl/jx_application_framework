/******************************************************************************
 JXSharedPrefObject.cpp

	Base class for objects that own an ID in the global JX shared
	preferences file.  To read/write preferences, simply call
	JXSharedPrefObject::Read/WritePrefs().

	All the instructions for JPrefObject are also relevant here.

	The version list passed to the constructor must be pre-sorted by
	version.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXSharedPrefObject.h"
#include "jx-af/jx/JXSharedPrefsManager.h"
#include "jx-af/jx/jXGlobals.h"
#include <sstream>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSharedPrefObject::JXSharedPrefObject
	(
	const JFileVersion	currVers,
	const JPrefID&		latestVersID,
	const VersionInfo	versList[],
	const JSize			versCount
	)
	:
	itsCurrentPrefsVersion(currVers),
	itsLatestVersionID(latestVersID)
{
	itsVersionList = jnew JArray<VersionInfo>(versCount);
	assert( itsVersionList != nullptr );

	for (JUnsignedOffset i=0; i<versCount; i++)
	{
		if (!itsVersionList->IsEmpty())	// require pre-sorted
		{
			const VersionInfo info = itsVersionList->GetLastElement();
			assert( versList[i].vers > info.vers );
		}

		itsVersionList->AppendElement(versList[i]);
	}

	ListenTo(JXGetSharedPrefsManager());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSharedPrefObject::~JXSharedPrefObject()
{
	jdelete itsVersionList;
}

/******************************************************************************
 GetPrefID

	vers cannot be smaller than the minimum version in our list.

 ******************************************************************************/

JPrefID
JXSharedPrefObject::GetPrefID
	(
	const JFileVersion vers
	)
	const
{
	for (const auto& info : *itsVersionList)
	{
		if (vers == info.vers)
		{
			return info.id;
		}
	}

	const VersionInfo info = itsVersionList->GetLastElement();
	return info.id;
}

/******************************************************************************
 ReadPrefs

 ******************************************************************************/

void
JXSharedPrefObject::ReadPrefs()
{
	(JXGetSharedPrefsManager())->ReadPrefs(this);
}

/******************************************************************************
 WritePrefs

 ******************************************************************************/

void
JXSharedPrefObject::WritePrefs()
	const
{
	(JXGetSharedPrefsManager())->WritePrefs(this);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXSharedPrefObject::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == JXGetSharedPrefsManager() &&
		message.Is(JXSharedPrefsManager::kRead))
	{
		ReadPrefs();
	}
	else
	{
		JBroadcaster::Receive(sender, message);
	}
}
