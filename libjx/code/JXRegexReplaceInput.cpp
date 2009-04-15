/******************************************************************************
 JXRegexReplaceInput.cpp

	Maintains a line of editable text for entering a regular expression
	replace pattern.

	BASE CLASS = JXInputField

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXRegexReplaceInput.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXRegexReplaceInput::JXRegexReplaceInput
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

JXRegexReplaceInput::~JXRegexReplaceInput()
{
	if (itsOwnsRegexFlag)
		{
		delete itsTestRegex;
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
		const JString& text = GetText();

		if (!IsRequired() && text.IsEmpty())
			{
			return kJTrue;
			}

		const JError err = itsTestRegex->SetReplacePattern(text);
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
