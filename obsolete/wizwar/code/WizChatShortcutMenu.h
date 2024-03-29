/******************************************************************************
 WizChatShortcutMenu.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_WizChatShortcutMenu
#define _H_WizChatShortcutMenu

#include <JXTextMenu.h>

class JXInputField;

class WizChatShortcutMenu : public JXTextMenu
{
public:

	WizChatShortcutMenu(JXInputField* message, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~WizChatShortcutMenu();

	void	CheckForShortcut();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXInputField*	itsMessage;		// not owned

private:

	void	InitMenu(const JIndex playerIndex);

	// not allowed

	WizChatShortcutMenu(const WizChatShortcutMenu& source);
	const WizChatShortcutMenu& operator=(const WizChatShortcutMenu& source);
};

#endif
