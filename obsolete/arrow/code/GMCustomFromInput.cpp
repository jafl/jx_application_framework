/******************************************************************************
 GMCustomFromInput.cc

	BASE CLASS = public JXInputField

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#include <GMCustomFromInput.h>

#include <JXTextCheckbox.h>

#include <JRegex.h>
#include <JUserNotification.h>

#include <jXGlobals.h>

#include <jAssert.h>

static const JRegex kMinimalFromLine("<([^<>]*)>");

/******************************************************************************
 Constructor

 *****************************************************************************/

GMCustomFromInput::GMCustomFromInput
	(
	JXTextCheckbox*	cb,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h),
	itsControlCB(cb)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMCustomFromInput::~GMCustomFromInput()
{
}

/******************************************************************************
 InputValid (virtual protected)

 ******************************************************************************/

JBoolean
GMCustomFromInput::InputValid()
{
	if (!JXInputField::InputValid())
		{
		return kJFalse;
		}
	else if (itsControlCB == NULL || itsControlCB->IsChecked())
		{
		const JString& text = GetText();
		JArray<JIndexRange> subList;
		JBoolean matched = kMinimalFromLine.Match(text, &subList);
		if (!matched)
			{
			JString report("The custom from line must be in the form:\n\n\tReal Name <user@domain>");
			JGetUserNotification()->ReportError(report);
			return kJFalse;
			}
		}
	return kJTrue;
}
