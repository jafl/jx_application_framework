/******************************************************************************
 jGlobals_Win32.cpp

	Copyright (C) 2005 John Lindal.

 ******************************************************************************/

#include <jGlobals.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <shlobj.h>
#include <jAssert.h>

/******************************************************************************
 JGetDataDirectories

	Returns the full paths of the system and user directories for the given
	program signature.  dirName is appended to each one for convenience.

	Returns false if the user doesn't have an Application Data directory,
	in which case userDir is empty.

	*** Does not check if either directory exists.

 ******************************************************************************/

const JCharacter* kSystemDataFileDir = "/usr/lib/";
const JCharacter* kUserDataFileDir   = "~/.";

bool
JGetDataDirectories
	(
	const JCharacter*	signature,
	const JCharacter*	dirName,
	JString*			sysDir,
	JString*			userDir
	)
{
	const bool ok = JGetDirectoryFromCSIDL(CSIDL_COMMON_APPDATA, sysDir);
	assert( ok );
	*sysDir = JCombinePathAndName(*sysDir, "JX");
	*sysDir = JCombinePathAndName(*sysDir, signature);
	*sysDir = JCombinePathAndName(*sysDir, dirName);

	if (JGetDirectoryFromCSIDL(CSIDL_APPDATA, userDir))
		{
		*userDir = JCombinePathAndName(*userDir, "JX");
		*userDir = JCombinePathAndName(*userDir, signature);
		*userDir = JCombinePathAndName(*userDir, dirName);
		return true;
		}
	else
		{
		userDir->Clear();
		return false;
		}
}
