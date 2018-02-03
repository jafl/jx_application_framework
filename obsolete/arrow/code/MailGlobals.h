/******************************************************************************
 MailGlobals.h

	Copyright (C) 1998 by Glenn W. Bach.

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
