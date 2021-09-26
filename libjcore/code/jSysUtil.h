/******************************************************************************
 jSysUtil.h

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_jSysUtil
#define _H_jSysUtil

#include "jx-af/jcore/JError.h"
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

bool	JUserIsAdmin();
bool	JUserIsAdmin(const uid_t uid);

bool	JUNIXSocketExists(const JString& pathName);

JError	JCreatePipe(int fd[2]);

#endif
