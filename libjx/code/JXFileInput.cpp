/******************************************************************************
 JXFileInput.cpp

	Input field for entering a path + file.

	BASE CLASS = JXFSInputBase

	Copyright (C) 1999-2018 by John Lindal.

 ******************************************************************************/

#include "JXFileInput.h"
#include "JXChooseFileDialog.h"
#include "JXSaveFileDialog.h"
#include "JXWindow.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

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
				  true, "Hint::JXFileInput",
				  enclosure, hSizing, vSizing, x,y, w,h)
{
	JXFileInputX();
}

// protected

JXFileInput::JXFileInput
	(
	StyledText*			text,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXFSInputBase(text, true, "Hint::JXFileInput",
				  enclosure, hSizing, vSizing, x,y, w,h)
{
	JXFileInputX();
}

// private

void
JXFileInput::JXFileInputX()
{
	itsAllowInvalidFileFlag = false;
	itsRequireReadFlag      = true;
	itsRequireWriteFlag     = true;
	itsRequireExecFlag      = false;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFileInput::~JXFileInput()
{
}

/******************************************************************************
 GetFile (virtual)

	Returns true if the current file name is valid.  In this case, *fullName
	is set to the full path + name, relative to the root directory.

	Use this instead of GetText(), because that may return a relative path.

 ******************************************************************************/

bool
JXFileInput::GetFile
	(
	JString* fullName
	)
	const
{
	const JString& text = GetText().GetText();

	JString basePath;
	const bool hasBasePath = GetBasePath(&basePath);

	return !text.IsEmpty() &&
				(JIsAbsolutePath(text) || hasBasePath) &&
				JConvertToAbsolutePath(text, basePath, fullName) &&
				JFileExists(*fullName) &&
				(!itsRequireReadFlag  || JFileReadable(*fullName)) &&
				(!itsRequireWriteFlag || JFileWritable(*fullName)) &&
				(!itsRequireExecFlag  || JFileExecutable(*fullName));
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

bool
JXFileInput::InputValid()
{
	if (itsAllowInvalidFileFlag)
	{
		return true;
	}
	else if (!JXFSInputBase::InputValid())
	{
		return false;
	}

	const JString& text = GetText()->GetText();
	if (text.IsEmpty())
	{
		return !IsRequired();
	}

	JString basePath;
	const bool hasBasePath = GetBasePath(&basePath);

	JString fullName;
	const JUtf8Byte* errID = nullptr;
	if (JIsRelativePath(text) && !hasBasePath)
	{
		errID = "NoRelPath::JXFileInput";
		GetText()->RestyleAll();
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
		return true;
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString(errID));
		return false;
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
	const bool	requireRead,
	const bool	requireWrite,
	const bool	requireExec
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

	Returns a string that can safely be passed to JXChooseFileDialog.

 ******************************************************************************/

JString
JXFileInput::GetTextForChooseFile()
	const
{
	JString text = GetText().GetText();

	JString basePath;
	const bool hasBasePath = GetBasePath(&basePath);

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

 ******************************************************************************/

bool
JXFileInput::ChooseFile
	(
	const JString& instr
	)
{
	auto* dlog =
		JXChooseFileDialog::Create(JXChooseFileDialog::kSelectSingleFile,
								   GetTextForChooseFile(), JString::empty, instr);

	if (dlog->DoDialog())
	{
		GetText()->SetText(dlog->GetFullName());
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 SaveFile

 ******************************************************************************/

bool
JXFileInput::SaveFile
	(
	const JString& prompt,
	const JString& instr
	)
{
	auto* dlog = JXSaveFileDialog::Create(prompt, GetTextForChooseFile(), JString::empty, instr);
	if (dlog->DoDialog())
	{
		GetText()->SetText(dlog->GetFullName());
		return true;
	}
	else
	{
		return false;
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

	auto* f = dynamic_cast<JXFileInput*>(field);

	if (!JFileExists(fullName) ||
		(f->itsRequireReadFlag  && !JFileReadable(fullName)) ||
		(f->itsRequireWriteFlag && !JFileWritable(fullName)) ||
		(f->itsRequireExecFlag  && !JFileExecutable(fullName)))
	{
		const JString closestDir = JGetClosestDirectory(fullName, false);
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
