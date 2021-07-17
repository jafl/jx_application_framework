/******************************************************************************
 jWebUtil.h

	Utility functions for dealing with URLs.

	Copyright (C) 2008-10 by John Lindal.

 ******************************************************************************/

#ifndef _H_jWebUtil
#define _H_jWebUtil

#include "JPrefsManager.h"

enum
{
	kJDefaultHTTPPort    = 80,
	kJDefaultHTTPSPort   = 443,
	kJDefaultFTPPort     = 21,
	kJDefaultFTPDataPort = 20,
	kJDefaultSFTPPort    = 115,
	kJDefaultSSHPort     = 22
};

bool	JIsURL(const JString& s);
bool	JParseURL(const JString& url, JString* protocol,
					  JString* host, JIndex* port, JString* path);
bool	JGetDefaultPort(const JString& protocol, JIndex* port);

void	JCheckForNewerVersion(JPrefsManager* prefsMgr, const JPrefID& prefID);

#endif
