/******************************************************************************
 JTextChooseSaveFile.cpp

	Class to let user select and save files using the console.

	*** Security note:
		The user is allowed to execute *any* command via /bin/sh by starting
		the line with an exclamation point.  Never call JTextChooseSaveFile
		objects while in suid root mode.

	Copyright (C) 1994-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JTextChooseSaveFile.h>
#include <jCommandLine.h>

#include <JString.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <stdlib.h>
#include <jGlobals.h>
#include <jAssert.h>

const JCharacter* kDoneStr   = "done";
const JCharacter* kCancelStr = "cancel";

/******************************************************************************
 Constructor

 ******************************************************************************/

JTextChooseSaveFile::JTextChooseSaveFile()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTextChooseSaveFile::~JTextChooseSaveFile()
{
}

/******************************************************************************
 ChooseFile

	Displays the prompt and asks the user to choose a file.

	Returns kJFalse if user cancels.

 ******************************************************************************/

JBoolean
JTextChooseSaveFile::ChooseFile
	(
	const JCharacter*	prompt,
	const JCharacter*	instructions,
	JString*			fullName
	)
{
	while (1)
		{
		if (!JStringEmpty(instructions))
			{
			std::cout << std::endl;
			std::cout << instructions << std::endl;
			}
		std::cout << std::endl;
		std::cout << prompt << std::endl;

		do
			{
			std::cout << "File (or '" << kCancelStr << "'): ";
			std::cin  >> *fullName;
			JInputFinished();
			fullName->TrimWhitespace();
			if (*fullName == kCancelStr)
				{
				return kJFalse;
				}
			}
			while (fullName->IsEmpty() || DoSystemCommand(*fullName));

		if (JFileExists(*fullName))
			{
			return kJTrue;
			}
		else if (!(JGetUserNotification())->AskUserYes("That file doesn't exist.  Try again?"))
			{
			return kJFalse;
			}
		}
}

JBoolean
JTextChooseSaveFile::ChooseFile
	(
	const JCharacter*	prompt,
	const JCharacter*	instructions,
	const JCharacter*	origName,
	JString*			fullName
	)
{
	return ChooseFile(prompt, instructions, fullName);
}

/******************************************************************************
 ChooseFiles

	Displays the prompt and asks the user to choose as many files as they want.

	Returns kJFalse if user cancels.

 ******************************************************************************/

JBoolean
JTextChooseSaveFile::ChooseFiles
	(
	const JCharacter*	prompt,
	const JCharacter*	instructions,
	JPtrArray<JString>*	fullNameList
	)
{
	fullNameList->CleanOut();

	if (!JStringEmpty(instructions))
		{
		std::cout << std::endl;
		std::cout << instructions << std::endl;
		}

	JString fullName;
	while (1)
		{
		std::cout << std::endl;
		std::cout << prompt << std::endl;

		do
			{
			std::cout << "File (or '" << kDoneStr << "' or '" << kCancelStr << "'): ";
			std::cin  >> fullName;
			JInputFinished();
			fullName.TrimWhitespace();
			if (fullName == kCancelStr)
				{
				fullNameList->CleanOut();
				return kJFalse;
				}
			else if (fullName == kDoneStr)
				{
				return kJTrue;
				}
			}
			while (fullName.IsEmpty() || DoSystemCommand(fullName));

		if (JFileExists(fullName))
			{
			JString* s = jnew JString(fullName);
			assert( s != NULL );
			fullNameList->Append(s);
			}
		else if (!(JGetUserNotification())->AskUserYes("That file doesn't exist.  Try again?"))
			{
			fullNameList->CleanOut();
			return kJFalse;
			}
		}
}

/******************************************************************************
 ChooseRPath

	Displays the prompt and asks the user to choose a readable path.

	Returns kJFalse if user cancels.

 ******************************************************************************/

JBoolean
JTextChooseSaveFile::ChooseRPath
	(
	const JCharacter*	prompt,
	const JCharacter*	instructions,
	const JCharacter*	origPath,
	JString*			newPath
	)
{
	while (1)
		{
		if (!GetPath(prompt, instructions, newPath))
			{
			return kJFalse;
			}

		if (JDirectoryExists(*newPath))
			{
			return kJTrue;
			}
		else if (!(JGetUserNotification())->AskUserYes("That directory doesn't exist.  Try again?"))
			{
			return kJFalse;
			}
		}
}

