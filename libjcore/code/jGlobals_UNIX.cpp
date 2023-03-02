/******************************************************************************
 jGlobals_UNIX.cpp

	Copyright (C) 2005 John Lindal.

 ******************************************************************************/

#include "jGlobals.h"
#include "jFileUtil.h"
#include "JProcess.h"
#include "jStreamUtil.h"
#include "jAssert.h"

/******************************************************************************
 JGetDataDirectories

	Returns the full paths of the system and user directories for the given
	program signature.  dirName is appended to each one for convenience.

	Returns false if the user doesn't have a home directory, in which
	case userDir is empty.

	*** Does not check if either directory exists.

 ******************************************************************************/

static const JString kDefaultSystemRoot("/usr/local", JString::kNoCopy);
static const JString kSystemDataFileDir("share/", JString::kNoCopy);
static const JUtf8Byte* kUserDataFileDir = "~/.";

static JString systemDataFileDir;

bool
JGetDataDirectories
	(
	const JUtf8Byte*	signature,
	const JUtf8Byte*	dirName,
	JString*			sysDir,
	JString*			userDir
	)
{
	if (systemDataFileDir.IsEmpty())
	{
		if (JProgramAvailable(JString("brew", JString::kNoCopy)))
		{
			const JUtf8Byte* argv[] = { "brew", "--prefix", nullptr };
			JProcess* p;
			int fd;
			const JError err =
				JProcess::Create(
					&p, argv, sizeof(argv),
					kJIgnoreConnection, nullptr,
					kJCreatePipe, &fd);

			if (err.OK())
			{
				JReadAll(fd, &systemDataFileDir);
				systemDataFileDir.TrimWhitespace();
				if (!systemDataFileDir.IsEmpty())
				{
					systemDataFileDir = JCombinePathAndName(systemDataFileDir, kSystemDataFileDir);
				}

				jdelete p;
			}
		}
		else
		{
			JString s1, s2;
			const JError err = JGetSymbolicLinkTarget(JString("/proc/self/exe", JString::kNoCopy), &s1);
			if (err.OK())
			{
				JSplitPathAndName(s1, &systemDataFileDir, &s2);
				systemDataFileDir = JCombinePathAndName(systemDataFileDir, JString("..", JString::kNoCopy));
				systemDataFileDir = JCombinePathAndName(systemDataFileDir, kSystemDataFileDir);
			}
		}

		if (systemDataFileDir.IsEmpty())
		{
			systemDataFileDir = JCombinePathAndName(kDefaultSystemRoot, kSystemDataFileDir);
		}
	}

	*sysDir  = systemDataFileDir;
	*sysDir += signature;
	*sysDir  = JCombinePathAndName(*sysDir, JString(dirName, JString::kNoCopy));

	JString relName(kUserDataFileDir);
	relName += signature;
	relName  = JCombinePathAndName(relName, JString(dirName, JString::kNoCopy));
	return JExpandHomeDirShortcut(relName, userDir);
}
