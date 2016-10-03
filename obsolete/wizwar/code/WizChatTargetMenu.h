/******************************************************************************
 WizChatTargetMenu.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_WizChatTargetMenu
#define _H_WizChatTargetMenu

#include <JXTextMenu.h>

class JXInputField;
class WizChatShortcutMenu;

class WizChatTargetMenu : public JXTextMenu
{
public:

	WizChatTargetMenu(JXInputField* message, WizChatShortcutMenu* msgMenu,
					  JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~WizChatTargetMenu();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXInputField*			itsMessage;			// not owned
	WizChatShortcutMenu*	itsMessageMenu;		// not owned

private:

	void	InitMenu(const JSize count);

	// not allowed

	WizChatTargetMenu(const WizChatTargetMenu& source);
	const WizChatTargetMenu& operator=(const WizChatTargetMenu& source);

public:

	// JBroadcaster messages

	static const JCharacter* kSendChatMessage;

	class SendChatMessage : public JBroadcaster::Message
		{
		public:

			SendChatMessage(const JIndex receiverIndex, const JCharacter* msg)
				:
				JBroadcaster::Message(kSendChatMessage),
				itsReceiverIndex(receiverIndex),
				itsMessage(msg)
				{ };

			JIndex
			GetReceiverIndex() const
			{
				return itsReceiverIndex;
			};

			const JCharacter*
			GetMessage() const
			{
				return itsMessage;
			};

		private:

			const JIndex		itsReceiverIndex;
			const JCharacter*	itsMessage;
		};
};

#endif
