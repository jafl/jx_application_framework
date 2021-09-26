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

	virtual ~JXUserNotification();

	virtual void	DisplayMessage(const JString& message) override;
	virtual void	ReportError(const JString& message) override;

	virtual bool	AskUserYes(const JString& message) override;
	virtual bool	AskUserNo(const JString& message) override;

	virtual CloseAction	OKToClose(const JString& message) override;

	virtual bool	AcceptLicense() override;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXDialogDirector*	itsCurrentDialog;
	bool				itsWarningResponse;

	JXOKToCloseDialog*	itsOKToCloseDialog;
	CloseAction			itsCloseAction;

private:

	void	WaitForResponse();
};

#endif
