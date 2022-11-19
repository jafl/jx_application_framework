/******************************************************************************
 jWebUtil.cpp

	System independent URL utilities.

	Copyright (C) 2008-10 by John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/jWebUtil.h"
#include "jx-af/jcore/JUpdateChecker.h"
#include "jx-af/jcore/JRegex.h"
#include "jx-af/jcore/JStringMatch.h"
#include "jx-af/jcore/JWebBrowser.h"
#include "jx-af/jcore/jGlobals.h"
#include <ace/Connector.h>
#include <ace/SOCK_Connector.h>
#include "jx-af/jcore/jErrno.h"
#include "jx-af/jcore/jAssert.h"

/******************************************************************************
 JIsURL

	Renames the specified directory.

 ******************************************************************************/

bool
JIsURL
	(
	const JString& s
	)
{
	return s.Contains("://");
}

/******************************************************************************
 JParseURL

	Extract host:post and file from protocol://host[:port]/[path]

	If the port is not specified, it is set to zero.  The caller must
	decided on a sensible default, based on the protocol.  One option is to
	call JGetDefaultPort().

 ******************************************************************************/

static const JRegex urlPattern = "^([^:]+)://([^/:]+)(?::([0-9]+))?(.*)";

bool
JParseURL
	(
	const JString&	url,
	JString*		protocol,
	JString*		host,
	JIndex*			port,
	JString*		path
	)
{
	*path = url;

	const JStringMatch m = urlPattern.Match(url, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
	{
		protocol->Set(m.GetSubstring(1));
		host->Set(m.GetSubstring(2));

		if (!m.GetSubstring(3).ConvertToUInt(port))
		{
			*port = 0;
		}

		if (!m.GetCharacterRange(4).IsEmpty())
		{
			path->Set(m.GetSubstring(4));
		}
		else
		{
			*path = "/";
		}

		return true;
	}

	return false;
}

/******************************************************************************
 JGetDefaultPort

	Returns true if it recognizes the protocol.

 ******************************************************************************/

bool
JGetDefaultPort
	(
	const JString&	protocol,
	JIndex*			port
	)
{
	if (JString::Compare(protocol, "http", JString::kIgnoreCase) == 0)
	{
		*port = kJDefaultHTTPPort;
		return true;
	}
	else if (JString::Compare(protocol, "https", JString::kIgnoreCase) == 0)
	{
		*port = kJDefaultHTTPSPort;
		return true;
	}
	else if (JString::Compare(protocol, "ftp", JString::kIgnoreCase) == 0)
	{
		*port = kJDefaultFTPPort;
		return true;
	}
	else if (JString::Compare(protocol, "sftp", JString::kIgnoreCase) == 0)
	{
		*port = kJDefaultSFTPPort;
		return true;
	}
	else if (JString::Compare(protocol, "ssh", JString::kIgnoreCase) == 0)
	{
		*port = kJDefaultSSHPort;
		return true;
	}
	else
	{
		*port = 0;
		return false;
	}
}

/******************************************************************************
 JCheckForNewerVersion

	If it is time to remind the user about a new version, display a message
	offering to open DOWNLOAD_URL retrieved from JStringManager.

	In the background, checks for a newer version of the program by
	downloading VERSION_URL (retrieved from JStringManager), which is
	assumed to contain a version number.  This is matched against the
	VERSION string retrieved from JStringManager.

	If a newer version is available, a reminder is scheduled for the next
	time the program is run, after 1,2,3 months and then every 6 months.

 ******************************************************************************/

void
JCheckForNewerVersion
	(
	JPrefsManager*	prefsMgr,
	const JPrefID&	prefID
	)
{
	auto* updater = jnew JUpdateChecker(prefsMgr, prefID);
	assert( updater != nullptr );

	if (updater->TimeToRemind())
	{
		const JUtf8Byte* map[] =
		{
			"vers", updater->GetLatestVersion().GetBytes()
		};
		const JString msg = JGetString("JRemindNewVersion", map, sizeof(map));
		if (JGetUserNotification()->AskUserYes(msg))
		{
			JGetWebBrowser()->ShowURL(JGetString("DOWNLOAD_URL"));
		}
	}

	updater->CheckForNewerVersion();
}
