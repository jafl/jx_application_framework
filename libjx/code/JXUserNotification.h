/******************************************************************************
 JXUserNotification.h

	Interface for the JXUserNotification class.

	Copyright (C) 1995-96 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXUserNotification
#define _H_JXUserNotification

#include <JUserNotification.h>
#include <JBroadcaster.h>

class JXDialogDirector;
class JXOKToCloseDialog;

class JXUserNotification : public JUserNotification, virtual public JBroadcaster
{
public:

	JXUserNotification();

	virtual ~JXUserNotification();

	virtual void		DisplayMessage(const JString& message);
	virtual void		ReportError(const JString& message);

	virtual JBoolean	AskUserYes(const JString& message);
	virtual JBoolean	AskUserNo(const JString& message);

	virtual CloseAction	OKToClose(const JString& message);

	virtual JBoolean	AcceptLicense();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXDialogDirector*	itsCurrentDialog;
	JBoolean			itsWarningResponse;

	JXOKToCloseDialog*	itsOKToCloseDialog;
	CloseAction			itsCloseAction;

private:

	void	WaitForResponse();

	// not allowed

	JXUserNotification(const JXUserNotification& source);
	const JXUserNotification& operator=(const JXUserNotification& source);
};

#endif
