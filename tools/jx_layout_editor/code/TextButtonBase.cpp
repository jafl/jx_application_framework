/******************************************************************************
 TextButtonBase.cpp

	BASE CLASS = CoreWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "TextButtonBase.h"
#include "LayoutContainer.h"
#include <jx-af/jcore/JStringManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TextButtonBase::TextButtonBase
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
	CoreWidget(false, layout, hSizing, vSizing, x,y, w,h)
{
}

TextButtonBase::TextButtonBase
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
	CoreWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TextButtonBase::~TextButtonBase()
{
}

/******************************************************************************
 SharedPrintConfiguration (protected)

 ******************************************************************************/

bool
TextButtonBase::SharedPrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	const JString&	shortcuts,
	JStringManager*	stringdb
	)
	const
{
	if (!shortcuts.IsEmpty())
	{
		indent.Print(output);
		varName.Print(output);
		output << "->SetShortcuts(";
		PrintStringForArg(shortcuts, varName + "::shortcuts", stringdb, output);
		output << ");" << std::endl;
		return true;
	}
	else
	{
		return false;
	}
}
