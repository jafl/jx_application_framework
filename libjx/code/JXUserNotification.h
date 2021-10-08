/******************************************************************************
 JXUserNotification.h

	Interface for the JXUserNotification class.

	Copyright (C) 1995-96 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXUserNotification
#define _H_JXUserNotification

#include <jx-af/jcore/JUserNotification.h>
#include <jx-af/jcore/JBroadcaster.h>

class JXDialogDirector;
class JXOKToCloseDialog;

class JXUserNotification : public JUserNotification, virtual public JBroadcaster
{
public:

	JXUserNotification();

	~JXUserNotification();

	void	DisplayMessage(const JString& message) override;
	void	ReportError(const JString& message) override;

	bool	AskUserYes(const JString& message) override;
	bool	AskUserNo(const JString& message) override;

	CloseAction	OKToClose(const JString& message) override;

	bool	AcceptLicense() override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXDialogDirector*	itsCurrentDialog;
	bool				itsWarningResponse;

	JXOKToCloseDialog*	itsOKToCloseDialog;
	CloseAction			itsCloseAction;

private:

	void	WaitForResponse();
};

#endif
