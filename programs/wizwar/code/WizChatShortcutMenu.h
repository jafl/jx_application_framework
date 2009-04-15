/******************************************************************************
 WizChatShortcutMenu.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_WizChatShortcutMenu
#define _H_WizChatShortcutMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXInputField*	itsMessage;		// not owned

private:

	void	InitMenu(const JIndex playerIndex);

	// not allowed

	WizChatShortcutMenu(const WizChatShortcutMenu& source);
	const WizChatShortcutMenu& operator=(const WizChatShortcutMenu& source);
};

#endif
