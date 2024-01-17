/******************************************************************************
 FloatInput.cpp

	BASE CLASS = InputFieldBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "FloatInput.h"
#include "FloatInputPanel.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FloatInput::FloatInput
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
	itsHasMinValue(false),
	itsMinValue(0.0),
	itsHasMaxValue(false),
	itsMaxValue(0.0)
{
}

FloatInput::FloatInput
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
	input >> itsIsRequiredFlag;
	input >> itsHasMinValue >> itsMinValue;
	input >> itsHasMaxValue >> itsMaxValue;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FloatInput::~FloatInput()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
FloatInput::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("FloatInput") << std::endl;

	InputFieldBase::StreamOut(output);

	output << itsIsRequiredFlag << std::endl;
	output << itsHasMinValue << ' ' << itsMinValue << std::endl;
	output << itsHasMaxValue << ' ' << itsMaxValue << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
FloatInput::ToString()
	const
{
	JString s = InputFieldBase::ToString();

	if (itsIsRequiredFlag)
	{
		s += JString::newline;
		s += JGetString("RequiredLabel::InputField");
	}

	if (itsHasMinValue)
	{
		s += JString::newline;
		s += JGetString("MinValueLabel::InputField");
		s += JString(itsMinValue);
	}

	if (itsHasMaxValue)
	{
		s += JString::newline;
		s += JGetString("MaxValueLabel::InputField");
		s += JString(itsMaxValue);
	}

	return s;
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
FloatInput::GetClassName()
	const
{
	return "JXFloatInput";
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
FloatInput::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	bool used = false;

	if (itsIsRequiredFlag)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetIsRequired();" << std::endl;
		used = true;
	}

	if (itsHasMinValue)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetLowerLimit(" << itsMinValue << ");" << std::endl;
		used = true;
	}

	if (itsHasMaxValue)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetUpperLimit(" << itsMaxValue << ");" << std::endl;
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
FloatInput::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel =
		jnew FloatInputPanel(dlog, itsIsRequiredFlag,
			itsHasMinValue, itsMinValue,
			itsHasMaxValue, itsMaxValue);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
FloatInput::SavePanelData()
{
	itsPanel->GetValues(&itsIsRequiredFlag,
						&itsHasMinValue, &itsMinValue,
						&itsHasMaxValue, &itsMaxValue);
	itsPanel = nullptr;
}
