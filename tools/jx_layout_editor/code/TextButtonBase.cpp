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
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	CoreWidget(input, layout, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TextButtonBase::~TextButtonBase()
{
}

/******************************************************************************
 SharedPrintCtorArgsWithComma (protected)

 ******************************************************************************/

void
TextButtonBase::SharedPrintCtorArgsWithComma
	(
	std::ostream&	output,
	const JString&	varName,
	const JString&	label,
	JStringManager* stringdb
	)
	const
{
	const JString id = varName + GetParentContainer()->GetStringNamespace();
	stringdb->SetItem(id, label, JPtrArrayT::kDelete);

	output << "JGetString(" << id << "),";
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
		const JString id = varName + "::shortcuts" + GetParentContainer()->GetStringNamespace();
		stringdb->SetItem(id, shortcuts, JPtrArrayT::kDelete);

		indent.Print(output);
		varName.Print(output);
		output << "->SetShortcuts(JGetString(" << id << "));" << std::endl;
		return true;
	}
	else
	{
		return false;
	}
}
