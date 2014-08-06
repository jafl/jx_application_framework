/******************************************************************************
 WizChatInput.h

	Interface for the WizChatInput class

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_WizChatInput
#define _H_WizChatInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class WizChatShortcutMenu;

class WizChatInput : public JXInputField
{
public:

	WizChatInput(JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~WizChatInput();

	void	SetShortcutMenu(WizChatShortcutMenu* menu);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

private:

	WizChatShortcutMenu*	itsShortcutMenu;

private:

	// not allowed

	WizChatInput(const WizChatInput& source);
	const WizChatInput& operator=(const WizChatInput& source);
};

#endif
