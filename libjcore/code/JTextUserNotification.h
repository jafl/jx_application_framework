/******************************************************************************
 JTextUserNotification.h

	Interface for the JTextUserNotification class.

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUserTextNotification
#define _H_JUserTextNotification

#include "JUserNotification.h"

class JTextUserNotification : public JUserNotification
{
public:

	JTextUserNotification();

	virtual ~JTextUserNotification();

	virtual void		DisplayMessage(const JString& message);
	virtual void		ReportError(const JString& message);

	virtual bool	AskUserYes(const JString& message);
	virtual bool	AskUserNo(const JString& message);

	virtual CloseAction	OKToClose(const JString& message);

	virtual bool	AcceptLicense();

private:

	// not allowed

	JTextUserNotification(const JTextUserNotification& source);
	const JTextUserNotification& operator=(const JTextUserNotification& source);
};

#endif
