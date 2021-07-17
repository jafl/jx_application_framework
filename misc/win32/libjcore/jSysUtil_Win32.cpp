/******************************************************************************
 jSysUtil_Win32.cpp

	Routines specific to Windows.

	Copyright (C) 2005 John Lindal.

 ******************************************************************************/

#include <jSysUtil.h>
#include <JStdError.h>
#include <jErrno.h>
#include <jMissingProto.h>

#define SECURITY_WIN32
#include <windows.h>
#include <security.h>
#include <lmcons.h>

/*******************************************************************************
 JUserIsAdmin

 *******************************************************************************/

bool
JUserIsAdmin()
{
	HANDLE tokenHandle;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
		{
		return false;
		}

	DWORD returnLength;
	GetTokenInformation(tokenHandle, TokenGroups, nullptr, 0, &returnLength);
	if (returnLength > 65535)
		{
		CloseHandle(tokenHandle);
		return false;
		}

	TOKEN_GROUPS* groupList = (TOKEN_GROUPS*) malloc(returnLength);
	if (groupList == nullptr)
		{
		CloseHandle(tokenHandle);
		return false;
		}

	if (!GetTokenInformation(tokenHandle, TokenGroups,
							 groupList, returnLength, &returnLength))
		{
		CloseHandle(tokenHandle);
		free(groupList);
		return false;
		}
	CloseHandle(tokenHandle);

	SID_IDENTIFIER_AUTHORITY siaNtAuth = SECURITY_NT_AUTHORITY;
	PSID adminSid;
	if (!AllocateAndInitializeSid(&siaNtAuth, 2, SECURITY_BUILTIN_DOMAIN_RID,
								  DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
								  &adminSid))
		{
		free(groupList);
		return false;
		}

	bool found = false;
	for (DWORD i = 0; i < groupList->GroupCount; i++)
		{
		if (EqualSid(adminSid, groupList->Groups[i].Sid))
			{
			found = true;
			break;
			}
		}

	FreeSid(adminSid);
	free(groupList);

	return found;
}

/******************************************************************************
 JGetHostName

	Returns the name of the machine.  If gethostname() doesn't work,
	we could use GetComputerNameEx() instead.

 ******************************************************************************/

JString
JGetHostName()
{
	JCharacter hostName[256];
	const int result = gethostname(hostName, 255);
	assert( result == 0 );
	return JString(hostName);
}

/******************************************************************************
 JGetUserName

	Returns the name of the specified user.

 ******************************************************************************/

JString
JGetUserName()
{
	JCharacter buf[ UNLEN + 2 ];
	DWORD len = UNLEN + 1;
	if (GetUserName(buf, &len))
		{
		return buf;
		}
	else
		{
		return "";
		}
}

JString
JGetUserName
	(
	const uid_t uid
	)
{
	assert( uid == 0 );
	return JGetUserName();
}

/******************************************************************************
 JGetUserRealWorldName

	Returns the "real world" name of the specified user.

 ******************************************************************************/

JString
JGetUserRealWorldName()
{
	return JGetUserName();
}

JString
JGetUserRealWorldName
	(
	const uid_t uid
	)
{
	assert( uid == 0 );
	return JGetUserRealWorldName();
}

/******************************************************************************
 JGetGroupName

	Returns the name of the specified user.

 ******************************************************************************/

JString
JGetGroupName()
{
	return "";
}

JString
JGetGroupName
	(
	const gid_t gid
	)
{
	assert( gid == 0 );
	return "";
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
	if (_pipe(fd, 4096, _O_BINARY) == 0)
		{
		return JNoError();
		}

	const int err = jerrno();
	if (err == EMFILE || err == ENFILE)
		{
		return JTooManyDescriptorsOpen();
		}
	else
		{
		return JUnexpectedError(err);
		}
}
