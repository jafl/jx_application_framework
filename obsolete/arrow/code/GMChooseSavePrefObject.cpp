/******************************************************************************
 GMChooseSavePrefObject.cc

	This just maintains the prefs for the help manager.

	BASE CLASS = JPrefObject

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <GMChooseSavePrefObject.h>
#include <GMChooseSaveFile.h>

#include <jXGlobals.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GMChooseSavePrefObject::GMChooseSavePrefObject
	(
	JPrefsManager*		prefsMgr,
	const JPrefID&		id,
	GMChooseSaveFile*	csf
	)
	:
	JPrefObject(prefsMgr, id),
	itsCSF(csf)
{
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMChooseSavePrefObject::~GMChooseSavePrefObject()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 Read/WritePrefs

 ******************************************************************************/

void
GMChooseSavePrefObject::ReadPrefs
	(
	istream& input
	)
{
	itsCSF->ReadGMSetup(input);
}

void
GMChooseSavePrefObject::WritePrefs
	(
	ostream& output
	)
	const
{
	itsCSF->WriteGMSetup(output);
}
