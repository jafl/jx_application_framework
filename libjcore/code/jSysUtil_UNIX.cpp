/******************************************************************************
 jSysUtil_UNIX.cpp

	Routines specific to UNIX.

	Copyright © 1997-98 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jSysUtil.h>
#include <JStdError.h>
#include <jErrno.h>
#include <JOrderedSetUtil.h>
#include <jGlobals.h>
#include <unistd.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kWrongSiteID = "WrongSite::jSysUtil";

/*******************************************************************************
 JUserIsAdmin

 *******************************************************************************/

JBoolean
JUserIsAdmin()
{
	return JI2B(getuid() == 0);
}

/******************************************************************************
 JGetHostName

	Returns the name of the machine.

 ******************************************************************************/

JString
JGetHostName()
{
	JCharacter hostName[255];
	const int result = gethostname(hostName, 255);
	assert( result == 0 );
	return JString(hostName);
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
};

static JArray<jUIDInfo> userInfoMap;

#define JTemplateType jUIDInfo
#include <JArray.tmpls>
#undef JTemplateType

static JOrderedSetT::CompareResult
jCompareUIDs
	(
	const jUIDInfo& i1,
	const jUIDInfo& i2
	)
{
	return JCompareIndices(i1.id, i2.id);
}

static JBoolean
jGetUserInfo
	(
	const uid_t uid,
	jUIDInfo*	info
	)
{
	if (userInfoMap.IsEmpty())
		{
		userInfoMap.SetCompareFunction(jCompareUIDs);
		userInfoMap.SetSortOrder(JOrderedSetT::kSortAscending);
		}

	const jUIDInfo target = { uid, NULL, NULL };
	JIndex i;
	if (userInfoMap.SearchSorted(target, JOrderedSetT::kAnyMatch, &i))
		{
		*info = userInfoMap.GetElement(i);
		}
	else
		{
		passwd* pwbuf = getpwuid(uid);
		if (pwbuf != NULL)
			{
			info->userName = new JString(pwbuf->pw_name);
			assert( info->userName != NULL );

			info->realName = new JString(pwbuf->pw_gecos);
			assert( info->realName != NULL );

			info->id = uid;
			const JBoolean inserted = userInfoMap.InsertSorted(*info, kJFalse);
			assert( inserted );
			}
		else
			{
			info->userName = info->realName = NULL;
			}
		}

	return JI2B( info->userName != NULL );
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
 jGetGroupInfo (local)

	Buffer information because LDAP requires server call *every* time.

 ******************************************************************************/

struct jGIDInfo
{
	gid_t		id;
	JString*	groupName;
};

static JArray<jGIDInfo> groupInfoMap;

#define JTemplateType jGIDInfo
#include <JArray.tmpls>
#undef JTemplateType

static JOrderedSetT::CompareResult
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
		groupInfoMap.SetSortOrder(JOrderedSetT::kSortAscending);
		}

	const jGIDInfo target = { gid, NULL };
	JIndex i;
	if (groupInfoMap.SearchSorted(target, JOrderedSetT::kAnyMatch, &i))
		{
		*info = groupInfoMap.GetElement(i);
		}
	else
		{
		group* grpbuf = getgrgid(gid);
		if (grpbuf != NULL)
			{
			info->groupName = new JString(grpbuf->gr_name);
			assert( info->groupName != NULL );

			info->id = gid;
			const JBoolean inserted = groupInfoMap.InsertSorted(*info, kJFalse);
			assert( inserted );
			}
		else
			{
			info->groupName = NULL;
			}
		}

	return JI2B( info->groupName != NULL );
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
	const JCharacter* pathName
	)
{
	ACE_stat info;

	return JConvertToBoolean(
			ACE_OS::lstat(pathName, &info) == 0 &&
			ACE_OS::stat( pathName, &info) == 0 &&
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

/******************************************************************************
 JCheckSiteName

	Exits if program is not running on the specified site.

	map must define:  site, name, url
	*** site must be first!

 ******************************************************************************/

void
JCheckSiteName
	(
	const JCharacter*	encSiteSuffix,
	const JCharacter	siteCode,
	const JCharacter*	map[],
	const JSize			size
	)
{
	JString siteSuffix = encSiteSuffix;
	const JSize len    = siteSuffix.GetLength();
	for (JIndex i=1; i<=len; i++)
		{
		siteSuffix.SetCharacter(i, siteSuffix.GetCharacter(i) ^ siteCode);
		}

	map[1] = siteSuffix.GetCString();

	if (!(JGetHostName()).EndsWith(siteSuffix, kJFalse))
		{
		const JString msg = JGetString(kWrongSiteID, map, size);
		(JGetUserNotification())->DisplayMessage(msg);
		exit(0);
		}
}
