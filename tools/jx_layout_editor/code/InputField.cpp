/******************************************************************************
 InputField.cpp

	BASE CLASS = InputFieldBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "InputField.h"
#include "InputFieldPanel.h"
#include "LayoutContainer.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

InputField::InputField
	(
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	InputFieldBase(layout, hSizing, vSizing, x,y, w,h),
	itsIsRequiredFlag(false),
	itsMinLength(0),
	itsMaxLength(0),
	itsWordWrapFlag(false),
	itsAcceptNewlineFlag(false)
{
}

InputField::InputField
	(
	std::istream&		input,
	const JFileVersion	vers,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	InputFieldBase(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	input >> itsIsRequiredFlag >> itsMinLength >> itsMaxLength;
	input >> itsValidationPattern;
	if (vers >= 1)
	{
		input >> itsValidationFlags;
	}
	input >> itsRegexErrorMsg;
	input >> itsWordWrapFlag >> itsAcceptNewlineFlag;
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

	InputFieldBase::StreamOut(output);

	output << itsIsRequiredFlag << std::endl;
	output << itsMinLength << std::endl;
	output << itsMaxLength << std::endl;
	output << itsValidationPattern << std::endl;
	output << itsValidationFlags << std::endl;
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
	JString s = InputFieldBase::ToString();

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
		if (!itsValidationFlags.IsEmpty())
		{
			s += "   (";
			s += itsValidationFlags;
			s += ")";
		}
		s += JString::newline;
		s += JGetString("ErrorMsgLabel:InputField");
		s += itsRegexErrorMsg;
	}

	if (itsWordWrapFlag)
	{
		s += JString::newline;
		s += JGetString("WordWrap::InputField");
	}

	if (itsAcceptNewlineFlag)
	{
		s += JString::newline;
		s += JGetString("AcceptNewline::InputField");
	}

	return s;
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
		output << itsWordWrapFlag << ", " << itsAcceptNewlineFlag << ", ";
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
	bool used = false;

	if (itsMinLength > 1)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetMinLength(" << itsMinLength << ");" << std::endl;
		used = true;
	}
	else if (itsIsRequiredFlag)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetIsRequired();" << std::endl;
		used = true;
	}

	if (itsMaxLength > 0)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetMaxLength(" << itsMaxLength << ");" << std::endl;
		used = true;
	}

	if (!itsValidationPattern.IsEmpty())
	{
		const JString id = varName + "::validation" + GetParentContainer()->GetStringNamespace();
		stringdb->SetItem(id, itsRegexErrorMsg, JPtrArrayT::kDelete);

		indent.Print(output);
		varName.Print(output);
		output << "->SetValidationPattern(jnew JRegex(" << itsValidationPattern;
		if (!itsValidationFlags.IsEmpty())
		{
			output << ", " << itsValidationFlags;
		}
		output << "), " << id << ");" << std::endl;
		used = true;
	}

	if (!used)
	{
		InputFieldBase::PrintConfiguration(output, indent, varName, stringdb);
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
			itsValidationPattern, itsValidationFlags, itsRegexErrorMsg,
			itsWordWrapFlag, itsAcceptNewlineFlag);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
InputField::SavePanelData()
{
	itsPanel->GetValues(&itsIsRequiredFlag, &itsMinLength, &itsMaxLength,
						&itsValidationPattern, &itsValidationFlags, &itsRegexErrorMsg,
						&itsWordWrapFlag, &itsAcceptNewlineFlag);
	itsPanel = nullptr;
}
