/******************************************************************************
 JXRegexReplaceInput.cpp

	Maintains a line of editable text for entering a regular expression
	replace pattern.

	BASE CLASS = JXInputField

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include <JXRegexReplaceInput.h>
#include <JInterpolate.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXRegexReplaceInput::JXRegexReplaceInput
	(
	JInterpolate*		testInterpolator,
	const JBoolean		widgetOwnsInterpolator,
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

JBoolean
JXRegexReplaceInput::InputValid()
{
	if (!JXInputField::InputValid())
		{
		return kJFalse;
		}
	else
		{
		const JString& text = GetText()->GetText();

		if (!IsRequired() && text.IsEmpty())
			{
			return kJTrue;
			}

		JCharacterRange errRange;
		const JError err = itsTestInterpolator->ContainsError(text, &errRange);
		if (err.OK())
			{
			return kJTrue;
			}
		else
			{
			SetSelection(errRange);
			err.ReportIfError();
			return kJFalse;
			}
		}
}
