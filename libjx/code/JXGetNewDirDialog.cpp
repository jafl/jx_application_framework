/******************************************************************************
 JXGetNewDirDialog.cpp

	BASE CLASS = JXGetStringDialog

	Copyright (C) 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JXGetNewDirDialog.h>
#include <JXInputField.h>
#include <JXChooseSaveFile.h>
#include <jXGlobals.h>
#include <jDirUtil.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kDirectoryExistsID = "DirectoryExists::JXGetNewDirDialog";
static const JCharacter* kNameUsedID        = "NameUsed::JXGetNewDirDialog";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXGetNewDirDialog::JXGetNewDirDialog
	(
	JXDirector*			supervisor,
	const JCharacter*	windowTitle,
	const JCharacter*	prompt,
	const JCharacter*	initialName,
	const JCharacter*	basePath,
	const JBoolean		modal
	)
	:
	JXGetStringDialog(supervisor, windowTitle, prompt, initialName, modal),
	itsBasePath(basePath)
{
	assert( JDirectoryExists(basePath) );

	GetInputField()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
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
		(JGetUserNotification())->ReportError(JGetString(kDirectoryExistsID));
		return kJFalse;
		}
	else if (JNameUsed(pathName))
		{
		(JGetUserNotification())->ReportError(JGetString(kNameUsedID));
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}
