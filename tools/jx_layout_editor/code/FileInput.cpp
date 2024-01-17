/******************************************************************************
 FileInput.cpp

	BASE CLASS = InputFieldBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "FileInput.h"
#include "FileInputPanel.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FileInput::FileInput
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
	itsFileRequiredFlag(true),
	itsAllowInvalidFlag(false),
	itsReadFlag(true),
	itsWriteFlag(true),
	itsExecFlag(false)
{
}

FileInput::FileInput
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
	input >> itsFileRequiredFlag >> itsAllowInvalidFlag;
	input >> itsReadFlag >> itsWriteFlag >> itsExecFlag;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FileInput::~FileInput()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
FileInput::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("FileInput") << std::endl;

	InputFieldBase::StreamOut(output);

	output << itsFileRequiredFlag << std::endl;
	output << itsAllowInvalidFlag << std::endl;
	output << itsReadFlag << std::endl;
	output << itsWriteFlag << std::endl;
	output << itsExecFlag << std::endl;
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
FileInput::ToString()
	const
{
	JString s = InputFieldBase::ToString();

	if (itsFileRequiredFlag)
	{
		s += JString::newline;
		s += JGetString("RequiredLabel::InputField");
	}

	if (itsAllowInvalidFlag)
	{
		s += JString::newline;
		s += JGetString("AllowInvalidFile::FileInput");
	}

	s += JString::newline;
	if (itsReadFlag)
	{
		s += "r ";
	}
	else
	{
		s += "- ";
	}

	if (itsWriteFlag)
	{
		s += "w ";
	}
	else
	{
		s += "- ";
	}

	if (itsExecFlag)
	{
		s += "x";
	}
	else
	{
		s += "-";
	}

	return s;
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
FileInput::GetClassName()
	const
{
	return "JXFileInput";
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
FileInput::PrintConfiguration
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
	output << "->SetIsRequired(" << itsFileRequiredFlag << ");" << std::endl;

	indent.Print(output);
	varName.Print(output);
	output << "->ShouldAllowInvalidFile(" << itsAllowInvalidFlag << ");" << std::endl;

	indent.Print(output);
	varName.Print(output);
	output << "->ShouldRequireReadable(" << itsReadFlag << ");" << std::endl;

	indent.Print(output);
	varName.Print(output);
	output << "->ShouldRequireWritable(" << itsWriteFlag << ");" << std::endl;

	indent.Print(output);
	varName.Print(output);
	output << "->ShouldRequireExecutable(" << itsExecFlag << ");" << std::endl;
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
FileInput::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel =
		jnew FileInputPanel(dlog, itsFileRequiredFlag, itsAllowInvalidFlag,
			itsReadFlag, itsWriteFlag, itsExecFlag);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
FileInput::SavePanelData()
{
	itsPanel->GetValues(&itsFileRequiredFlag, &itsAllowInvalidFlag,
						&itsReadFlag, &itsWriteFlag, &itsExecFlag);
	itsPanel = nullptr;
}
