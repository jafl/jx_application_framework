/******************************************************************************
 GCLGlobals.cpp

	Access to testjx global objects and factories.

	Written by Glenn W. Bach.

 ******************************************************************************/

#include <JXStdInc.h>
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
	itsAlarmMgr	= new GCLAlarmManager();
	assert(itsAlarmMgr != NULL);

}

/******************************************************************************
 GCLDeleteGlobals

 ******************************************************************************/

void
GCLDeleteGlobals()
{
	delete itsAlarmMgr;
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
