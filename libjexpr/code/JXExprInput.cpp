/******************************************************************************
 JXExprInput.cpp

	An input field for entering variable names for the JExpr library.

	BASE CLASS = JXInputField

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jexpr/JXExprInput.h"
#include "jx-af/jexpr/JUserInputFunction.h"
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// Font menu

static const JUtf8Byte* kMacFontMenuStr = "Normal      %r | Greek      %r";
static const JUtf8Byte* kWinFontMenuStr = "Normal %h n %r | Greek %h g %r";

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
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h),
	itsGreekFlag(false)
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
	if (c == JUserInputFunction::kSwitchFontCharacter)
	{
		itsGreekFlag = ! itsGreekFlag;
	}
	else
	{
		JXInputField::HandleKeyPress(
			itsGreekFlag ? JUserInputFunction::ConvertToGreek(c) : c,
			keySym, modifiers);
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
	auto* menu =
		jnew JXTextMenu(JGetString("FontMenuTitle::JXExprInput"), enclosure,
						kFixedLeft, kVElastic, 0,0, 10,10);
	assert( menu != nullptr );
	menu->SetUpdateAction(JXMenu::kDisableNone);

	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
	{
		menu->SetMenuItems(kMacFontMenuStr);
	}
	else
	{
		menu->SetShortcuts(JGetString("FontMenuShortcut::JXExprInput"));
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
		const auto* selection =
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
	itsFontMenu->CheckItem(itsGreekFlag ? kGreekFontCmd : kNormalFontCmd);
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
		itsGreekFlag = false;
	}
	else if (item == kGreekFontCmd)
	{
		itsGreekFlag = true;
	}
}