/******************************************************************************
 ChooseRWPath

	Displays the prompt and asks the user to choose a writable path.

	Returns kJFalse if user cancels.

 ******************************************************************************/

JBoolean
JTextChooseSaveFile::ChooseRWPath
	(
	const JCharacter*	prompt,
	const JCharacter*	instructions,
	const JCharacter*	origPath,
	JString*			newPath
	)
{
	while (1)
		{
		if (!GetPath(prompt, instructions, newPath))
			{
			return kJFalse;
			}

		if (JDirectoryWritable(*newPath))
			{
			return kJTrue;
			}
		else if (JDirectoryExists(*newPath))
			{
			if (!(JGetUserNotification())->AskUserYes("That directory isn't writable.  Try again?"))
				{
				return kJFalse;
				}
			}
		else	// directory doesn't exist
			{
			if (!(JGetUserNotification())->AskUserYes("That directory doesn't exist.  Try again?"))
				{
				return kJFalse;
				}
			}
		}
}

/******************************************************************************
 GetPath (private)

 ******************************************************************************/

JBoolean
JTextChooseSaveFile::GetPath
	(
	const JCharacter*	prompt,
	const JCharacter*	instructions,
	JString*			newPath
	)
{
	if (!JStringEmpty(instructions))
		{
		std::cout << std::endl;
		std::cout << instructions << std::endl;
		}
	std::cout << std::endl;
	std::cout << prompt << std::endl;

	do
		{
		std::cout << "Path (or '" << kCancelStr << "'): ";
		std::cin  >> *newPath;
		JInputFinished();
		newPath->TrimWhitespace();
		if (*newPath == kCancelStr)
			{
			return kJFalse;
			}
		}
		while (newPath->IsEmpty() || DoSystemCommand(*newPath));

	JAppendDirSeparator(newPath);
	return kJTrue;
}

/******************************************************************************
 SaveFile

	Displays the prompts and asks the user for a file name.

	Returns kJFalse if user cancels.

 ******************************************************************************/

JBoolean
JTextChooseSaveFile::SaveFile
	(
	const JCharacter*	prompt,
	const JCharacter*	instructions,
	const JCharacter*	originalName,
	JString*			newFullName
	)
{
	while (1)
		{
		if (!JStringEmpty(instructions))
			{
			std::cout << std::endl;
			std::cout << instructions << std::endl;
			}
		std::cout << std::endl;
		std::cout << prompt << std::endl;

		do
			{
			std::cout << "File (or '" << kCancelStr << "'): ";
			std::cin  >> *newFullName;
			JInputFinished();
			newFullName->TrimWhitespace();
			if (*newFullName == kCancelStr)
				{
				return kJFalse;
				}
			}
			while (newFullName->IsEmpty() || DoSystemCommand(*newFullName));

		JString path,name;
		JSplitPathAndName(*newFullName, &path, &name);
		const JBoolean fileExists   = JFileExists(*newFullName);
		const JBoolean fileWritable = JFileWritable(*newFullName);
		const JBoolean dirExists    = JDirectoryExists(path);
		const JBoolean dirWritable  = JDirectoryWritable(path);
		if (dirWritable && !fileExists)
			{
			return kJTrue;
			}
		else if (fileWritable)
			{
			if ((JGetUserNotification())->AskUserNo("That file already exists.  Replace it?"))
				{
				return kJTrue;
				}
			}
		else if (!dirExists)
			{
			if (!(JGetUserNotification())->AskUserYes("That directory doesn't exist.  Try again?"))
				{
				return kJFalse;
				}
			}
		else if (!dirWritable && !fileExists)
			{
			if (!(JGetUserNotification())->AskUserYes("That directory isn't writable.  Try again?"))
				{
				return kJFalse;
				}
			}
		else	// file exists and is not writable
			{
			if (!(JGetUserNotification())->AskUserYes("That file isn't writable.  Try again?"))
				{
				return kJFalse;
				}
			}
		}
}

/******************************************************************************
 DoSystemCommand

	Returns kJTrue if the string contains a system command (which it executed).

 ******************************************************************************/

JBoolean
JTextChooseSaveFile::DoSystemCommand
	(
	const JString& str
	)
	const
{
	if (!str.IsEmpty() && str.GetCharacter(1) == '!')
		{
		if (str.GetLength() >= 2)
			{
			JString cmd = str.GetSubstring(2, str.GetLength());
			std::cout << std::endl;
			system(cmd);
			}
		std::cout << std::endl;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
