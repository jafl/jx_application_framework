/******************************************************************************
 JXSharedPrefsManager.cpp

	Written by John Lindal.

 ******************************************************************************/

#include "JXSharedPrefsManager_mock.h"
#include "JXSharedPrefObject.h"
#include <jAssert.h>

static JXSharedPrefsManager theSharedPrefsMgr;

// JBroadcaster message types

const JUtf8Byte* JXSharedPrefsManager::kRead = "Read::JXSharedPrefsManager";

/******************************************************************************
 JXGetSharedPrefsManager

 ******************************************************************************/

JXSharedPrefsManager*
JXGetSharedPrefsManager()
{
	return &theSharedPrefsMgr;
}

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSharedPrefsManager::JXSharedPrefsManager()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSharedPrefsManager::~JXSharedPrefsManager()
{
}

/******************************************************************************
 ReadPrefs

 ******************************************************************************/

void
JXSharedPrefsManager::ReadPrefs
	(
	JXSharedPrefObject* obj
	)
{
}

/******************************************************************************
 WritePrefs

 ******************************************************************************/

void
JXSharedPrefsManager::WritePrefs
	(
	const JXSharedPrefObject* obj
	)
{
}
