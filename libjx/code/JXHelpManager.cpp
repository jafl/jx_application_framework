/******************************************************************************
 JXHelpManager.cpp

	This class is designed to be used as a global object.  All requests by
	the user to display help information should be passed to this object.

	BASE CLASS = none

	Copyright (C) 1997-2017 by John Lindal.

 ******************************************************************************/

#include <JXHelpManager.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXHelpManager::JXHelpManager()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXHelpManager::~JXHelpManager()
{
}

/******************************************************************************
 ShowTOC

	The top of the main page is assumed to be the Table of Contents.

 ******************************************************************************/

void
JXHelpManager::ShowTOC()
{
	(JXGetWebBrowser())->ShowURL(JGetString("HELP_URL"));
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
	JString url = name;
	if (!url.Contains("://"))
		{
		url = JGetString("HELP_URL") + "#" + url;
		}

	(JXGetWebBrowser())->ShowURL(url);
}
