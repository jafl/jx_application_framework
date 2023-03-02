/******************************************************************************
 JXUserNotification.h

	Interface for the JXUserNotification class.

	Copyright (C) 1995-96 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXUserNotification
#define _H_JXUserNotification

#include <jx-af/jcore/JUserNotification.h>

class JXUserNotification : public JUserNotification
{
public:

	JXUserNotification();

	~JXUserNotification() override;

	void	DisplayMessage(const JString& message) override;
	void	ReportError(const JString& message) override;

	bool	AskUserYes(const JString& message) override;
	bool	AskUserNo(const JString& message) override;

	CloseAction	OKToClose(const JString& message) override;

	bool	AcceptLicense() override;
};

#endif
