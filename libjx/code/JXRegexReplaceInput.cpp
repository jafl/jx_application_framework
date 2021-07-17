/******************************************************************************
 JXRegexReplaceInput.cpp

	Maintains a line of editable text for entering a regular expression
	replace pattern.

	BASE CLASS = JXInputField

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JXRegexReplaceInput.h"
#include <JInterpolate.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXRegexReplaceInput::JXRegexReplaceInput
	(
	JInterpolate*		testInterpolator,
	const bool		widgetOwnsInterpolator,
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
	itsTestInterpolator     = testInterpolator;
	itsOwnsInterpolatorFlag = widgetOwnsInterpolator;

	SetIsRequired();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXRegexReplaceInput::~JXRegexReplaceInput()
{
	if (itsOwnsInterpolatorFlag)
		{
		jdelete itsTestInterpolator;
		}
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

bool
JXRegexReplaceInput::InputValid()
{
	if (!JXInputField::InputValid())
		{
		return false;
		}
	else
		{
		const JString& text = GetText()->GetText();

		if (!IsRequired() && text.IsEmpty())
			{
			return true;
			}

		JCharacterRange errRange;
		const JError err = itsTestInterpolator->ContainsError(text, &errRange);
		if (err.OK())
			{
			return true;
			}
		else
			{
			SetSelection(errRange);
			err.ReportIfError();
			return false;
			}
		}
}
