/******************************************************************************
 JXPrefsManager.cpp

	Builds the file name based on the application signature.

	BASE CLASS = JPrefsManager

	Copyright © 2000 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXPrefsManager.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXPrefsManager::JXPrefsManager
	(
	const JFileVersion	currentVersion,
	const JBoolean		eraseFileIfOpen
	)
	:
	JPrefsManager((JXGetApplication())->GetSignature(),
				  currentVersion, eraseFileIfOpen)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPrefsManager::~JXPrefsManager()
{
}
