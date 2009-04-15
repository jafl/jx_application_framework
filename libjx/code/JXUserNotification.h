/******************************************************************************
 JXUserNotification.h

	Interface for the JXUserNotification class.

	Copyright © 1995-96 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXUserNotification
#define _H_JXUserNotification

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUserNotification.h>
#include <JBroadcaster.h>

class JXDialogDirector;
class JXOKToCloseDialog;

class JXUserNotification : public JUserNotification, virtual public JBroadcaster
{
public:

	JXUserNotification();

	virtual ~JXUserNotification();

	virtual void		DisplayMessage(const JCharacter* message);
	virtual void		ReportError(const JCharacter* message);

	virtual JBoolean	AskUserYes(const JCharacter* message);
	virtual JBoolean	AskUserNo(const JCharacter* message);

	virtual CloseAction	OKToClose(const JCharacter* message);

	virtual JBoolean	AcceptLicense();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

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
