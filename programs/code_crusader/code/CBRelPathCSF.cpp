/******************************************************************************
 CBRelPathCSF.cpp

	BASE CLASS = JXChooseSaveFile

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBRelPathCSF.h"
#include "CBRPChooseFileDialog.h"
#include "CBRPChoosePathDialog.h"
#include "CBProjectDocument.h"
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	We take CBProjectDocument instead of a file path against the day when
	"Save as..." is supported.

 ******************************************************************************/

CBRelPathCSF::CBRelPathCSF
	(
	CBProjectDocument* doc
	)
	:
	JXChooseSaveFile()
{
	itsFileDialog = NULL;
	itsPathDialog = NULL;
	itsPathType   = kProjectRelative;
	itsDoc        = doc;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBRelPathCSF::~CBRelPathCSF()
{
}

/******************************************************************************
 ChooseRelFile

 ******************************************************************************/

JBoolean
CBRelPathCSF::ChooseRelFile
	(
	const JCharacter*	prompt,
	const JCharacter*	instructions,
	const JCharacter*	origName,
	JString*			name
	)
{
	const JString fullOrigName = PrepareForChoose(origName);
	if (JXChooseSaveFile::ChooseFile(prompt, instructions, fullOrigName, name))
		{
		*name = ConvertToRelativePath(*name);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 CreateChooseFileDialog (virtual protected)

 ******************************************************************************/

JXChooseFileDialog*
CBRelPathCSF::CreateChooseFileDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JBoolean		allowSelectMultiple,
	const JCharacter*	origName,
	const JCharacter*	message
	)
{
	itsFileDialog =
		CBRPChooseFileDialog::Create(supervisor, dirInfo, fileFilter,
									 allowSelectMultiple,
									 itsPathType, origName, message);
	return itsFileDialog;
}

/******************************************************************************
 ChooseRelRPath

 ******************************************************************************/

JBoolean
CBRelPathCSF::ChooseRelRPath
	(
	const JCharacter*	prompt,
	const JCharacter*	instructions,
	const JCharacter*	origPath,
	JString*			newPath
	)
{
	const JString startPath = PrepareForChoose(origPath);
	if (JXChooseSaveFile::ChooseRPath(prompt, instructions, startPath, newPath))
		{
		*newPath = ConvertToRelativePath(*newPath);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ChooseRelRWPath

 ******************************************************************************/

JBoolean
CBRelPathCSF::ChooseRelRWPath
	(
	const JCharacter*	prompt,
	const JCharacter*	instructions,
	const JCharacter*	origPath,
	JString*			newPath
	)
{
	const JString startPath = PrepareForChoose(origPath);
	if (JXChooseSaveFile::ChooseRWPath(prompt, instructions, startPath, newPath))
		{
		*newPath = ConvertToRelativePath(*newPath);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PrepareForChoose (private)

 ******************************************************************************/

JString
CBRelPathCSF::PrepareForChoose
	(
	const JCharacter* origName
	)
{
	CalcPathType(origName, &itsPathType);

	JString startPath;
	if (JString::IsEmpty(origName))
		{
		startPath = GetProjectPath();
		}
	else if (!JString::IsEmpty(origName) &&
			 !JConvertToAbsolutePath(origName, GetProjectPath(), &startPath))
		{
		startPath = origName;
		if (JIsRelativePath(startPath))
			{
			startPath.Prepend(GetProjectPath());
			}
		}

	return startPath;
}

/******************************************************************************
 CalcPathType

 ******************************************************************************/

// private -- returns void to avoid conflict with static version

void
CBRelPathCSF::CalcPathType
	(
	const JCharacter*	path,
	PathType*			type
	)
	const
{
	if (!JString::IsEmpty(path))
		{
		*type = CalcPathType(path);
		}
}

// static

CBRelPathCSF::PathType
CBRelPathCSF::CalcPathType
	(
	const JCharacter* path
	)
{
	assert( !JString::IsEmpty(path) );

	if (path[0] == '~')
		{
		return kHomeRelative;
		}
	else if (JIsAbsolutePath(path))
		{
		return kAbsolutePath;
		}
	else
		{
		return kProjectRelative;
		}
}

/******************************************************************************
 CreateChoosePathDialog (virtual protected)

 ******************************************************************************/

JXChoosePathDialog*
CBRelPathCSF::CreateChoosePathDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JBoolean		selectOnlyWritable,
	const JCharacter*	message
	)
{
	itsPathDialog =
		CBRPChoosePathDialog::Create(supervisor, dirInfo, fileFilter,
									 selectOnlyWritable,
									 itsPathType, message);
	return itsPathDialog;
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
CBRelPathCSF::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsPathType = itsFileDialog->GetPathType();
			}
		itsFileDialog = NULL;
		}

	else if (sender == itsPathDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsPathType = itsPathDialog->GetPathType();
			}
		itsPathDialog = NULL;
		}

	JXChooseSaveFile::Receive(sender, message);
}

/******************************************************************************
 GetProjectPath

 ******************************************************************************/

const JString&
CBRelPathCSF::GetProjectPath()
	const
{
	return itsDoc->GetFilePath();
}

/******************************************************************************
 ConvertToRelativePath

 ******************************************************************************/

JString
CBRelPathCSF::ConvertToRelativePath
	(
	const JCharacter* fullPath
	)
{
	return ConvertToRelativePath(fullPath, GetProjectPath(), itsPathType);
}

/******************************************************************************
 ConvertToRelativePath (static)

 ******************************************************************************/

JString
CBRelPathCSF::ConvertToRelativePath
	(
	const JCharacter*	fullPath,
	const JCharacter*	projPath,
	const PathType		pathType
	)
{
	if (!JFileExists(fullPath) && !JDirectoryExists(fullPath))
		{
		return fullPath;
		}

	if (pathType == kProjectRelative)
		{
		return JConvertToRelativePath(fullPath, projPath);
		}
	else if (pathType == kHomeRelative)
		{
		JString path, trueHome;
		if (JGetHomeDirectory(&path) &&
			JGetTrueName(path, &trueHome))
			{
			path = JConvertToRelativePath(fullPath, trueHome);
			if (path.BeginsWith("." ACE_DIRECTORY_SEPARATOR_STR))
				{
				path.SetCharacter(1, '~');
				}
			else if (JIsRelativePath(path))
				{
				path.Prepend("~" ACE_DIRECTORY_SEPARATOR_STR);
				}
			return path;
			}
		}

	return fullPath;
}
