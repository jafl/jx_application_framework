/******************************************************************************
 jSysUtil_UNIX.cpp

	Routines specific to UNIX.

	Copyright (C) 1997-98 John Lindal.

 ******************************************************************************/

#include "jSysUtil.h"
#include "JStdError.h"
#include "jErrno.h"
#include "JListUtil.h"
#include "jGlobals.h"
#include <unistd.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include "jAssert.h"

/*******************************************************************************
 JUserIsAdmin

 *******************************************************************************/

JBoolean
JUserIsAdmin()
{
	return JUserIsAdmin(getuid());
}

JBoolean
JUserIsAdmin
	(
	const uid_t uid
	)
{
	return JI2B(uid == 0);
}

/******************************************************************************
 JGetHostName

	Returns the name of the machine.

 ******************************************************************************/

JString
JGetHostName()
{
	JUtf8Byte hostName[255];
	const int result = gethostname(hostName, 255);
	assert( result == 0 );
	return JString(hostName, 0);
}

/******************************************************************************
 jGetUserInfo (local)

	Buffer information because LDAP requires server call *every* time.

 ******************************************************************************/

struct jUIDInfo
{
	uid_t		id;
	JString*	userName;
	JString*	realName;
	JString*	homeDirectory;
	JString*	shell;

	void Free()
	{
		jdelete userName;
		jdelete realName;
		jdelete homeDirectory;
		jdelete shell;
	};
};

static JArray<jUIDInfo> theUserInfoMap;

static JListT::CompareResult
jCompareUIDs
	(
	const jUIDInfo& i1,
	const jUIDInfo& i2
	)
{
	return JCompareIndices(i1.id, i2.id);
}

static void
jCleanUserInfoMap()
{
	for (jUIDInfo info : theUserInfoMap)
		{
		info.Free();
		}
}

static JBoolean
jGetUserInfo
	(
	const uid_t uid,
	jUIDInfo*	info
	)
{
	if (theUserInfoMap.IsEmpty())
		{
		theUserInfoMap.SetCompareFunction(jCompareUIDs);
		theUserInfoMap.SetSortOrder(JListT::kSortAscending);
		atexit(jCleanUserInfoMap);
		}

	const jUIDInfo target = { uid, nullptr, nullptr };
	JIndex i;
	if (theUserInfoMap.SearchSorted(target, JListT::kAnyMatch, &i))
		{
		*info = theUserInfoMap.GetElement(i);
		}
	else
		{
		passwd* pwbuf = getpwuid(uid);
		if (pwbuf != nullptr)
			{
			info->userName = jnew JString(pwbuf->pw_name, 0);
			assert( info->userName != nullptr );

			info->realName = jnew JString(pwbuf->pw_gecos, 0);
			assert( info->realName != nullptr );

			info->homeDirectory = jnew JString(pwbuf->pw_dir, 0);
			assert( info->homeDirectory != nullptr );

			info->shell = jnew JString(pwbuf->pw_shell, 0);
			assert( info->shell != nullptr );

			info->id = uid;
			const JBoolean inserted = theUserInfoMap.InsertSorted(*info, kJFalse);
			assert( inserted );
			}
		else
			{
			info->userName = info->realName = info->homeDirectory = info->shell = nullptr;
			}
		}

	return JI2B( info->userName != nullptr );
}

/******************************************************************************
 JGetUserName

	Returns the name of the specified user.

 ******************************************************************************/

JString
JGetUserName()
{
	return JGetUserName(getuid());
}

JString
JGetUserName
	(
	const uid_t uid
	)
{
	jUIDInfo info;
	if (jGetUserInfo(uid, &info))
		{
		return *(info.userName);
		}
	else
		{
		return JString(uid, 0, JString::kForceNoExponent);
		}
}

/******************************************************************************
 JGetUserRealWorldName

	Returns the "real world" name of the specified user.

 ******************************************************************************/

JString
JGetUserRealWorldName()
{
	return JGetUserRealWorldName(getuid());
}

JString
JGetUserRealWorldName
	(
	const uid_t uid
	)
{
	jUIDInfo info;
	if (jGetUserInfo(uid, &info))
		{
		return *((info.realName)->IsEmpty() ?
				 info.userName : info.realName);
		}
	else
		{
		return JString(uid, 0, JString::kForceNoExponent);
		}
}

