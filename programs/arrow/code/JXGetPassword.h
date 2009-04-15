/******************************************************************************
 JXGetPassword.h

	Interface for the JXGetPassword class.

	Copyright © 1995-96 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXGetPassword
#define _H_JXGetPassword

#include <JBroadcaster.h>
#include <JString.h>

class JXGetPasswordDialog;

class JXGetPassword : virtual public JBroadcaster
{
public:

	JXGetPassword();

	virtual ~JXGetPassword();

	JBoolean GetPassword(JString* password);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXGetPasswordDialog*	itsPasswordDir;
	JBoolean				itsSuccess;
	JString					itsPassword;

private:

	void	WaitForResponse();

	// not allowed

	JXGetPassword(const JXGetPassword& source);
	const JXGetPassword& operator=(const JXGetPassword& source);
};

#endif
