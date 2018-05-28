/******************************************************************************
 jGlobals_UNIX.cpp

	Copyright (C) 2005 John Lindal.

 ******************************************************************************/

#include <jGlobals.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jAssert.h>

/******************************************************************************
 JGetDataDirectories

	Returns the full paths of the system and user directories for the given
	program signature.  dirName is appended to each one for convenience.

	Returns kJFalse if the user doesn't have a home directory, in which
	case userDir is empty.

	*** Does not check if either directory exists.

 ******************************************************************************/

#ifdef _J_OSX
const JUtf8Byte* kSystemDataFileDir = "/usr/local/lib/";
#else
const JUtf8Byte* kSystemDataFileDir = "/usr/lib/";
#endif

const JUtf8Byte* kUserDataFileDir = "~/.";

JBoolean
JGetDataDirectories
	(
	const JString&	signature,
	const JString&	dirName,
	JString*		sysDir,
	JString*		userDir
	)
{
	*sysDir  = kSystemDataFileDir;
	*sysDir += signature;
	*sysDir  = JCombinePathAndName(*sysDir, dirName);

	JString relName(kUserDataFileDir, 0);
	relName         += signature;
	relName          = JCombinePathAndName(relName, dirName);
	return JExpandHomeDirShortcut(relName, userDir);
}
