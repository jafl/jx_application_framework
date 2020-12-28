/******************************************************************************
 JXPathInput.cpp

	Input field for entering a file path.

	BASE CLASS = JXFSInputBase

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1998-2018 by John Lindal.

 ******************************************************************************/

#include "JXPathInput.h"
#include <JColorManager.h>
#include <jDirUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXPathInput::JXPathInput
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
				  kJFalse, "Hint::JXPathInput",
				  enclosure, hSizing, vSizing, x,y, w,h)
{
	JXPathInputX();
}

// protected

JXPathInput::JXPathInput
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
	JXFSInputBase(text, kJFalse, "Hint::JXPathInput",
				  enclosure, hSizing, vSizing, x,y, w,h)
{
	JXPathInputX();
}

// private

void
JXPathInput::JXPathInputX()
{
	itsAllowInvalidPathFlag = kJFalse;
	itsRequireWriteFlag     = kJFalse;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPathInput::~JXPathInput()
{
}

/******************************************************************************
 GetPath (virtual)

	Returns kJTrue if the current path is valid.  In this case, *path is
	set to the full path, relative to the root directory.

	Use this instead of GetText(), because that may return a relative path.

 ******************************************************************************/

JBoolean
JXPathInput::GetPath
	(
	JString* path
	)
	const
{
	const JString& text = GetText().GetText();

	JString basePath;
	const JBoolean hasBasePath = GetBasePath(&basePath);

	return JI2B(!text.IsEmpty() &&
				(JIsAbsolutePath(text) || hasBasePath) &&
				JConvertToAbsolutePath(text, basePath, path) &&
				JDirectoryExists(*path) &&
				JDirectoryReadable(*path) &&
				JCanEnterDirectory(*path) &&
				(!itsRequireWriteFlag || JDirectoryWritable(*path)));
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

JBoolean
JXPathInput::InputValid()
{
	if (itsAllowInvalidPathFlag)
		{
		return kJTrue;
		}
	else if (!JXFSInputBase::InputValid())
		{
		return kJFalse;
		}

	const JString& text = GetText()->GetText();
	if (text.IsEmpty())		// paranoia -- JXFSInputBase should have reported
		{
		return !IsRequired();
		}

	JString basePath;
	const JBoolean hasBasePath = GetBasePath(&basePath);

	JString path;
	if (JIsRelativePath(text) && !hasBasePath)
		{
		JGetUserNotification()->ReportError(JGetString("NoRelPath::JXPathInput"));
		GetText()->RestyleAll();
		return kJFalse;
		}
	if (!JConvertToAbsolutePath(text, basePath, &path))
		{
		JGetUserNotification()->ReportError(JGetString("InvalidDir::JXPathInput"));
		GetText()->RestyleAll();
		return kJFalse;
		}

	const JString currDir = JGetCurrentDirectory();
	const JError err      = JChangeDirectory(path);
	JChangeDirectory(currDir);

	if (err.OK())
		{
		if (!JDirectoryReadable(path))
			{
			JGetUserNotification()->ReportError(JGetString("Unreadable::JXPathInput"));
			GetText()->RestyleAll();
			return kJFalse;
			}
		else if (itsRequireWriteFlag && !JDirectoryWritable(path))
			{
			JGetUserNotification()->ReportError(JGetString("DirNotWritable::JXGlobal"));
			GetText()->RestyleAll();
			return kJFalse;
			}
		else
			{
			return kJTrue;
			}
		}

	const JUtf8Byte* errID;
	if (err == kJAccessDenied)
		{
		errID = "AccessDenied::JXPathInput";
		}
	else if (err == kJBadPath)
		{
		errID = "DirectoryDoesNotExist::JXGlobal";
		}
	else if (err == kJComponentNotDirectory)
		{
		errID = "CompNotDir::JXPathInput";
		}
	else
		{
		errID = "InvalidDir::JXPathInput";
		}

	JGetUserNotification()->ReportError(JGetString(errID));
	GetText()->RestyleAll();
	return kJFalse;
}

/******************************************************************************
 GetTextColor (static)

	Draw the entire text red if the path is invalid.  This is provided
	so tables can draw the text the same way as the input field.

	base can be nullptr.  If you use JXPathInput for relative paths, base
	should be the path passed to SetBasePath().

 ******************************************************************************/

JColorID
JXPathInput::GetTextColor
	(
	const JString&	path,
	const JString&	base,
	const JBoolean	requireWrite
	)
{
	if (path.IsEmpty())
		{
		return JColorManager::GetBlackColor();
		}

	JString fullPath;
	if ((JIsAbsolutePath(path) || !base.IsEmpty()) &&
		JConvertToAbsolutePath(path, base, &fullPath) &&
		JDirectoryReadable(fullPath) &&
		JCanEnterDirectory(fullPath) &&
		(!requireWrite || JDirectoryWritable(fullPath)))
		{
		return JColorManager::GetBlackColor();
		}
	else
		{
		return JColorManager::GetRedColor();
		}
}

/******************************************************************************
 GetTextForChoosePath (virtual)

	Returns a string that can safely be passed to JChooseSaveFile::ChooseFile().

 ******************************************************************************/

JString
JXPathInput::GetTextForChoosePath()
	const
{
	JString text = GetText().GetText();

	JString basePath;
	const JBoolean hasBasePath = GetBasePath(&basePath);

	if (text.IsEmpty() && hasBasePath)
		{
		text = basePath;
		}
	else if (!text.IsEmpty() && JIsRelativePath(text) && hasBasePath)
		{
		text = JCombinePathAndName(basePath, text);
		}
	return text;
}

/******************************************************************************
 ChoosePath

	Calls either ChooseRPath() or ChooseRWPath(), as appropriate based
	on itsRequireWriteFlag.

	instr can be nullptr, just like in JChooseSaveFile::Choose*Path().

 ******************************************************************************/

JBoolean
JXPathInput::ChoosePath
	(
	const JString& prompt,
	const JString& instr
	)
{
	JString origPath = GetTextForChoosePath();
	JString newPath;
	if ((itsRequireWriteFlag &&
		 JGetChooseSaveFile()->ChooseRWPath(prompt, instr, origPath, &newPath)) ||
		(!itsRequireWriteFlag &&
		 JGetChooseSaveFile()->ChooseRPath(prompt, instr, origPath, &newPath)))
		{
		GetText()->SetText(newPath);
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
JXPathInput::StyledText::ComputeErrorLength
	(
	JXFSInputBase*	field,
	const JSize		totalLength,	// original character count
	const JString&	fullPath		// modified path
	)
	const
{
	if (fullPath.IsEmpty())
		{
		return totalLength;
		}

	JXPathInput* f = dynamic_cast<JXPathInput*>(field);

	const JString closestDir = JGetClosestDirectory(fullPath, f->itsRequireWriteFlag);
	if (fullPath.BeginsWith(closestDir))
		{
		return fullPath.GetCharacterCount() - closestDir.GetCharacterCount();
		}
	else
		{
		return totalLength;
		}
}
