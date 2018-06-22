/******************************************************************************
 JXMenu.h

	Mock for the JXMenu class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_JXMenu
#define _H_JXMenu

#include <JXContainer_mock.h>
#include <JXKeyModifiers.h>

class JXMenu : public JXContainer
{
public:

	enum ItemType
	{
		kPlainType,
		kCheckboxType,
		kRadioType
	};

	enum UpdateAction
	{
		kDisableNone,
		kDisableSingles,
		kDisableAll
	};

	enum Style
	{
		kMacintoshStyle,
		kWindowsStyle
	};

public:

	JXMenu(JXDisplay* d, JFontManager* f);

	static JXModifierKey	AdjustNMShortcutModifier(const JXModifierKey key);

	static void	SetDisplayStyle(const Style style);

private:

	static Style	theDefaultStyle;
	static Style	theDisplayStyle;

private:

	void	AdjustAppearance();
};

inline void
JXMenu::SetDisplayStyle
	(
	const Style style
	)
{
	theDisplayStyle = style;
}

#endif
