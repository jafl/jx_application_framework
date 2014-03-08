/******************************************************************************
 JXSharedPrefObject.cpp

	Base class for objects that own an ID in the global JX shared
	preferences file.  To read/write preferences, simply call
	JXSharedPrefObject::Read/WritePrefs().

	All the instructions for JPrefObject are also relevant here.

	The version list passed to the constructor must be pre-sorted by
	version.

	BASE CLASS = virtual JBroadcaster

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXSharedPrefObject.h>
#include <JXSharedPrefsManager.h>
#include <jXGlobals.h>
#include <sstream>
#include <jAssert.h>

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
	itsVersionList = new JArray<VersionInfo>(versCount);
	assert( itsVersionList != NULL );

	for (JIndex i=0; i<versCount; i++)
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
	delete itsVersionList;
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
	const JSize count = itsVersionList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const VersionInfo info = itsVersionList->GetElement(i);
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
