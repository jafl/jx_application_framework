/******************************************************************************
 JXExprInput.cpp

	An input field for entering variable names for the JExpr library.

	BASE CLASS = JXInputField

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXExprInput.h"
#include "JUserInputFunction.h"
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// Font menu

static const JUtf8Byte* kFontMenuStr = "Normal %h n %r | Greek %h g %r";

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
	menu->SetShortcuts(JGetString("FontMenuShortcut::JXExprInput"));
	menu->SetMenuItems(kFontMenuStr);
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
	assert( itsFontMenu == nullptr );

	itsFontMenu = menu;
	itsFontMenu->AttachHandlers(this,
		&JXExprInput::UpdateFontMenu,
		&JXExprInput::HandleFontMenu);
}

/******************************************************************************
 UpdateFontMenu (private)

 ******************************************************************************/

void
JXExprInput::UpdateFontMenu()
{
	if (HasFocus())
	{
		itsFontMenu->CheckItem(itsGreekFlag ? kGreekFontCmd : kNormalFontCmd);
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
	if (!HasFocus())
	{
		return;
	}

	if (item == kNormalFontCmd)
	{
		itsGreekFlag = false;
	}
	else if (item == kGreekFontCmd)
	{
		itsGreekFlag = true;
	}
}
