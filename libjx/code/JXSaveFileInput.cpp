/******************************************************************************
 JXSaveFileInput.cpp

	When this field gets focus, is selects everything up to but not including
	the last period.

	BASE CLASS = JXInputField

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXSaveFileInput.h>
#include <jFileUtil.h>
#include <ace/OS.h>
#include <jAssert.h>

JBoolean JXSaveFileInput::itsAllowSpaceFlag = kJTrue;

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
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	SetCharacterInWordFunction(IsCharacterInWord);

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
	if (JSplitRootAndSuffix(GetText(), &root, &suffix))
		{
		if (root.IsEmpty())
			{
			SetCaretLocation(1);
			}
		else
			{
			SetSelection(1, root.GetLength());
			}
		}
}

/******************************************************************************
 NeedsToFilterText (virtual protected)

	Derived classes should return kJTrue if FilterText() needs to be called.
	This is an optimization, to avoid copying the data if nothing needs to
	be done to it.

 ******************************************************************************/

JBoolean
JXSaveFileInput::NeedsToFilterText
	(
	const JCharacter* text
	)
	const
{
	return JI2B(JXInputField::NeedsToFilterText(text) ||
				strchr(text, ACE_DIRECTORY_SEPARATOR_CHAR) != NULL ||
				(!itsAllowSpaceFlag && strchr(text, ' ') != NULL));
}

/******************************************************************************
 FilterText (virtual protected)

	Derived classes can override this to enforce restrictions on the text.
	Return kJFalse if the text cannot be used at all.

	*** Note that style may be NULL or empty if the data was plain text.

 ******************************************************************************/

JBoolean
JXSaveFileInput::FilterText
	(
	JString*			text,
	JRunArray<Font>*	style
	)
{
	if (!JXInputField::FilterText(text, style))
		{
		return kJFalse;
		}

	// convert slash to dash, and possibly space to underscore

	const JSize length = text->GetLength();
	for (JIndex i=1; i<=length; i++)
		{
		if (text->GetCharacter(i) == ACE_DIRECTORY_SEPARATOR_CHAR)
			{
			text->SetCharacter(i, '-');
			}
		else if (!itsAllowSpaceFlag && text->GetCharacter(i) == ' ')
			{
			text->SetCharacter(i, '_');
			}
		}

	return kJTrue;
}

/******************************************************************************
 IsCharacterInWord (static private)

 ******************************************************************************/

JBoolean
JXSaveFileInput::IsCharacterInWord
	(
	const JString&	text,
	const JIndex	charIndex
	)
{
	return JI2B( text.GetCharacter(charIndex) != '.' );
}
