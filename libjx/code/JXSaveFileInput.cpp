/******************************************************************************
 JXSaveFileInput.cpp

	When this field gets focus, is selects everything up to but not including
	the last period.

	BASE CLASS = JXInputField

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXSaveFileInput.h"
#include <JStringIterator.h>
#include <jFileUtil.h>
#include <jAssert.h>

JBoolean JXSaveFileInput::theAllowSpaceFlag = kJTrue;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSaveFileInput::JXSaveFileInput
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
	GetText()->SetCharacterInWordFunction(IsCharacterInWord);

#ifdef _J_UNIX
	SetMaxLength(255);
#endif
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSaveFileInput::~JXSaveFileInput()
{
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JXSaveFileInput::HandleFocusEvent()
{
	JXInputField::HandleFocusEvent();

	JString root, suffix;
	if (JSplitRootAndSuffix(GetText()->GetText(), &root, &suffix))
		{
		if (root.IsEmpty())
			{
			GoToBeginningOfLine();
			}
		else
			{
			SetSelection(JCharacterRange(1, root.GetCharacterCount()));
			}
		}
}

/******************************************************************************
 IsCharacterInWord (static private)

 ******************************************************************************/

JBoolean
JXSaveFileInput::IsCharacterInWord
	(
	const JUtf8Character& c
	)
{
	return JI2B( c != '.' );
}

/******************************************************************************
 NeedsToFilterText (virtual protected)

	Derived classes should return kJTrue if FilterText() needs to be called.
	This is an optimization, to avoid copying the data if nothing needs to
	be done to it.

 ******************************************************************************/

JBoolean
JXSaveFileInput::StyledText::NeedsToFilterText
	(
	const JString& text
	)
	const
{
	return JI2B(JXInputField::StyledText::NeedsToFilterText(text) ||
				text.Contains(ACE_DIRECTORY_SEPARATOR_STR) ||
				(!theAllowSpaceFlag && text.Contains(" ")));
}

/******************************************************************************
 FilterText (virtual protected)

	Derived classes can override this to enforce restrictions on the text.
	Return kJFalse if the text cannot be used at all.

	*** Note that style may be nullptr or empty if the data was plain text.

 ******************************************************************************/

JBoolean
JXSaveFileInput::StyledText::FilterText
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	if (!JXInputField::StyledText::FilterText(text, style))
		{
		return kJFalse;
		}

	// convert slash to dash, and possibly space to underscore

	JStringIterator iter(text);
	JUtf8Character c;
	while (iter.Next(&c))
		{
		if (c == ACE_DIRECTORY_SEPARATOR_CHAR)
			{
			iter.SetPrev('-', kJFalse);
			}
		else if (!theAllowSpaceFlag && c.IsSpace())
			{
			iter.SetPrev('_', kJFalse);
			}
		}

	return kJTrue;
}
