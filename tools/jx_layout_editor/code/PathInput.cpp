/******************************************************************************
 PathInput.cpp

	BASE CLASS = InputFieldBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "PathInput.h"
#include "PathInputPanel.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PathInput::PathInput
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
	itsPathRequiredFlag(true),
	itsAllowInvalidFlag(false),
	itsWriteFlag(false)
{
}

PathInput::PathInput
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
	input >> itsPathRequiredFlag >> itsAllowInvalidFlag;
	input >> itsWriteFlag;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PathInput::~PathInput()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
PathInput::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("PathInput") << std::endl;

	InputFieldBase::StreamOut(output);

	output << itsPathRequiredFlag << std::endl;
	output << itsAllowInvalidFlag << std::endl;
	output << itsWriteFlag << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
PathInput::ToString()
	const
{
	JString s = InputFieldBase::ToString();

	if (itsPathRequiredFlag)
	{
		s += JString::newline;
		s += JGetString("RequiredLabel::InputField");
	}

	if (itsAllowInvalidFlag)
	{
		s += JString::newline;
		s += JGetString("AllowInvalidFile::FileInput");
	}

	if (itsWriteFlag)
	{
		s += JString::newline;
		s += JGetString("RequireWritable::PathInput");
	}

	return s;
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
PathInput::GetClassName()
	const
{
	return "JXPathInput";
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
PathInput::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	indent.Print(output);
	varName.Print(output);
	output << "->SetIsRequired(" << itsPathRequiredFlag << ");" << std::endl;

	indent.Print(output);
	varName.Print(output);
	output << "->ShouldAllowInvalidPath(" << itsAllowInvalidFlag << ");" << std::endl;

	indent.Print(output);
	varName.Print(output);
	output << "->ShouldRequireWritable(" << itsWriteFlag << ");" << std::endl;
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
PathInput::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel =
		jnew PathInputPanel(dlog, itsPathRequiredFlag, itsAllowInvalidFlag, itsWriteFlag);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
PathInput::SavePanelData()
{
	itsPanel->GetValues(&itsPathRequiredFlag, &itsAllowInvalidFlag, &itsWriteFlag);
	itsPanel = nullptr;
}
