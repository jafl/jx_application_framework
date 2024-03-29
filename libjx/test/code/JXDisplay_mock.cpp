/******************************************************************************
 JXDisplay.cpp

	Written by John Lindal.

 ******************************************************************************/

#include <JXDisplay_mock.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXDisplay::JXDisplay
	(
	const bool isMacOS
	)
	:
	itsXDisplay(nullptr),
	itsColorManager(nullptr),
	itsMaxStringByteCount(1024),
	itsIsMacOSFlag(isMacOS),
	itsBounds(nullptr),
	itsLatestKeyModifiers(this),
	itsModifierKeymap(nullptr)
{
	for (JUnsignedOffset i=0; i<=kXModifierCount; i++)
	{
		itsJXKeyModifierMapping[i] = i;
	}

	itsJXKeyModifierMapping[ kJXNumLockKeyIndex    ] = kJXMod5KeyIndex;
	itsJXKeyModifierMapping[ kJXScrollLockKeyIndex ] = kJXMod5KeyIndex;
	itsJXKeyModifierMapping[ kJXMetaKeyIndex       ] = kJXMod1KeyIndex;
	itsJXKeyModifierMapping[ kJXAltKeyIndex        ] = kJXMod2KeyIndex;
	itsJXKeyModifierMapping[ kJXSuperKeyIndex      ] = kJXMod3KeyIndex;
	itsJXKeyModifierMapping[ kJXHyperKeyIndex      ] = kJXMod4KeyIndex;
}

/******************************************************************************
 KeycodeToModifier

	Converts an X keycode into an X modifier index.

 ******************************************************************************/

bool
JXDisplay::KeycodeToModifier
	(
	const KeyCode	keycode,
	JIndex*			modifierIndex
	)
	const
{
	return false;
}
