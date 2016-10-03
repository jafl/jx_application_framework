/******************************************************************************
 WizChatDirector.h

	Interface for the WizChatDirector class

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_WizChatDirector
#define _H_WizChatDirector

#include <JXWindowDirector.h>
#include <jColor.h>
#include "Wiz2War.h"	// need definition of messages

class JXTextMenu;
class JXStaticText;
class WizChatInput;
class WizChatShortcutMenu;
class WizChatTargetMenu;

class WizChatDirector : public JXWindowDirector
{
public:

	WizChatDirector(JXDirector* supervisor);
	WizChatDirector(istream& input, const JFileVersion vers,
					JXDirector* supervisor);

	virtual ~WizChatDirector();

	void	WritePrefs(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXStaticText*	itsMessageDisplay;
	JBoolean		itsDoubleSpaceFlag;

	JXTextMenu*	itsActionsMenu;
	JXTextMenu*	itsPrefsMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout

	WizChatInput*        itsChatInput;
	WizChatShortcutMenu* itsChatShortcutMenu;
	WizChatTargetMenu*   itsChatTargetMenu;

// end JXLayout

private:

	void	WizChatDirectorX();
	void	BuildWindow();

	void	NotifyPlayerJoined(const Wiz2War::PlayerJoined& info);
	void	NotifyPlayerLeft(const JIndex index, const JBoolean timeout);
	void	EchoMessage(const JIndex receiverIndex, const JCharacter* msg);
	void	AppendMessage(const JIndex senderIndex, const JCharacter* msg,
						  const JColorIndex	color = 0);

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	UpdateHelpMenu();
	void	HandleHelpMenu(const JIndex index);

	// not allowed

	WizChatDirector(const WizChatDirector& source);
	const WizChatDirector& operator=(const WizChatDirector& source);
};

#endif
