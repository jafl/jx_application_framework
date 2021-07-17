/******************************************************************************
 CBKeyScriptInput.cpp

	Input field for entering a keystroke script.

	BASE CLASS = JXInputField

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBKeyScriptInput.h"
#include "CBMacroManager.h"
#include <JXColorManager.h>
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
	JXInputField(jnew StyledText(enclosure->GetFontManager()),
				 enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBKeyScriptInput::~CBKeyScriptInput()
{
}

/******************************************************************************
 AdjustStylesBeforeBroadcast (virtual protected)

	Draw the illegal backslash characters in red.

 ******************************************************************************/

void
CBKeyScriptInput::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&			text,
	JRunArray<JFont>*		styles,
	JStyledText::TextRange*	recalcRange,
	JStyledText::TextRange*	redrawRange,
	const bool			deletion
	)
{
	CBMacroManager::HighlightErrors(text, styles);
	*redrawRange = JStyledText::TextRange(
		JCharacterRange(1, text.GetCharacterCount()),
		JUtf8ByteRange(1, text.GetByteCount()));
}
