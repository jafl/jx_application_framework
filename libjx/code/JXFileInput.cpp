/******************************************************************************
 JXFileInput.cpp

	Input field for entering a path + file.

	BASE CLASS = JXFSInputBase

	Copyright (C) 1999-2018 by John Lindal.

 ******************************************************************************/

#include "JXFileInput.h"
#include <JColorManager.h>
#include <jDirUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFileInput::JXFileInput
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXFSInputBase(jnew StyledText(this, enclosure->GetFontManager()),
				  kJTrue, "Hint::JXFileInput",
				  enclosure, hSizing, vSizing, x,y, w,h)
{
	itsAllowInvalidFileFlag = kJFalse;
	itsRequireReadFlag      = kJTrue;
	itsRequireWriteFlag     = kJTrue;
	itsRequireExecFlag      = kJFalse;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFileInput::~JXFileInput()
{
}

/******************************************************************************
 GetFile (virtual)

	Returns kJTrue if the current file name is valid.  In this case, *fullName
	is set to the full path + name, relative to the root directory.

	Use this instead of GetText(), because that may return a relative path.

 ******************************************************************************/

JBoolean
JXFileInput::GetFile
	(
	JString* fullName
	)
	const
{
	const JString& text = GetText().GetText();

	JString basePath;
	const JBoolean hasBasePath = GetBasePath(&basePath);

	return JI2B(!text.IsEmpty() &&
				(JIsAbsolutePath(text) || hasBasePath) &&
				JConvertToAbsolutePath(text, basePath, fullName) &&
				JFileExists(*fullName) &&
				(!itsRequireReadFlag  || JFileReadable(*fullName)) &&
				(!itsRequireWriteFlag || JFileWritable(*fullName)) &&
				(!itsRequireExecFlag  || JFileExecutable(*fullName)));
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

JBoolean
JXFileInput::InputValid()
{
	if (itsAllowInvalidFileFlag)
		{
		return kJTrue;
		}
	else if (!JXFSInputBase::InputValid())
		{
		return kJFalse;
		}

	const JString& text = GetText()->GetText();
	if (text.IsEmpty())
		{
		return !IsRequired();
		}

	JString basePath;
	const JBoolean hasBasePath = GetBasePath(&basePath);

	JString fullName;
	const JUtf8Byte* errID = nullptr;
	if (JIsRelativePath(text) && !hasBasePath)
		{
		errID = "NoRelPath::JXFileInput";
		RecalcAll();
		}
	else if (!JConvertToAbsolutePath(text, basePath, &fullName) ||
			 !JFileExists(fullName))
		{
		errID = "DoesNotExist::JXFileInput";
		}
	else if (itsRequireReadFlag && !JFileReadable(fullName))
		{
		errID = "Unreadable::JXFileInput";
		}
	else if (itsRequireWriteFlag && !JFileWritable(fullName))
		{
		errID = "Unwritable::JXFileInput";
		}
	else if (itsRequireExecFlag && !JFileExecutable(fullName))
		{
		errID = "CannotExec::JXFileInput";
		}

	if (JString::IsEmpty(errID))
		{
		return kJTrue;
		}
	else
		{
		(JGetUserNotification())->ReportError(JGetString(errID));
		return kJFalse;
		}
}

/******************************************************************************
 GetTextColor (static)

	Draw the entire text red if the file is invalid.  This is provided
	so tables can draw the text the same way as the input field.

	base can be nullptr.  If you use JXFileInput for relative paths, basePath
	should be the path passed to SetBasePath().

 ******************************************************************************/

JColorID
JXFileInput::GetTextColor
	(
	const JString&	fileName,
	const JString&	basePath,
	const JBoolean	requireRead,
	const JBoolean	requireWrite,
	const JBoolean	requireExec
	)
{
	if (fileName.IsEmpty())
		{
		return JColorManager::GetBlackColor();
		}

	JString fullName;
	if ((JIsAbsolutePath(fileName) || !basePath.IsEmpty()) &&
		JConvertToAbsolutePath(fileName, basePath, &fullName) &&
		(!requireRead  || JFileReadable(fullName)) &&
		(!requireWrite || JFileWritable(fullName)) &&
		(!requireExec  || JFileExecutable(fullName)))
		{
		return JColorManager::GetBlackColor();
		}
	else
		{
		return JColorManager::GetRedColor();
		}
}

/******************************************************************************
 GetTextForChooseFile (virtual)

	Returns a string that can safely be passed to JChooseSaveFile::ChooseFile().

 ******************************************************************************/

JString
JXFileInput::GetTextForChooseFile()
	const
{
	JString text = GetText().GetText();

	JString basePath;
	const JBoolean hasBasePath = GetBasePath(&basePath);

	if (text.IsEmpty() && hasBasePath)
		{
		text = basePath;
		JAppendDirSeparator(&text);
		}
	if (text.EndsWith(ACE_DIRECTORY_SEPARATOR_STR))
		{
		text.Append("*");
		}
	if (!text.IsEmpty() && JIsRelativePath(text) && hasBasePath)
		{
		text = JCombinePathAndName(basePath, text);
		}
	return text;
}

/******************************************************************************
 ChooseFile

	instr can be nullptr, just like in JChooseSaveFile::ChooseFile().

 ******************************************************************************/

JBoolean
JXFileInput::ChooseFile
	(
	const JString& prompt,
	const JString& instr
	)
{
	const JString origName = GetTextForChooseFile();
	JString newName;
	if ((JGetChooseSaveFile())->ChooseFile(prompt, instr, origName, &newName))
		{
		GetText()->SetText(newName);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SaveFile

	instr can be nullptr, just like in JChooseSaveFile::SaveFile().

 ******************************************************************************/

JBoolean
JXFileInput::SaveFile
	(
	const JString& prompt,
	const JString& instr
	)
{
	const JString origName = GetTextForChooseFile();
	JString newName;
	if ((JGetChooseSaveFile())->SaveFile(prompt, instr, origName, &newName))
		{
		GetText()->SetText(newName);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ComputeErrorLength (virtual protected)

	Return the number of characters that should be marked as invalid.

 ******************************************************************************/

JSize
JXFileInput::StyledText::ComputeErrorLength
	(
	JXFSInputBase*	field,
	const JSize		totalLength,	// original character count
	const JString&	fullName		// modified path
	)
	const
{
	if (fullName.IsEmpty())
		{
		return totalLength;
		}

	JXFileInput* f = dynamic_cast<JXFileInput*>(field);

	if (!JFileExists(fullName) ||
		(f->itsRequireReadFlag  && !JFileReadable(fullName)) ||
		(f->itsRequireWriteFlag && !JFileWritable(fullName)) ||
		(f->itsRequireExecFlag  && !JFileExecutable(fullName)))
		{
		const JString closestDir = JGetClosestDirectory(fullName, kJFalse);
		if (fullName.BeginsWith(closestDir))
			{
			return fullName.GetCharacterCount() - closestDir.GetCharacterCount();
			}
		else
			{
			return totalLength;
			}
		}
	else
		{
		return 0;
		}
}
