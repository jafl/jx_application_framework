/******************************************************************************
 WizChatInput.h

	Interface for the WizChatInput class

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_WizChatInput
#define _H_WizChatInput

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

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;

private:

	WizChatShortcutMenu*	itsShortcutMenu;

private:

	// not allowed

	WizChatInput(const WizChatInput& source);
	const WizChatInput& operator=(const WizChatInput& source);
};

#endif
