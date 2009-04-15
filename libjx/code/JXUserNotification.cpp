/******************************************************************************
 JXUserNotification.cpp

	Class for notifying the user of something via the JX Library.
	Since jAssert probably uses us to display the error, we can't use
	jAssert or we might generate an infinite loop.

	*** Do not use this for displaying failed assertion messages.
		This will allow the event handler to continue operating, which is
		usually fatal after an assert fires.

	BASE CLASS = JUserNotification, virtual JBroadcaster

	Copyright © 1995-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXUserNotification.h>
#include <JXMessageDialog.h>
#include <JXErrorDialog.h>
#include <JXWarningDialog.h>
#include <JXOKToCloseDialog.h>
#include <JXAcceptLicenseDialog.h>
#include <JXWindow.h>
#include <jXGlobals.h>
#include <JString.h>
#include <stdio.h>
#include <stdlib.h>
#include <jAssert.h>

#undef assert

static int __jXUNassert(char*, char*, int);

#ifdef NDEBUG
	#define assert(x)	((void) 0)
#else
	#define assert(x)	((void) ((x) || __jXUNassert(#x, __FILE__, __LINE__)))
#endif

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUserNotification::JXUserNotification()
	:
	JUserNotification()
{
	itsCurrentDialog   = NULL;
	itsOKToCloseDialog = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXUserNotification::~JXUserNotification()
{
}

/******************************************************************************
 DisplayMessage

 ******************************************************************************/

void
JXUserNotification::DisplayMessage
	(
	const JCharacter* message
	)
{
	assert( itsCurrentDialog == NULL );

	if (!IsSilent())
		{
		(JXGetApplication())->PrepareForBlockingWindow();

		itsCurrentDialog = new JXMessageDialog(JXGetApplication(), message);
		assert( itsCurrentDialog != NULL );
		WaitForResponse();
		}
}

/******************************************************************************
 ReportError

 ******************************************************************************/

void
JXUserNotification::ReportError
	(
	const JCharacter* message
	)
{
	assert( itsCurrentDialog == NULL );

	if (!IsSilent())
		{
		(JXGetApplication())->PrepareForBlockingWindow();

		itsCurrentDialog = new JXErrorDialog(JXGetApplication(), message);
		assert( itsCurrentDialog != NULL );
		WaitForResponse();
		}
}

/******************************************************************************
 AskUserYes

	Display the message (asking for a y/n answer) and wait for a response.

 ******************************************************************************/

JBoolean
JXUserNotification::AskUserYes
	(
	const JCharacter* message
	)
{
	assert( itsCurrentDialog == NULL );

	(JXGetApplication())->PrepareForBlockingWindow();

	itsCurrentDialog = new JXWarningDialog(JXGetApplication(), message);
	assert( itsCurrentDialog != NULL );
	WaitForResponse();

	return itsWarningResponse;
}

/******************************************************************************
 AskUserNo

	Display the message (asking for a y/n answer) and wait for a response.

 ******************************************************************************/

JBoolean
JXUserNotification::AskUserNo
	(
	const JCharacter* message
	)
{
	return AskUserYes(message);
}

/******************************************************************************
 OKToClose

 ******************************************************************************/

JUserNotification::CloseAction
JXUserNotification::OKToClose
	(
	const JCharacter* message
	)
{
	assert( itsCurrentDialog == NULL && itsOKToCloseDialog == NULL );

	(JXGetApplication())->PrepareForBlockingWindow();

	itsOKToCloseDialog = new JXOKToCloseDialog(JXGetApplication(), message);
	assert( itsOKToCloseDialog != NULL );

	itsCurrentDialog = itsOKToCloseDialog;
	WaitForResponse();

	return itsCloseAction;
}

/******************************************************************************
 AcceptLicense

	Display the license (asking for acceptance) and wait for a response.

 ******************************************************************************/

JBoolean
JXUserNotification::AcceptLicense()
{
	assert( itsCurrentDialog == NULL );

	(JXGetApplication())->PrepareForBlockingWindow();

	itsCurrentDialog = new JXAcceptLicenseDialog(JXGetApplication());
	assert( itsCurrentDialog != NULL );
	WaitForResponse();

	return itsWarningResponse;
}

/******************************************************************************
 Receive (protected)

	Listen for response from active dialog.

 ******************************************************************************/

void
JXUserNotification::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsCurrentDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		itsWarningResponse = info->Successful();
		itsCurrentDialog   = NULL;
		}

	else if (sender == itsOKToCloseDialog && message.Is(JXOKToCloseDialog::kGotResponse))
		{
		const JXOKToCloseDialog::GotResponse* response =
			dynamic_cast(const JXOKToCloseDialog::GotResponse*, &message);
		assert( response != NULL );
		itsCloseAction     = response->GetResponse();
		itsCurrentDialog   = NULL;
		itsOKToCloseDialog = NULL;
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
JXUserNotification::WaitForResponse()
{
	ListenTo(itsCurrentDialog);
	itsCurrentDialog->BeginDialog();

	// display the inactive cursor in all the other windows

	JXApplication* app = JXGetApplication();
	app->DisplayInactiveCursor();

	// block with event loop running until we get a response

	JXWindow* window = itsCurrentDialog->GetWindow();
	while (itsCurrentDialog != NULL)
		{
		app->HandleOneEventForWindow(window);
		}

	app->BlockingWindowFinished();
}

/******************************************************************************
 Private assert function

 ******************************************************************************/

int
__jXUNassert
	(
	char*	expr,
	char*	file,
	int		line
	)
{
	fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", expr, file, line);
	abort();
	return 0;	// so the compiler doesn't complain
}
