/******************************************************************************
 jWebUtil.h

	Utility functions for dealing with URLs.

	Copyright © 2008-10 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jWebUtil
#define _H_jWebUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPrefsManager.h>

enum
{
	kJDefaultHTTPPort    = 80,
	kJDefaultHTTPSPort   = 443,
	kJDefaultFTPPort     = 21,
	kJDefaultFTPDataPort = 20,
	kJDefaultSFTPPort    = 115,
	kJDefaultSSHPort     = 22
};

JBoolean	JIsURL(const JCharacter* s);
JBoolean	JParseURL(const JCharacter* url, JString* protocol,
					  JString* host, JIndex* port, JString* path);
JBoolean	JGetDefaultPort(const JCharacter* protocol, JIndex* port);

void	JCheckForNewerVersion(JPrefsManager* prefsMgr, const JPrefID& prefID);

#endif
