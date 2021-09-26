/******************************************************************************
 JXGetNewDirDialog.cpp

	BASE CLASS = JXGetStringDialog

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

#include "jx-af/jx/JXGetNewDirDialog.h"
#include "jx-af/jx/JXInputField.h"
#include "jx-af/jx/JXChooseSaveFile.h"
#include "jx-af/jx/jXGlobals.h"
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXGetNewDirDialog::JXGetNewDirDialog
	(
	JXDirector*		supervisor,
	const JString&	windowTitle,
	const JString&	prompt,
	const JString&	initialName,
	const JString&	basePath,
	const bool	modal
	)
	:
	JXGetStringDialog(supervisor, windowTitle, prompt, initialName, modal),
	itsBasePath(basePath)
{
	assert( JDirectoryExists(basePath) );

	GetInputField()->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXGetNewDirDialog::~JXGetNewDirDialog()
{
}

/******************************************************************************
 GetNewDirName

 ******************************************************************************/

JString
JXGetNewDirDialog::GetNewDirName()
	const
{
	JString dirName = GetString();
	if (JIsRelativePath(dirName))
	{
		dirName = JCombinePathAndName(itsBasePath, dirName);
	}
	return dirName;
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
JXGetNewDirDialog::OKToDeactivate()
{
	if (!JXGetStringDialog::OKToDeactivate())
	{
		return false;
	}
	else if (Cancelled())
	{
		return true;
	}

	const JString pathName = GetNewDirName();
	if (JDirectoryExists(pathName))
	{
		JGetUserNotification()->ReportError(JGetString("DirectoryExists::JXGlobal"));
		return false;
	}
	else if (JNameUsed(pathName))
	{
		JGetUserNotification()->ReportError(JGetString("NameUsed::JXGetNewDirDialog"));
		return false;
	}
	else
	{
		return true;
	}
}
