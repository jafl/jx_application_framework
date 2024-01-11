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
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	CoreWidget(layout, false, enclosure, hSizing, vSizing, x,y, w,h)
{
}

TextButtonBase::TextButtonBase
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
	CoreWidget(layout, input, enclosure, hSizing, vSizing, x,y, w,h)
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
	const JString id = varName + GetLayoutContainer()->GetStringNamespace();
	stringdb->SetItem(id, label, JPtrArrayT::kDelete);

	output << "JGetString(" << id << "),";
}

/******************************************************************************
 SharedPrintConfiguration (protected)

 ******************************************************************************/

void
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
		const JString id = varName + "::shortcuts" + GetLayoutContainer()->GetStringNamespace();
		stringdb->SetItem(id, shortcuts, JPtrArrayT::kDelete);

		indent.Print(output);
		varName.Print(output);
		output << "->SetShortcuts(JGetString(" << id << "));" << std::endl;
	}
}