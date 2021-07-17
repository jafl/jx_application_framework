/******************************************************************************
 CBProjectTableInput.cpp

	Input field for entering a path + file.

	BASE CLASS = JXFileInput

	Copyright © 2006 by John Lindal.

 ******************************************************************************/

#include "CBProjectTableInput.h"
#include "CBProjectTable.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBProjectTableInput::CBProjectTableInput
	(
	CBProjectTable*		table,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXFileInput(enclosure, hSizing, vSizing, x,y, w,h),
	itsProjectTable(table)
{
	ShouldAllowInvalidFile();
	ShouldRequireWritable(false);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBProjectTableInput::~CBProjectTableInput()
{
}

/******************************************************************************
 HandleKeyPress (virtual)

	Ctrl-Return renames the file.

 ******************************************************************************/

void
CBProjectTableInput::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsProjectTable != nullptr &&
		(c == '\r' || c == '\n') &&
		!modifiers.meta() && !modifiers.shift())
		{
		itsProjectTable->SetInputAction(
			modifiers.control() ? CBProjectTable::kRename : CBProjectTable::kRelink);
		}

	JXFileInput::HandleKeyPress(c, keySym, modifiers);
}
