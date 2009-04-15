/******************************************************************************
 JPrefObject.cpp

	Base class for objects that own an ID in a preferences file.  To read/
	write preferences, simply call JPrefObject::Read/WritePrefs().

	This is designed to be a mixin class, but must only be used once since
	it assumes a single id in the prefs file.  It should also not be
	inherited virtually.

	Even though Read/WritePrefs() is virtual, it should never be overridden
	more than once, in the leaf class.  Otherwise, one will eventually have
	to insert an override of these functions into the inheritance path, and
	this will require that derived classes know about the change.  The better
	way to handle it is for the leaf class to either read/write everything
	explicitly via access functions provided by the base classes or to call
	subsidiary read/write functions in each base class.  This makes it
	explicit that the leaf class knows what is stored and makes it much easier
	to handle new information in base classes.

	BASE CLASS = none

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JPrefObject.h>
#include <sstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

	prefsMgr can be NULL

 ******************************************************************************/

JPrefObject::JPrefObject
	(
	JPrefsManager*	prefsMgr,
	const JPrefID&	id
	)
	:
	itsPrefsMgr(prefsMgr),
	itsID(id)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPrefObject::~JPrefObject()
{
}

/******************************************************************************
 ReadPrefs

 ******************************************************************************/

void
JPrefObject::ReadPrefs()
{
	std::string data;
	if (PrefInfoValid() && itsPrefsMgr->GetData(itsID, &data))
		{
		std::istringstream dataStream(data);
		ReadPrefs(dataStream);
		}
}

/******************************************************************************
 WritePrefs

 ******************************************************************************/

void
JPrefObject::WritePrefs()
	const
{
	if (PrefInfoValid())
		{
		std::ostringstream data;
		WritePrefs(data);
		itsPrefsMgr->SetData(itsID, data);
		}
}
