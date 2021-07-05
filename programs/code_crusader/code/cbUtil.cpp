/******************************************************************************
 cbUtil.cpp

	Functions used by Code Crusader.

	Copyright © 2006 John Lindal.

 ******************************************************************************/

#include "CBDocumentManager.h"
#include "cbUtil.h"
#include "cbGlobals.h"
#include <jFileUtil.h>
#include <jAssert.h>

/******************************************************************************
 CBCleanUpAfterRename

 ******************************************************************************/

void
CBCleanUpAfterRename
	(
	const JString& origFullName,
	const JString& newFullName
	)
{
	JString backupName = origFullName;
	backupName        += "~";
	JRemoveFile(backupName);

	JString path, name;
	JSplitPathAndName(origFullName, &path, &name);
	const JString safetyName = path + "#" + name + "#";
	JRemoveFile(safetyName);
	const JString crashName = path + "##" + name + "##";
	JRemoveFile(crashName);

	if (!newFullName.IsEmpty())
		{
		CBGetDocumentManager()->FileRenamed(origFullName, newFullName);
		}
}