/******************************************************************************
 JGetUserHomeDirectory

	Returns the home directory of the specified user.

 ******************************************************************************/

JString
JGetUserHomeDirectory()
{
	return JGetUserHomeDirectory(getuid());
}

JString
JGetUserHomeDirectory
	(
	const uid_t uid
	)
{
	jUIDInfo info;
	if (jGetUserInfo(uid, &info))
		{
		return *(info.homeDirectory);
		}
	else
		{
		return JString("/", 0);
		}
}

/******************************************************************************
 JGetUserShell

	Returns the shell of the specified user.

 ******************************************************************************/

JString
JGetUserShell()
{
	return JGetUserShell(getuid());
}

JString
JGetUserShell
	(
	const uid_t uid
	)
{
	jUIDInfo info;
	if (jGetUserInfo(uid, &info))
		{
		return *(info.shell);
		}
	else
		{
		return JString("/bin/sh", 0);
		}
}

/******************************************************************************
 jGetGroupInfo (local)

	Buffer information because LDAP requires server call *every* time.

 ******************************************************************************/

struct jGIDInfo
{
	gid_t		id;
	JString*	groupName;
};

static JArray<jGIDInfo> groupInfoMap;

static JListT::CompareResult
jCompareGIDs
	(
	const jGIDInfo& i1,
	const jGIDInfo& i2
	)
{
	return JCompareIndices(i1.id, i2.id);
}

static JBoolean
jGetGroupInfo
	(
	const gid_t	gid,
	jGIDInfo*	info
	)
{
	if (groupInfoMap.IsEmpty())
		{
		groupInfoMap.SetCompareFunction(jCompareGIDs);
		groupInfoMap.SetSortOrder(JListT::kSortAscending);
		}

	const jGIDInfo target = { gid, nullptr };
	JIndex i;
	if (groupInfoMap.SearchSorted(target, JListT::kAnyMatch, &i))
		{
		*info = groupInfoMap.GetElement(i);
		}
	else
		{
		group* grpbuf = getgrgid(gid);
		if (grpbuf != nullptr)
			{
			info->groupName = jnew JString(grpbuf->gr_name, 0);
			assert( info->groupName != nullptr );

			info->id = gid;
			const JBoolean inserted = groupInfoMap.InsertSorted(*info, kJFalse);
			assert( inserted );
			}
		else
			{
			info->groupName = nullptr;
			}
		}

	return JI2B( info->groupName != nullptr );
}

/******************************************************************************
 JGetGroupName

	Returns the name of the specified user.

 ******************************************************************************/

JString
JGetGroupName()
{
	return JGetGroupName(getgid());
}

JString
JGetGroupName
	(
	const gid_t gid
	)
{
	jGIDInfo info;
	if (jGetGroupInfo(gid, &info))
		{
		return *(info.groupName);
		}
	else
		{
		return JString(gid, 0, JString::kForceNoExponent);
		}
}

/******************************************************************************
 JUNIXSocketExists

	Returns kJTrue if pathName is the pathname of an existing unix socket,
	kJFalse otherwise.

 *****************************************************************************/

JBoolean
JUNIXSocketExists
	(
	const JString& pathName
	)
{
	ACE_stat info;

	return JConvertToBoolean(
			ACE_OS::lstat(pathName.GetBytes(), &info) == 0 &&
			ACE_OS::stat( pathName.GetBytes(), &info) == 0 &&
#ifdef _J_OLD_SUNOS
			S_ISFIFO(info.st_mode)
#else
			S_ISSOCK(info.st_mode)
#endif
			);
}

/******************************************************************************
 JCreatePipe

	Opens a pair of file descriptors.  fd[0] is for reading.  fd[1] is for
	writing.

 *****************************************************************************/

JError
JCreatePipe
	(
	int fd[2]
	)
{
	jclear_errno();
	if (pipe(fd) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EFAULT)
		{
		return JSegFault();
		}
	else if (err == EMFILE || err == ENFILE)
		{
		return JTooManyDescriptorsOpen();
		}
	else
		{
		return JUnexpectedError(err);
		}
}
