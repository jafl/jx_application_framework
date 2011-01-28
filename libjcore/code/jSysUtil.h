/******************************************************************************
 jSysUtil.h

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jSysUtil
#define _H_jSysUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JError.h>
#include <unistd.h>
#include <sys/types.h>
#include <ace/OS_NS_sys_stat.h>

class JString;

JString	JGetHostName();

JString	JGetUserName();
JString	JGetUserName(const uid_t uid);

JString	JGetUserRealWorldName();
JString	JGetUserRealWorldName(const uid_t uid);

JString	JGetUserHomeDirectory();
JString	JGetUserHomeDirectory(const uid_t uid);

JString	JGetUserShell();
JString	JGetUserShell(const uid_t uid);

JString	JGetGroupName();
JString	JGetGroupName(const gid_t gid);

JBoolean	JUserIsAdmin();
JBoolean	JUserIsAdmin(const uid_t uid);

JBoolean	JUNIXSocketExists(const JCharacter* pathName);

JError	JCreatePipe(int fd[2]);

void	JCheckSiteName(const JCharacter* encSiteSuffix, const JCharacter siteCode,
					   const JCharacter* map[], const JSize size);

#endif
