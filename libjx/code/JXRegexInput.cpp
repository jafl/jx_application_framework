/******************************************************************************
 JXRegexInput.cpp

	Maintains a line of editable text for entering a regular expression.

	BASE CLASS = JXInputField

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include <JXRegexInput.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXRegexInput::JXRegexInput
	(
	JRegex*				testRegex,
	const JBoolean		widgetOwnsRegex,
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
	itsTestRegex     = testRegex;
	itsOwnsRegexFlag = widgetOwnsRegex;

	SetIsRequired();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXRegexInput::~JXRegexInput()
{
	if (itsOwnsRegexFlag)
		{
		jdelete itsTestRegex;
		}
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

JBoolean
JXRegexInput::InputValid()
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

		const JError err = itsTestRegex->SetPattern(text);
		if (err.OK())
			{
			return kJTrue;
			}
		else
			{
			err.ReportIfError();
			return kJFalse;
			}
		}
}
