/******************************************************************************
 jGlobals_UNIX.cpp

	Copyright (C) 2005 John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/jGlobals.h"
#include "jx-af/jcore/jDirUtil.h"
#include "jx-af/jcore/jFileUtil.h"
#include "jx-af/jcore/jAssert.h"

/******************************************************************************
 JGetDataDirectories

	Returns the full paths of the system and user directories for the given
	program signature.  dirName is appended to each one for convenience.

	Returns false if the user doesn't have a home directory, in which
	case userDir is empty.

	*** Does not check if either directory exists.

 ******************************************************************************/

#ifdef _J_MACOS
const JUtf8Byte* kSystemDataFileDir = "/usr/local/lib/";
#else
const JUtf8Byte* kSystemDataFileDir = "/usr/lib/";
#endif

const JUtf8Byte* kUserDataFileDir = "~/.";

bool
JGetDataDirectories
	(
	const JUtf8Byte*	signature,
	const JUtf8Byte*	dirName,
	JString*			sysDir,
	JString*			userDir
	)
{
	*sysDir  = kSystemDataFileDir;
	*sysDir += signature;
	*sysDir  = JCombinePathAndName(*sysDir, JString(dirName, JString::kNoCopy));

	JString relName(kUserDataFileDir);
	relName += signature;
	relName  = JCombinePathAndName(relName, JString(dirName, JString::kNoCopy));
	return JExpandHomeDirShortcut(relName, userDir);
}
