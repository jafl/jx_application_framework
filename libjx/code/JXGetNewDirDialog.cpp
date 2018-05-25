/******************************************************************************
 JXGetNewDirDialog.cpp

	BASE CLASS = JXGetStringDialog

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

#include <JXGetNewDirDialog.h>
#include <JXInputField.h>
#include <JXChooseSaveFile.h>
#include <jXGlobals.h>
#include <jDirUtil.h>
#include <jAssert.h>

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
	const JBoolean	modal
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

JBoolean
JXGetNewDirDialog::OKToDeactivate()
{
	if (!JXGetStringDialog::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	const JString pathName = GetNewDirName();
	if (JDirectoryExists(pathName))
		{
		(JGetUserNotification())->ReportError(JGetString("DirectoryExists::JXGlobal"));
		return kJFalse;
		}
	else if (JNameUsed(pathName))
		{
		(JGetUserNotification())->ReportError(JGetString("NameUsed::JXGetNewDirDialog"));
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}
