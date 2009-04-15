/*****************************************************************************
 JXGetPassword.cc

	BASE CLASS = virtual JBroadcaster

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include <JXGetPassword.h>
#include <JXGetPasswordDialog.h>
#include <JXWindow.h>
#include <JXApplication.h>

#include <jXGlobals.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXGetPassword::JXGetPassword()
{
	itsPasswordDir   = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXGetPassword::~JXGetPassword()
{
}

/******************************************************************************
 GetPassword

 ******************************************************************************/

JBoolean
JXGetPassword::GetPassword
	(
	JString* password
	)
{
	assert( itsPasswordDir == NULL );

	itsPasswordDir = new JXGetPasswordDialog(JXGetApplication());
	assert( itsPasswordDir != NULL );
	WaitForResponse();

	*password = itsPassword;
	return itsSuccess;
}

/******************************************************************************
 Receive (protected)

	Listen for response from active dialog.

 ******************************************************************************/

void
JXGetPassword::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPasswordDir && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		itsSuccess = info->Successful();
		if (itsSuccess)
			{
			itsPassword = itsPasswordDir->GetPassword();
			}
		itsPasswordDir   = NULL;
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 WaitForResponse (private)

 ******************************************************************************/

void
JXGetPassword::WaitForResponse()
{
	JXWindow* window = itsPasswordDir->GetWindow();
	window->PlaceAsDialogWindow();
	window->LockCurrentSize();

	ListenTo(itsPasswordDir);
	itsPasswordDir->BeginDialog();

	// display the inactive cursor in all the other windows

	JXApplication* app = JXGetApplication();
	app->DisplayInactiveCursor();

	// block with event loop running until we get a response

	while (itsPasswordDir != NULL)
		{
		app->HandleOneEventForWindow(window);
		}
}
