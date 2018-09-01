/******************************************************************************
 JXExprInput.cpp

	An input field for entering variable names for the JExpr library.

	BASE CLASS = JXInputField

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXExprInput.h"
#include "JUserInputFunction.h"
#include <JXTextMenu.h>
#include <jGlobals.h>
#include <jAssert.h>

// Font menu

static const JCharacter* kFontMenuTitleStr    = "Font";
static const JCharacter* kFontMenuShortcutStr = "#F";
static const JCharacter* kMacFontMenuStr = "Normal      %r | Greek      %r";
static const JCharacter* kWinFontMenuStr = "Normal %h n %r | Greek %h g %r";

enum
{
	kNormalFontCmd = 1,
	kGreekFontCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

JXExprInput::JXExprInput
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
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsFontMenu = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXExprInput::~JXExprInput()
{
}

/******************************************************************************
 GetVarName

 ******************************************************************************/

JString
JXExprInput::GetVarName()
	const
{
	return JUserInputFunction::GetParseableText(*this);
}

/******************************************************************************
 SetVarName

 ******************************************************************************/

void
JXExprInput::SetVarName
	(
	const JCharacter* str
	)
{
	JUserInputFunction::SetParseableText(this, str);
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXExprInput::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == JPGetGreekCharPrefixChar())
		{
		const JFont font = GetCurrentFont();
		if (strcmp(font.GetName(), JGetDefaultFontName()) == 0)
			{
			SetCurrentFontName(JGetGreekFontName());
			}
		else
			{
			SetCurrentFontName(JGetDefaultFontName());
			}
		}
	else
		{
		JXInputField::HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 CreateFontMenu (static)

	Returns a Font menu that can then be passed to SetFontMenu.  We do
	it this way because a menu bar is likely to exist permanently, while
	input fields often come and go, especially in tables.

 ******************************************************************************/

JXTextMenu*
JXExprInput::CreateFontMenu
	(
	JXContainer* enclosure
	)
{
	JXTextMenu* menu = jnew JXTextMenu(kFontMenuTitleStr, enclosure,
									  kFixedLeft, kVElastic, 0,0, 10,10);
	assert( menu != nullptr );

	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		menu->SetMenuItems(kMacFontMenuStr);
		}
	else
		{
		menu->SetShortcuts(kFontMenuShortcutStr);
		menu->SetMenuItems(kWinFontMenuStr);
		}

	return menu;
}

/******************************************************************************
 SetFontMenu

	Give us a Font menu to use.

 ******************************************************************************/

void
JXExprInput::SetFontMenu
	(
	JXTextMenu* menu
	)
{
	itsFontMenu = menu;
	ListenTo(itsFontMenu);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXExprInput::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFontMenu && HasFocus() &&
		message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFontMenu();
		}
	else if (sender == itsFontMenu && HasFocus() &&
			 message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFontMenu(selection->GetIndex());
		}

	else
		{
		JXInputField::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateFontMenu (private)

 ******************************************************************************/

void
JXExprInput::UpdateFontMenu()
{
	itsFontMenu->EnableAll();

	JFont font = GetCurrentFont();
	if (JString::Compare(font.GetName(), JGetGreekFontName()) == 0)
		{
		itsFontMenu->CheckItem(kGreekFontCmd);
		}
	else
		{
		itsFontMenu->CheckItem(kNormalFontCmd);
		}
}

/******************************************************************************
 HandleFontMenu (private)

 ******************************************************************************/

void
JXExprInput::HandleFontMenu
	(
	const JIndex item
	)
{
	if (item == kNormalFontCmd)
		{
		SetCurrentFontName(JGetDefaultFontName());
		}
	else if (item == kGreekFontCmd)
		{
		SetCurrentFontName(JGetGreekFontName());
		}
}
