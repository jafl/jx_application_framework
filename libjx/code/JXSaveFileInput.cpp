/******************************************************************************
 JXSaveFileInput.cpp

	When this field gets focus, is selects everything up to but not including
	the last period.

	BASE CLASS = JXInputField

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXSaveFileInput.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

bool JXSaveFileInput::theAllowSpaceFlag = true;

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

bool
JXSaveFileInput::IsCharacterInWord
	(
	const JUtf8Character& c
	)
{
	return c != '.';
}

/******************************************************************************
 NeedsToFilterText (virtual protected)

	Derived classes should return true if FilterText() needs to be called.
	This is an optimization, to avoid copying the data if nothing needs to
	be done to it.

 ******************************************************************************/

bool
JXSaveFileInput::StyledText::NeedsToFilterText
	(
	const JString& text
	)
	const
{
	return JXInputField::StyledText::NeedsToFilterText(text) ||
				text.Contains(ACE_DIRECTORY_SEPARATOR_STR) ||
				(!theAllowSpaceFlag && text.Contains(" "));
}

/******************************************************************************
 FilterText (virtual protected)

	Derived classes can override this to enforce restrictions on the text.
	Return false if the text cannot be used at all.

	*** Note that style may be nullptr or empty if the data was plain text.

 ******************************************************************************/

bool
JXSaveFileInput::StyledText::FilterText
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	if (!JXInputField::StyledText::FilterText(text, style))
	{
		return false;
	}

	// convert slash to dash, and possibly space to underscore

	JStringIterator iter(text);
	JUtf8Character c;
	while (iter.Next(&c))
	{
		if (c == ACE_DIRECTORY_SEPARATOR_CHAR)
		{
			iter.SetPrev(JUtf8Character('-'), kJIteratorStay);
		}
		else if (!theAllowSpaceFlag && c.IsSpace())
		{
			iter.SetPrev(JUtf8Character('_'), kJIteratorStay);
		}
	}

	return true;
}
