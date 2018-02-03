/******************************************************************************
 cbUtil.cpp

	Functions used by Code Crusader.

	Copyright (C) 2006 John Lindal.

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
	const JCharacter* origFullName,
	const JCharacter* newFullName
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

	if (!JString::IsEmpty(newFullName))
		{
		(CBGetDocumentManager())->FileRenamed(origFullName, newFullName);
		}
}
