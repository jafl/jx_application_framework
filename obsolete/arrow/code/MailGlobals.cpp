/******************************************************************************
 MailGlobals.h

	Copyright © 1997 by Glenn Bach.

 ******************************************************************************/

#include "MailGlobals.h"

#include <JString.h>
#include <JPtrArray.h>

#include <jDirUtil.h>
#include <jFileUtil.h>

#include <stdlib.h>
#include <stdio.h>
#include <jAssert.h>

JBoolean			itsPGPEnabled;
GAddressBookMgr*	itsAddressBookMgr;;

/******************************************************************************
 InitGlobals


 ******************************************************************************/

void
InitGlobals()
{
	itsAddressBookMgr = new GAddressBookMgr();
	assert(itsAddressBookMgr != NULL);

	itsPGPEnabled = kJTrue;
}

/******************************************************************************
 GGetAddressBookMgr


 ******************************************************************************/

void
GGetAddressBookMgr()
{
	return itsAddressBookMgr;
}

/******************************************************************************
 GPGPEnabled


 ******************************************************************************/

JBoolean
GPGPEnabled()
{
	return itsPGPEnabled;
}
