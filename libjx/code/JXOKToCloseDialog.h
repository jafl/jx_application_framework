/******************************************************************************
 JXOKToCloseDialog.h

	Interface for the JXOKToCloseDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXOKToCloseDialog
#define _H_JXOKToCloseDialog

#include "jx-af/jx/JXUNDialogBase.h"
#include <jx-af/jcore/JUserNotification.h>	// need definition of CloseAction

class JXTextButton;

class JXOKToCloseDialog : public JXUNDialogBase
{
public:

	JXOKToCloseDialog(JXDirector* supervisor, const JString& message);

	~JXOKToCloseDialog();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextButton* itsDiscardButton;

// end JXLayout

private:

	void	BuildWindow(const JString& message);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kGotResponse;

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
