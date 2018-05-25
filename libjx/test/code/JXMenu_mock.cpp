/******************************************************************************
 JXMenu.cpp

	Written by John Lindal.

 ******************************************************************************/

#include <JXMenu_mock.h>
#include <JXDisplay_mock.h>
#include <jAssert.h>

JXMenu::Style JXMenu::theDefaultStyle = JXMenu::kMacintoshStyle;
JXMenu::Style JXMenu::theDisplayStyle = JXMenu::kWindowsStyle;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMenu::JXMenu
	(
	JXDisplay*		d,
	JFontManager*	f
	)
	:
	JXContainer(d, f)
{
}

/******************************************************************************
 AdjustAppearance (private)

 ******************************************************************************/

void
JXMenu::AdjustAppearance()
{
}

/******************************************************************************
 AdjustNMShortcutModifier (static)

 ******************************************************************************/

JXModifierKey
JXMenu::AdjustNMShortcutModifier
	(
	const JXModifierKey key
	)
{
	if ((theDisplayStyle == kMacintoshStyle && theDefaultStyle == kWindowsStyle) ||
		(theDisplayStyle == kWindowsStyle   && theDefaultStyle == kMacintoshStyle))
		{
		if (key == kJXControlKeyIndex)
			{
			return kJXMetaKeyIndex;
			}
		else if (key == kJXMetaKeyIndex)
			{
			return kJXControlKeyIndex;
			}
		}

	return key;
}
