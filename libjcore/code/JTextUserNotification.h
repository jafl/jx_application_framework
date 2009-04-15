/******************************************************************************
 JTextUserNotification.h

	Interface for the JTextUserNotification class.

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JUserTextNotification
#define _H_JUserTextNotification

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUserNotification.h>

class JTextUserNotification : public JUserNotification
{
public:

	JTextUserNotification();

	virtual ~JTextUserNotification();

	virtual void		DisplayMessage(const JCharacter* message);
	virtual void		ReportError(const JCharacter* message);

	virtual JBoolean	AskUserYes(const JCharacter* message);
	virtual JBoolean	AskUserNo(const JCharacter* message);

	virtual CloseAction	OKToClose(const JCharacter* message);

	virtual JBoolean	AcceptLicense();

private:

	// not allowed

	JTextUserNotification(const JTextUserNotification& source);
	const JTextUserNotification& operator=(const JTextUserNotification& source);
};

#endif
