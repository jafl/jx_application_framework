/******************************************************************************
 CBKeyScriptInput.cpp

	Input field for entering a keystroke script.

	BASE CLASS = JXInputField

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBKeyScriptInput.h"
#include "CBMacroManager.h"
#include <JXColormap.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBKeyScriptInput::CBKeyScriptInput
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
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBKeyScriptInput::~CBKeyScriptInput()
{
}

/******************************************************************************
 AdjustStylesBeforeRecalc (virtual protected)

	Draw the illegal backslash characters in red.

 ******************************************************************************/

void
CBKeyScriptInput::AdjustStylesBeforeRecalc
	(
	const JString&		buffer,
	JRunArray<Font>*	styles,
	JIndexRange*		recalcRange,
	JIndexRange*		redrawRange,
	const JBoolean		deletion
	)
{
	CBMacroManager::HighlightErrors(buffer, GetColormap(), styles);
	*redrawRange += JIndexRange(1, buffer.GetLength());
}
