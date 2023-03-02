/******************************************************************************
 JXOKToCloseDialog.h

	Interface for the JXOKToCloseDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXOKToCloseDialog
#define _H_JXOKToCloseDialog

#include "JXUNDialogBase.h"
#include <jx-af/jcore/JUserNotification.h>	// need definition of CloseAction

class JXTextButton;

class JXOKToCloseDialog : public JXUNDialogBase
{
public:

	JXOKToCloseDialog(const JString& message);

	~JXOKToCloseDialog() override;

	JUserNotification::CloseAction	GetCloseAction() const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JUserNotification::CloseAction	itsCloseAction;

// begin JXLayout

	JXTextButton* itsDiscardButton;

// end JXLayout

private:

	void	BuildWindow(const JString& message);
};


/******************************************************************************
 GetCloseAction

 ******************************************************************************/

inline JUserNotification::CloseAction
JXOKToCloseDialog::GetCloseAction()
	const
{
	return Cancelled() ? JUserNotification::kDontClose : itsCloseAction;
}

#endif
