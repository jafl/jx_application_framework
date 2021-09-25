/******************************************************************************
 JXStyleMenu.cpp

	Base class for changing font styles.  This menu is an action menu, so all
	messages that are broadcast are meaningless to outsiders.

	Derived classes must override the following functions:

		GetFontStyleForMenuUpdate
			Return the current font style so the appropriate checkboxes
			can be set on the menu.

		HandleMenuItem
			Respond to the selected menu item.  UpdateStyle() is provided
			for convenience.

	Derived classes can also override the following functions:

		UpdateMenu
			Derived classes can override to enable/disable items before
			calling this function.

	BASE CLASS = JXTextMenu

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#include "JXStyleMenu.h"
#include "JXStyleMenuDirector.h"
#include "JXChooseColorDialog.h"
#include "JXWindow.h"
#include "JXColorManager.h"
#include "jXActionDefs.h"
#include <jGlobals.h>
#include <jAssert.h>

static const JUtf8Byte* kMacMenuStr =
	"    Plain            %b %k Meta-T %i" kJXPlainStyleAction
	"%l| Bold             %b %k Meta-B %i" kJXBoldStyleAction
	"  | Italic           %b %k Meta-I %i" kJXItalicStyleAction
	"  | Underline        %b %k Meta-U %i" kJXUnderlineStyleAction
	"  | Double underline %b           %i" kJXDoubleUnderlineStyleAction
	"  | Strike           %b           %i" kJXStrikeStyleAction
	"%l| Black            %r           %i" kJXBlackStyleAction
	"  | Gray             %r           %i" kJXGrayStyleAction
	"  | Brown            %r           %i" kJXBrownStyleAction
	"  | Orange           %r           %i" kJXOrangeStyleAction
	"  | Red              %r           %i" kJXRedStyleAction
	"  | Dark red         %r           %i" kJXDarkRedStyleAction
	"  | Green            %r           %i" kJXGreenStyleAction
	"  | Blue             %r           %i" kJXBlueStyleAction
	"  | Light blue       %r           %i" kJXLightBlueStyleAction
	"  | Pink             %r           %i" kJXPinkStyleAction
	"%l| Other            %r           %i" kJXOtherColorStyleAction;

static const JUtf8Byte* kWinMenuStr =
	"    Plain            %b %h p %k Ctrl-T %i" kJXPlainStyleAction
	"%l| Bold             %b %h b %k Ctrl-B %i" kJXBoldStyleAction
	"  | Italic           %b %h i %k Ctrl-I %i" kJXItalicStyleAction
	"  | Underline        %b %h u %k Ctrl-U %i" kJXUnderlineStyleAction
	"  | Double underline %b %h d           %i" kJXDoubleUnderlineStyleAction
	"  | Strike           %b %h s           %i" kJXStrikeStyleAction
	"%l| Black            %r                %i" kJXBlackStyleAction
	"  | Gray             %r                %i" kJXGrayStyleAction
	"  | Brown            %r                %i" kJXBrownStyleAction
	"  | Orange           %r                %i" kJXOrangeStyleAction
	"  | Red              %r                %i" kJXRedStyleAction
	"  | Dark red         %r                %i" kJXDarkRedStyleAction
	"  | Green            %r                %i" kJXGreenStyleAction
	"  | Blue             %r                %i" kJXBlueStyleAction
	"  | Light blue       %r                %i" kJXLightBlueStyleAction
	"  | Pink             %r                %i" kJXPinkStyleAction
	"%l| Other            %r                %i" kJXOtherColorStyleAction;

// remember to update kColorCount and JXStyleMenuX()

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStyleMenu::JXStyleMenu
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextMenu(JGetString("Title::JXStyleMenu"), enclosure, hSizing, vSizing, x,y, w,h)
{
	JXStyleMenuX();
}

JXStyleMenu::JXStyleMenu
	(
	JXMenu*				owner,
	const JIndex		itemIndex,
	JXContainer*		enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	JXStyleMenuX();
}

// private

void
JXStyleMenu::JXStyleMenuX()
{
	itsChooseColorDialog = nullptr;

	if (JXMenu::GetDefaultStyle() == kMacintoshStyle)
	{
		SetMenuItems(kMacMenuStr);
	}
	else
	{
		SetShortcuts(JGetString("Shortcut::JXStyleMenu"));
		SetMenuItems(kWinMenuStr);
	}

	const JColorID blackColor = JColorManager::GetBlackColor();

	SetItemFontStyle(
		kBoldStyleCmd,         JFontStyle(true, false, 0, false, blackColor));
	SetItemFontStyle(
		kItalicStyleCmd,       JFontStyle(false, true, 0, false, blackColor));
	SetItemFontStyle(
		kUnderlineStyleCmd,    JFontStyle(false, false, 1, false, blackColor));
	SetItemFontStyle(
		kDblUnderlineStyleCmd, JFontStyle(false, false, 2, false, blackColor));
	SetItemFontStyle(
		kStrikeStyleCmd,       JFontStyle(false, false, 0, true, blackColor));

	assert( kColorCount == 11 );
	itsColorList[ 0] = blackColor;
	itsColorList[ 1] = JColorManager::GetGrayColor(60);
	itsColorList[ 2] = JColorManager::GetBrownColor();
	itsColorList[ 3] = JColorManager::GetOrangeColor();
	itsColorList[ 4] = JColorManager::GetRedColor();
	itsColorList[ 5] = JColorManager::GetDarkRedColor();
	itsColorList[ 6] = JColorManager::GetDarkGreenColor();
	itsColorList[ 7] = JColorManager::GetBlueColor();
	itsColorList[ 8] = JColorManager::GetLightBlueColor();
	itsColorList[ 9] = JColorManager::GetPinkColor();
	itsColorList[10] = JColorManager::GetDefaultBackColor();

	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStyleMenu::~JXStyleMenu()
{
}

/******************************************************************************
 SetCustomColor (private)

	Change the color associated with "other".

 ******************************************************************************/

