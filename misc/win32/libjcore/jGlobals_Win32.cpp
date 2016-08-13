/******************************************************************************
 jGlobals_Win32.cpp

	Copyright © 2005 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jGlobals.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <shlobj.h>
#include <jAssert.h>

/******************************************************************************
 JGetDataDirectories

	Returns the full paths of the system and user directories for the given
	program signature.  dirName is appended to each one for convenience.

	Returns kJFalse if the user doesn't have an Application Data directory,
	in which case userDir is empty.

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
	const JBoolean ok = JGetDirectoryFromCSIDL(CSIDL_COMMON_APPDATA, sysDir);
	assert( ok );
	*sysDir = JCombinePathAndName(*sysDir, "JX");
	*sysDir = JCombinePathAndName(*sysDir, signature);
	*sysDir = JCombinePathAndName(*sysDir, dirName);

	if (JGetDirectoryFromCSIDL(CSIDL_APPDATA, userDir))
		{
		*userDir = JCombinePathAndName(*userDir, "JX");
		*userDir = JCombinePathAndName(*userDir, signature);
		*userDir = JCombinePathAndName(*userDir, dirName);
		return kJTrue;
		}
	else
		{
		userDir->Clear();
		return kJFalse;
		}
}
