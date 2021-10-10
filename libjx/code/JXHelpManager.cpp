/******************************************************************************
 JXHelpManager.cpp

	This class is designed to be used as a global object.  All requests by
	the user to display help information should be passed to this object.

	BASE CLASS = none

	Copyright (C) 1997-2017 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXHelpManager.h"
#include <jx-af/jcore/JWebBrowser.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXHelpManager::JXHelpManager()
{
}

/******************************************************************************
 ShowTOC

	The top of the main page is assumed to be the Table of Contents.

 ******************************************************************************/

void
JXHelpManager::ShowTOC()
{
	JGetWebBrowser()->ShowURL(JGetString("HELP_URL"));
}

/******************************************************************************
 ShowSection

 ******************************************************************************/

void
JXHelpManager::ShowSection
	(
	const JUtf8Byte* name
	)
{
	JString url(name);
	if (!url.Contains("://"))
	{
		url = JGetString("HELP_URL") + JString("#", JString::kNoCopy) + url;
	}

	JGetWebBrowser()->ShowURL(url);
}

/******************************************************************************
 ShowChangeLog

 ******************************************************************************/

void
JXHelpManager::ShowChangeLog()
{
	JGetWebBrowser()->ShowURL(JGetString("CHANGE_LOG_URL"));
}

/******************************************************************************
 ShowCredits

 ******************************************************************************/

void
JXHelpManager::ShowCredits()
{
	JGetWebBrowser()->ShowURL(JGetString("CREDITS_URL"));
}
