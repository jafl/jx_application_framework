/******************************************************************************
 JXDisplay.cpp

	Written by John Lindal.

 ******************************************************************************/

#include <JXDisplay_mock.h>
#include <jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXDisplay::JXDisplay
	(
	const JBoolean isOSX
	)
	:
	itsXDisplay(nullptr),
	itsColorManager(nullptr),
	itsMaxStringByteCount(1024),
	itsIsOSXFlag(isOSX),
	itsBounds(nullptr),
	itsLatestKeyModifiers(this),
	itsModifierKeymap(nullptr)
{
	for (JIndex i=0; i<=kXModifierCount; i++)
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

JBoolean
JXDisplay::KeycodeToModifier
	(
	const KeyCode	keycode,
	JIndex*			modifierIndex
	)
	const
{
	return kJFalse;
}
