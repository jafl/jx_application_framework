/******************************************************************************
 JXRegexInput.cpp

	Maintains a line of editable text for entering a regular expression.

	BASE CLASS = JXInputField

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JXRegexInput.h"
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXRegexInput::JXRegexInput
	(
	JRegex*				testRegex,
	const bool		widgetOwnsRegex,
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

bool
JXRegexInput::InputValid()
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

		const JError err = itsTestRegex->SetPattern(text);
		if (err.OK())
			{
			return true;
			}
		else
			{
			err.ReportIfError();
			return false;
			}
		}
}