inline void
JXStyleMenu::SetCustomColor
	(
	const JColorID color
	)
{
	itsColorList[ kColorCount-1 ] = color;
}

/******************************************************************************
 CreateMenuWindow (virtual protected)

 ******************************************************************************/

JXMenuDirector*
JXStyleMenu::CreateMenuWindow
	(
	JXWindowDirector* supervisor
	)
{
	auto* dir =
		jnew JXStyleMenuDirector(supervisor, this, GetTextMenuData());
	assert( dir != nullptr );
	return dir;
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXStyleMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateMenu();
	}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );

		const JIndex i = selection->GetIndex();
		if (i == kCustomColorCmd)
		{
			ChooseColor();
		}
		else
		{
			if (i >= kFirstColorCmd)
			{
				itsColorIndex = IndexToColor(i);
			}
			HandleMenuItem(i);
		}
	}

	else if (sender == itsChooseColorDialog &&
			 message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			itsColorIndex = itsChooseColorDialog->GetColor();
			SetCustomColor(itsColorIndex);
			HandleMenuItem(kCustomColorCmd);
		}
		itsChooseColorDialog = nullptr;
	}

	else
	{
		JXTextMenu::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateMenu (virtual protected)

	Derived classes can override to enable/disable items before calling this
	function.

 ******************************************************************************/

void
JXStyleMenu::UpdateMenu()
{
	const JFontStyle style = GetFontStyleForMenuUpdate();

	// styles

	bool plain = true;

	if (style.bold)
	{
		CheckItem(kBoldStyleCmd);
		plain = false;
	}

	if (style.italic)
	{
		CheckItem(kItalicStyleCmd);
		plain = false;
	}

	if (style.underlineCount == 1)
	{
		CheckItem(kUnderlineStyleCmd);
		plain = false;
	}
	else if (style.underlineCount == 2)
	{
		CheckItem(kDblUnderlineStyleCmd);
		plain = false;
	}
	else if (style.underlineCount > 0)
	{
		// no menu item to check, but it's not plain
		plain = false;
	}

	if (style.strike)
	{
		CheckItem(kStrikeStyleCmd);
		plain = false;
	}

	if (plain && style.color == JColorManager::GetBlackColor())
	{
		CheckItem(kPlainStyleCmd);
	}

	// color

	itsColorIndex = style.color;
	CheckItem(ColorToIndex(itsColorIndex));
}

/******************************************************************************
 UpdateStyle (protected)

	Convenience function that can be called from HandleMenuItem().

 ******************************************************************************/

void
JXStyleMenu::UpdateStyle
	(
	const JIndex	index,
	JFontStyle*		style
	)
{
	if (index == kPlainStyleCmd)
	{
		*style = JFontStyle();
	}

	else if (index == kBoldStyleCmd)
	{
		style->bold = !style->bold;
	}

	else if (index == kItalicStyleCmd)
	{
		style->italic = style->italic;
	}

	else if (index == kUnderlineStyleCmd && style->underlineCount != 1)
	{
		style->underlineCount = 1;
	}
	else if (index == kUnderlineStyleCmd)
	{
		style->underlineCount = 0;
	}

	else if (index == kDblUnderlineStyleCmd && style->underlineCount != 2)
	{
		style->underlineCount = 2;
	}
	else if (index == kDblUnderlineStyleCmd)
	{
		style->underlineCount = 0;
	}

	else if (index == kStrikeStyleCmd)
	{
		style->strike = !style->strike;
	}

	else if (index >= kFirstColorCmd)
	{
		style->color = itsColorIndex;
	}
}

/******************************************************************************
 ChooseColor (private)

 ******************************************************************************/

void
JXStyleMenu::ChooseColor()
{
	assert( itsChooseColorDialog == nullptr );

	JXWindowDirector* supervisor = GetWindow()->GetDirector();
	itsChooseColorDialog =
		jnew JXChooseColorDialog(supervisor, IndexToColor(kCustomColorCmd));
	assert( itsChooseColorDialog != nullptr );

	ListenTo(itsChooseColorDialog);
	itsChooseColorDialog->BeginDialog();
}

/******************************************************************************
 IndexToColor (protected)

 ******************************************************************************/

JColorID
JXStyleMenu::IndexToColor
	(
	const JIndex menuIndex
	)
	const
{
	assert( kFirstColorCmd <= menuIndex && menuIndex <= kCustomColorCmd );
	return itsColorList[ menuIndex - kFirstColorCmd ];
}

/******************************************************************************
 ColorToIndex (protected)

	Since this function must never fail, we use the "other" item to display
	the color if it is not in our list.

 ******************************************************************************/

JIndex
JXStyleMenu::ColorToIndex
	(
	const JColorID color
	)
	const
{
	for (JUnsignedOffset i=0; i<kColorCount-1; i++)
	{
		if (itsColorList[i] == color)
		{
			return kFirstColorCmd + i;
		}
	}

	// change color associated with "other" and return this index

	const_cast<JXStyleMenu*>(this)->SetCustomColor(color);
	return kCustomColorCmd;
}
