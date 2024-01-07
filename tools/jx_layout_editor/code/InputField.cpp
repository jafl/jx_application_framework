/******************************************************************************
 InputField.cpp

	BASE CLASS = BaseWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "InputField.h"
#include "InputFieldPanel.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

InputField::InputField
	(
	LayoutContainer*	layout,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	BaseWidget(layout, true, enclosure, hSizing, vSizing, x,y, w,h),
	itsIsRequiredFlag(false),
	itsMinLength(0),
	itsMaxLength(0)
{
	InputFieldX();
}

InputField::InputField
	(
	LayoutContainer*	layout,
	std::istream&		input,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	BaseWidget(layout, input, enclosure, hSizing, vSizing, x,y, w,h)
{
	input >> itsIsRequiredFlag >> itsMinLength >> itsMaxLength;
	input >> itsValidationPattern >> itsRegexErrorMsg;

	InputFieldX();
}

// private

void
InputField::InputFieldX()
{
	SetBorderWidth(2);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

InputField::~InputField()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
InputField::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("InputField") << std::endl;

	BaseWidget::StreamOut(output);

	output << itsIsRequiredFlag << std::endl;
	output << itsMinLength << std::endl;
	output << itsMaxLength << std::endl;
	output << itsValidationPattern << std::endl;
	output << itsRegexErrorMsg << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
InputField::ToString()
	const
{
	JString s = BaseWidget::ToString();
	if (itsIsRequiredFlag)
	{
		s += JString::newline;
		s += JGetString("RequiredLabel::InputField");
	}

	if (itsMinLength > 0)
	{
		s += JString::newline;
		s += JGetString("MinLengthLabel::InputField");
		s += JString((JUInt64) itsMinLength);
	}

	if (itsMaxLength > 0)
	{
		s += JString::newline;
		s += JGetString("MaxLengthLabel::InputField");
		s += JString((JUInt64) itsMaxLength);
	}

	if (!itsValidationPattern.IsEmpty())
	{
		s += JString::newline;
		s += JGetString("ValidationLabel:InputField");
		s += itsValidationPattern;
		s += JString::newline;
		s += JGetString("ErrorMsgLabel:InputField");
		s += itsRegexErrorMsg;
	}

	return s;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
InputField::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	p.SetPenColor(JColorManager::GetWhiteColor());
	p.SetFilling(true);
	p.Rect(GetFrameLocal());
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
InputField::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawDownFrame(p, frame, 2);
	DrawSelection(p, frame);
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
InputField::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel =
		jnew InputFieldPanel(dlog, itsIsRequiredFlag, itsMinLength, itsMaxLength,
			itsValidationPattern, itsRegexErrorMsg);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
InputField::SavePanelData()
{
	itsPanel->GetValues(&itsIsRequiredFlag, &itsMinLength, &itsMaxLength,
						&itsValidationPattern, &itsRegexErrorMsg);
	itsPanel = nullptr;
}
