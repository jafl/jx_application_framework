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
	JXFileInput(enclosure, hSizing, vSizing, x,y, w,h)
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

	Returns kJTrue if the current file name is valid.  In this case, *fullName
	is set to the full path + name, relative to the root directory.

	Use this instead of GetText(), because that may return a relative path.

 ******************************************************************************/

JBoolean
CMDebuggerProgramInput::GetFile
	(
	JString* fullName
	)
	const
{
	const JString& text = GetText();
	if (JProgramAvailable(text))
		{
		*fullName = text;
		return kJTrue;
		}
	else
		{
		return JXFileInput::GetFile(fullName);
		}
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

JBoolean
CMDebuggerProgramInput::InputValid()
{
	const JString& text = GetText();
	if (JProgramAvailable(text))
		{
		return kJTrue;
		}
	else
		{
		return JXFileInput::InputValid();
		}
}

/******************************************************************************
 AdjustStylesBeforeRecalc (virtual protected)

 ******************************************************************************/

void
CMDebuggerProgramInput::xAdjustStylesBeforeRecalc
	(
	const JString&		buffer,
	JRunArray<JFont>*	styles,
	JIndexRange*		recalcRange,
	JIndexRange*		redrawRange,
	const JBoolean		deletion
	)
{
	const JString& text = GetText();
	if (JProgramAvailable(text))
		{
		const JSize totalLength = buffer.GetLength();

		JFont f = styles->GetFirstElement();
		styles->RemoveAll();

		f.SetColor(GetColormap()->GetBlackColor());
		styles->AppendElements(f, totalLength);

		*redrawRange += JIndexRange(1, totalLength);
		}
	else
		{
		JXFileInput::AdjustStylesBeforeRecalc(buffer, styles, recalcRange, redrawRange, deletion);
		}
}
