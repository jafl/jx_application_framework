/******************************************************************************
 JXOKToCloseDialog.h

	Interface for the JXOKToCloseDialog class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXOKToCloseDialog
#define _H_JXOKToCloseDialog

#include <JXUNDialogBase.h>
#include <JUserNotification.h>	// need definition of CloseAction

class JXTextButton;

class JXOKToCloseDialog : public JXUNDialogBase
{
public:

	JXOKToCloseDialog(JXDirector* supervisor, const JCharacter* message);

	virtual ~JXOKToCloseDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

// begin JXLayout

	JXTextButton* itsDiscardButton;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* message);

	// not allowed

	JXOKToCloseDialog(const JXOKToCloseDialog& source);
	const JXOKToCloseDialog& operator=(const JXOKToCloseDialog& source);

public:

	// JBroadcaster messages

	static const JCharacter* kGotResponse;

	class GotResponse : public JBroadcaster::Message
	{
	public:

		GotResponse(const JUserNotification::CloseAction response)
			:
			JBroadcaster::Message(kGotResponse),
			itsResponse(response)
			{ };

		JUserNotification::CloseAction
		GetResponse() const
		{
			return itsResponse;
		};

	private:

		JUserNotification::CloseAction	itsResponse;
	};
};

#endif
