/******************************************************************************
 JXContainer.cpp

	Written by John Lindal.

 ******************************************************************************/

#include <JXContainer_mock.h>
#include <JXDisplay_mock.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXContainer::JXContainer
	(
	JXDisplay*		d,
	JFontManager*	f
	)
	:
	itsDisplay(d),
	itsFontManager(f)
{
}

/******************************************************************************
 GetDisplay

 ******************************************************************************/

JXDisplay*
JXContainer::GetDisplay()
	const
{
	return itsDisplay;
}

/******************************************************************************
 GetFontManager

 ******************************************************************************/

JFontManager*
JXContainer::GetFontManager()
	const
{
	return itsFontManager;
}
