/******************************************************************************
 MailGlobals.h

	Copyright © 1998 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_MailGlobals
#define _H_MailGlobals

#include <JString.h>
#include <GAddressBookMgr.h>
#include <jTypes.h>

void				InitGlobals();
GAddressBookMgr*	GGetAddressBookMgr();
JBoolean			GPGPEnabled();

#endif
