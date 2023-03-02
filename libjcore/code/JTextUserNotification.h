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

	~JTextUserNotification() override;

	void	DisplayMessage(const JString& message) override;
	void	ReportError(const JString& message) override;

	bool	AskUserYes(const JString& message) override;
	bool	AskUserNo(const JString& message) override;

	CloseAction	OKToClose(const JString& message) override;

	bool	AcceptLicense() override;
};

#endif
