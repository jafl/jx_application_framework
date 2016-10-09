/******************************************************************************
 GCLGlobals.cpp

	Access to testjx global objects and factories.

	Written by Glenn W. Bach.

 ******************************************************************************/

#include "GCLGlobals.h"
#include "GCLAlarmManager.h"

#include <jAssert.h>

static GCLAlarmManager*		itsAlarmMgr			= NULL;

/******************************************************************************
 GCLCreateGlobals


 ******************************************************************************/

void
GCLCreateGlobals()
{
	itsAlarmMgr	= jnew GCLAlarmManager();
	assert(itsAlarmMgr != NULL);

}

/******************************************************************************
 GCLDeleteGlobals

 ******************************************************************************/

void
GCLDeleteGlobals()
{
	jdelete itsAlarmMgr;
}
/******************************************************************************
 GCLGetAlarmMgr

 ******************************************************************************/

GCLAlarmManager*
GCLGetAlarmMgr()
{
	assert(itsAlarmMgr != NULL);
	return itsAlarmMgr;
}
