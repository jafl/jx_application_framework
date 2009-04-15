/******************************************************************************
 jGlobals_UNIX.cpp

	Copyright © 2005 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
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

const JCharacter* kSystemDataFileDir = "/usr/lib/";
const JCharacter* kUserDataFileDir   = "~/.";

JBoolean
JGetDataDirectories
	(
	const JCharacter*	signature,
	const JCharacter*	dirName,
	JString*			sysDir,
	JString*			userDir
	)
{
	*sysDir  = kSystemDataFileDir;
	*sysDir += signature;
	*sysDir  = JCombinePathAndName(*sysDir, dirName);

	JString relName  = kUserDataFileDir;
	relName         += signature;
	relName          = JCombinePathAndName(relName, dirName);
	return JExpandHomeDirShortcut(relName, userDir);
}
