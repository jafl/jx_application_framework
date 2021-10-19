/******************************************************************************
 jWebUtil.cpp

	System independent URL utilities.

	Copyright (C) 2008-10 by John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/jWebUtil.h"
#include "jx-af/jcore/JVersionSocket.h"
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

using VersionSocket    = JVersionSocket<ACE_SOCK_STREAM>;
using VersionConnector = ACE_Connector<VersionSocket, ACE_SOCK_CONNECTOR>;

void
JCheckForNewerVersion
	(
	JPrefsManager*	prefsMgr,
	const JPrefID&	prefID
	)
{
	const JString& versionURL = JGetString("VERSION_URL");

	JString protocol, host, path;
	JIndex port;
	if (!JParseURL(versionURL, &protocol, &host, &port, &path))
	{
		std::cerr << "unable to parse url: " << versionURL << std::endl;
		return;
	}

	if (port == 0 && !JGetDefaultPort(protocol, &port))
	{
		std::cerr << "unknown protocol in versionURL: " << versionURL << std::endl;
		return;
	}

	auto* socket = new VersionSocket(host, path, prefsMgr, prefID);
	assert( socket != nullptr );

	const JString vers = socket->GetLatestVersion();
	if (socket->TimeToRemind())
	{
		const JUtf8Byte* map[] =
		{
			"vers", vers.GetBytes(),
			"site", host.GetBytes()
		};
		const JString msg = JGetString("JRemindNewVersion", map, sizeof(map));
		if (JGetUserNotification()->AskUserYes(msg))
		{
			JGetWebBrowser()->ShowURL(JGetString("DOWNLOAD_URL"));
		}
	}

	if (socket->TimeToCheck())
	{
		ACE_INET_Addr addr(port, host.GetBytes());

		auto* connector = new VersionConnector;
		assert( connector != nullptr );

		if (connector->connect(socket, addr, ACE_Synch_Options::asynch) == -1 &&
			jerrno() != EAGAIN)
		{
//			std::cerr << "unable to open socket: " << versionURL << std::endl;
			delete connector;
		}
	}
	else
	{
		delete socket;
	}
}
