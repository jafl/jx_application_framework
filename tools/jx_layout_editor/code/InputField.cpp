/******************************************************************************
 InputField.cpp

	BASE CLASS = BaseWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "InputField.h"
#include "InputFieldPanel.h"
#include "LayoutContainer.h"
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
	itsMaxLength(0),
	itsWordWrapFlag(false),
	itsAcceptNewlineFlag(false)
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
	input >> itsWordWrapFlag >> itsAcceptNewlineFlag;

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
	output << itsWordWrapFlag << std::endl;
	output << itsAcceptNewlineFlag << std::endl;
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

	if (itsWordWrapFlag)
	{
		s += JString::newline;
		s += JGetString("WordWrap::InputField");
	}

	if (itsWordWrapFlag)
	{
		s += JString::newline;
		s += JGetString("AcceptNewline::InputField");
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
 GetClassName (virtual protected)

 ******************************************************************************/

JString
InputField::GetClassName()
	const
{
	return "JXInputField";
}

/******************************************************************************
 PrintCtorArgsWithComma (virtual protected)

 ******************************************************************************/

void
InputField::PrintCtorArgsWithComma
	(
	std::ostream&	output,
	const JString&	varName,
	JStringManager* stringdb
	)
	const
{
	if (itsWordWrapFlag || itsAcceptNewlineFlag)
	{
		output << itsWordWrapFlag << ", " << itsAcceptNewlineFlag << ',';
	}
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
InputField::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	if (itsMinLength > 1)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetMinLength(" << itsMinLength << ");" << std::endl;
	}
	else if (itsIsRequiredFlag)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetIsRequired();" << std::endl;
	}

	if (itsMaxLength > 0)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetMaxLength(" << itsMaxLength << ");" << std::endl;
	}

	if (!itsValidationPattern.IsEmpty())
	{
		const JString id = varName + "::validation" + GetLayoutContainer()->GetStringNamespace();
		stringdb->SetItem(id, itsRegexErrorMsg, JPtrArrayT::kDelete);

		indent.Print(output);
		varName.Print(output);
		output << "->SetValidationPattern(" << itsValidationPattern << ", " << id << ");" << std::endl;
	}
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
			itsValidationPattern, itsRegexErrorMsg,
			itsWordWrapFlag, itsAcceptNewlineFlag);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
InputField::SavePanelData()
{
	itsPanel->GetValues(&itsIsRequiredFlag, &itsMinLength, &itsMaxLength,
						&itsValidationPattern, &itsRegexErrorMsg,
						&itsWordWrapFlag, &itsAcceptNewlineFlag);
	itsPanel = nullptr;
}
