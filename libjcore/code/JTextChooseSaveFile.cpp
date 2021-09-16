/******************************************************************************
 JTextChooseSaveFile.cpp

	Class to let user select and save files using the console.

	*** Security note:
		The user is allowed to execute *any* command via system() by starting
		the line with an exclamation point.  Never call JTextChooseSaveFile
		objects while in suid root mode.

	Copyright (C) 1994-98 by John Lindal.

 ******************************************************************************/

#include "JTextChooseSaveFile.h"
#include "jCommandLine.h"

#include "JString.h"
#include "jFileUtil.h"
#include "jDirUtil.h"
#include <stdlib.h>
#include "jGlobals.h"
#include "jAssert.h"

const JUtf8Byte* kDoneStr   = "done";
const JUtf8Byte* kCancelStr = "cancel";

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

	Returns false if user cancels.

 ******************************************************************************/

bool
JTextChooseSaveFile::ChooseFile
	(
	const JString&	prompt,
	const JString&	instructions,
	JString*		fullName
	)
{
	while (true)
		{
		if (!instructions.IsEmpty())
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
				return false;
				}
			}
			while (fullName->IsEmpty() || DoSystemCommand(*fullName));

		if (JFileExists(*fullName))
			{
			return true;
			}
		else if (!JGetUserNotification()->AskUserYes(JGetString("FileDoesNotExist::JTextChooseSaveFile")))
			{
			return false;
			}
		}
}

bool
JTextChooseSaveFile::ChooseFile
	(
	const JString&	prompt,
	const JString&	instructions,
	const JString&	origName,
	JString*		fullName
	)
{
	return ChooseFile(prompt, instructions, fullName);
}

/******************************************************************************
 ChooseFiles

	Displays the prompt and asks the user to choose as many files as they want.

	Returns false if user cancels.

 ******************************************************************************/

bool
JTextChooseSaveFile::ChooseFiles
	(
	const JString&		prompt,
	const JString&		instructions,
	JPtrArray<JString>*	fullNameList
	)
{
	fullNameList->CleanOut();

	if (!instructions.IsEmpty())
		{
		std::cout << std::endl;
		std::cout << instructions << std::endl;
		}

	JString fullName;
	while (true)
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
				return false;
				}
			else if (fullName == kDoneStr)
				{
				return true;
				}
			}
			while (fullName.IsEmpty() || DoSystemCommand(fullName));

		if (JFileExists(fullName))
			{
			auto* s = jnew JString(fullName);
			assert( s != nullptr );
			fullNameList->Append(s);
			}
		else if (!JGetUserNotification()->AskUserYes(JGetString("FileDoesNotExist::JTextChooseSaveFile")))
			{
			fullNameList->CleanOut();
			return false;
			}
		}
}

/******************************************************************************
 ChooseRPath

	Displays the prompt and asks the user to choose a readable path.

	Returns false if user cancels.

 ******************************************************************************/

bool
JTextChooseSaveFile::ChooseRPath
	(
	const JString&	prompt,
	const JString&	instructions,
	const JString&	origPath,
	JString*		newPath
	)
{
	while (true)
		{
		if (!GetPath(prompt, instructions, newPath))
			{
			return false;
			}

		if (JDirectoryExists(*newPath))
			{
			return true;
			}
		else if (!JGetUserNotification()->AskUserYes(JGetString("DirectoryDoesNotExist::JTextChooseSaveFile")))
			{
			return false;
			}
		}
}

/******************************************************************************
 ChooseRWPath

	Displays the prompt and asks the user to choose a writable path.

	Returns false if user cancels.

 ******************************************************************************/

bool
JTextChooseSaveFile::ChooseRWPath
	(
	const JString&	prompt,
	const JString&	instructions,
	const JString&	origPath,
	JString*		newPath
	)
{
	while (true)
		{
		if (!GetPath(prompt, instructions, newPath))
			{
			return false;
			}

		if (JDirectoryWritable(*newPath))
			{
			return true;
			}
		else if (JDirectoryExists(*newPath))
			{
			if (!JGetUserNotification()->AskUserYes(JGetString("DirectoryNotWritable::JTextChooseSaveFile")))
				{
				return false;
				}
			}
		else	// directory doesn't exist
			{
			if (!JGetUserNotification()->AskUserYes(JGetString("DirectoryDoesNotExist::JTextChooseSaveFile")))
				{
				return false;
				}
			}
		}
}

/******************************************************************************
 GetPath (private)

 ******************************************************************************/

bool
JTextChooseSaveFile::GetPath
	(
	const JString&	prompt,
	const JString&	instructions,
	JString*		newPath
	)
{
	if (!instructions.IsEmpty())
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
			return false;
			}
		}
		while (newPath->IsEmpty() || DoSystemCommand(*newPath));

	JAppendDirSeparator(newPath);
	return true;
}

/******************************************************************************
 SaveFile

	Displays the prompts and asks the user for a file name.

	Returns false if user cancels.

 ******************************************************************************/

bool
JTextChooseSaveFile::SaveFile
	(
	const JString&	prompt,
	const JString&	instructions,
	const JString&	originalName,
	JString*		newFullName
	)
{
	while (true)
		{
		if (!instructions.IsEmpty())
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
				return false;
				}
			}
			while (newFullName->IsEmpty() || DoSystemCommand(*newFullName));

		JString path,name;
		JSplitPathAndName(*newFullName, &path, &name);
		const bool fileExists   = JFileExists(*newFullName);
		const bool fileWritable = JFileWritable(*newFullName);
		const bool dirExists    = JDirectoryExists(path);
		const bool dirWritable  = JDirectoryWritable(path);
		if (dirWritable && !fileExists)
			{
			return true;
			}
		else if (fileWritable)
			{
			if (JGetUserNotification()->AskUserNo(JGetString("FileExists::JTextChooseSaveFile")))
				{
				return true;
				}
			}
		else if (!dirExists)
			{
			if (!JGetUserNotification()->AskUserYes(JGetString("DirectoryDoesNotExist::JTextChooseSaveFile")))
				{
				return false;
				}
			}
		else if (!dirWritable && !fileExists)
			{
			if (!JGetUserNotification()->AskUserYes(JGetString("DirectoryNotWritable::JTextChooseSaveFile")))
				{
				return false;
				}
			}
		else	// file exists and is not writable
			{
			if (!JGetUserNotification()->AskUserYes(JGetString("FileNotWritable::JTextChooseSaveFile")))
				{
				return false;
				}
			}
		}
}

/******************************************************************************
 DoSystemCommand

	Returns true if the string contains a system command (which it executed).

 ******************************************************************************/

bool
JTextChooseSaveFile::DoSystemCommand
	(
	const JString& str
	)
	const
{
	if (!str.IsEmpty() && str.GetFirstCharacter() == '!')
		{
		if (str.GetCharacterCount() >= 2)
			{
			std::cout << std::endl;
			if (system(str.GetBytes() + 1) == -1)
				{
				std::cout << "failed";
				}
			}
		std::cout << std::endl;
		return true;
		}
	else
		{
		return false;
		}
}
