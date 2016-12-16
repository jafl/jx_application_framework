/******************************************************************************
 jWebUtil.cpp

	System independent URL utilities.

	Copyright (C) 2008-10 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jWebUtil.h>
#include <JVersionSocket.h>
#include <JRegex.h>
#include <JWebBrowser.h>
#include <jGlobals.h>
#include <ace/Connector.h>
#include <ace/SOCK_Connector.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 JIsURL

	Renames the specified directory.

 ******************************************************************************/

JBoolean
JIsURL
	(
	const JString& s
	)
{
	return JI2B(strstr(s, "://") != NULL);
}

/******************************************************************************
 JParseURL

	Extract host:post and file from protocol://host[:port]/[path]

	If the port is not specified, it is set to zero.  The caller must
	decided on a sensible default, based on the protocol.  One option is to
	call JGetDefaultPort().

 ******************************************************************************/

static const JRegex urlPattern = "([^:]+)://([^/:]+)(?::([0-9]+))?(.*)";

JBoolean
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

	JArray<JIndexRange> matchList;
	if (urlPattern.Match(url, &matchList))
		{
		protocol->Set(url, matchList.GetElement(2));
		host->Set(url, matchList.GetElement(3));

		JIndexRange r = matchList.GetElement(4);
		if (!r.IsEmpty())
			{
			const JString s(url, r);
			if (!s.ConvertToUInt(port))
				{
				*port = 0;
				}
			}
		else
			{
			*port = 0;
			}

		r = matchList.GetElement(5);
		if (!r.IsEmpty())
			{
			path->Set(url, r);
			}
		else
			{
			*path = "/";
			}

		return kJTrue;
		}

	return kJFalse;
}

/******************************************************************************
 JGetDefaultPort

	Returns kJTrue if it recognizes the protocol.

 ******************************************************************************/

JBoolean
JGetDefaultPort
	(
	const JString&	protocol,
	JIndex*			port
	)
{
	if (strcasecmp(protocol, "http") == 0)
		{
		*port = kJDefaultHTTPPort;
		return kJTrue;
		}
	else if (strcasecmp(protocol, "https") == 0)
		{
		*port = kJDefaultHTTPSPort;
		return kJTrue;
		}
	else if (strcasecmp(protocol, "ftp") == 0)
		{
		*port = kJDefaultFTPPort;
		return kJTrue;
		}
	else if (strcasecmp(protocol, "sftp") == 0)
		{
		*port = kJDefaultSFTPPort;
		return kJTrue;
		}
	else if (strcasecmp(protocol, "ssh") == 0)
		{
		*port = kJDefaultSSHPort;
		return kJTrue;
		}
	else
		{
		*port = 0;
		return kJFalse;
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

typedef JVersionSocket<ACE_SOCK_STREAM>						VersionSocket;
typedef ACE_Connector<VersionSocket, ACE_SOCK_CONNECTOR>	VersionConnector;

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

	VersionSocket* socket = new VersionSocket(host, path, prefsMgr, prefID);
	assert( socket != NULL );

	const JString vers = socket->GetLatestVersion();
	if (socket->TimeToRemind())
		{
		const JUtf8Byte* map[] =
			{
			"vers", vers,
			"site", host
			};
		const JString msg = JGetString("JRemindNewVersion", map, sizeof(map));
		if ((JGetUserNotification())->AskUserYes(msg))
			{
			(JGetWebBrowser())->ShowURL(JGetString("DOWNLOAD_URL"));
			}
		}

	if (socket->TimeToCheck())
		{
		ACE_INET_Addr addr(port, host);

		VersionConnector* connector = new VersionConnector;
		assert( connector != NULL );

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
