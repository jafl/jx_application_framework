/******************************************************************************
 IntegerInput.cpp

	BASE CLASS = InputFieldBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "IntegerInput.h"
#include "IntegerInputPanel.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

IntegerInput::IntegerInput
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
	itsMinValue(0),
	itsHasMaxValue(false),
	itsMaxValue(0)
{
}

IntegerInput::IntegerInput
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

IntegerInput::~IntegerInput()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
IntegerInput::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("IntegerInput") << std::endl;

	InputFieldBase::StreamOut(output);

	output << itsIsRequiredFlag << std::endl;
	output << itsHasMinValue << ' ' << itsMinValue << std::endl;
	output << itsHasMaxValue << ' ' << itsMaxValue << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
IntegerInput::ToString()
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
IntegerInput::GetClassName()
	const
{
	return "JXIntegerInput";
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
IntegerInput::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	bool used = false;

	if (!itsIsRequiredFlag)
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetIsRequired(false);" << std::endl;
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
IntegerInput::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel =
		jnew IntegerInputPanel(dlog, itsIsRequiredFlag,
			itsHasMinValue, itsMinValue,
			itsHasMaxValue, itsMaxValue);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
IntegerInput::SavePanelData()
{
	itsPanel->GetValues(&itsIsRequiredFlag,
						&itsHasMinValue, &itsMinValue,
						&itsHasMaxValue, &itsMaxValue);
	itsPanel = nullptr;
}
