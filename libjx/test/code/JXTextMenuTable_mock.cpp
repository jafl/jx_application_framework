/******************************************************************************
 JXTextMenuTable.cpp

	Written by John Lindal.

 ******************************************************************************/

#include <JXTextMenuTable_mock.h>
#include <jx-af/jcore/jAssert.h>


/******************************************************************************
 AdjustFont (static)

 ******************************************************************************/

void
JXTextMenuTable::AdjustFont
	(
	JXDisplay*		display,
	const JIndex	colIndex,
	const JString&	text,
	JFont*			font
	)
{
}

/******************************************************************************
 GetTextWidth (static)

 ******************************************************************************/

JSize
JXTextMenuTable::GetTextWidth
	(
	JFontManager*	fontMgr,
	const JFont&	font,
	const JIndex	colIndex,
	const JString&	text
	)
{
	return 0;
}
