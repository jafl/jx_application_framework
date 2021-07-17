/******************************************************************************
 CMDebuggerProgramInput.cpp

	Input field for entering the debugger program to use.  Any executable
	on PATH is allowed, in addition to a full path.

	BASE CLASS = JXFileInput

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "CMDebuggerProgramInput.h"
#include <JXColorManager.h>
#include <jProcessUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMDebuggerProgramInput::CMDebuggerProgramInput
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
	JXFileInput(jnew StyledText(this, enclosure->GetFontManager()),
				enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMDebuggerProgramInput::~CMDebuggerProgramInput()
{
}

/******************************************************************************
 GetFile (virtual)

	Returns true if the current file name is valid.  In this case, *fullName
	is set to the full path + name, relative to the root directory.

	Use this instead of GetText(), because that may return a relative path.

 ******************************************************************************/

bool
CMDebuggerProgramInput::GetFile
	(
	JString* fullName
	)
	const
{
	const JString& text = GetText().GetText();
	if (JProgramAvailable(text))
		{
		*fullName = text;
		return true;
		}
	else
		{
		fullName->Clear();
		return false;
		}
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

bool
CMDebuggerProgramInput::InputValid()
{
	const JString& text = GetText()->GetText();
	return JProgramAvailable(text);
}

/******************************************************************************
 ComputeErrorLength (virtual protected)

	Return the number of characters that should be marked as invalid.

 ******************************************************************************/

JSize
CMDebuggerProgramInput::StyledText::ComputeErrorLength
	(
	JXFSInputBase*	field,
	const JSize		totalLength,	// original character count
	const JString&	fullName		// modified path
	)
	const
{
	const JString& text = field->GetText()->GetText();
	return JProgramAvailable(text) ? 0 : totalLength;
}
