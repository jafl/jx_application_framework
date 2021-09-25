/******************************************************************************
 JXUserNotification.cpp

	Class for notifying the user of something via the JX Library.
	Since jAssert probably uses us to display the error, we can't use
	jAssert or we might generate an infinite loop.

	*** Do not use this for displaying failed assertion messages.
		This will allow the event handler to continue operating, which is
		usually fatal after an assert fires.

	BASE CLASS = JUserNotification, virtual JBroadcaster

	Copyright (C) 1995-97 by John Lindal.

 ******************************************************************************/

#include "JXUserNotification.h"
#include "JXMessageDialog.h"
#include "JXErrorDialog.h"
#include "JXWarningDialog.h"
#include "JXOKToCloseDialog.h"
#include "JXAcceptLicenseDialog.h"
#include "JXWindow.h"
#include "jXGlobals.h"
#include <JString.h>
#include <stdio.h>
#include <stdlib.h>
#include <jAssert.h>

#undef assert

static int __jXUNassert(const char*, const char*, int);

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
	itsCurrentDialog   = nullptr;
	itsOKToCloseDialog = nullptr;
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
	const JString& message
	)
{
	assert( itsCurrentDialog == nullptr );

	if (!IsSilent())
	{
		JXGetApplication()->PrepareForBlockingWindow();

		itsCurrentDialog = jnew JXMessageDialog(JXGetApplication(), message);
		assert( itsCurrentDialog != nullptr );
		WaitForResponse();
	}
}

/******************************************************************************
 ReportError

 ******************************************************************************/

void
JXUserNotification::ReportError
	(
	const JString& message
	)
{
	assert( itsCurrentDialog == nullptr );

	if (!IsSilent())
	{
		JXGetApplication()->PrepareForBlockingWindow();

		itsCurrentDialog = jnew JXErrorDialog(JXGetApplication(), message);
		assert( itsCurrentDialog != nullptr );
		WaitForResponse();
	}
}

/******************************************************************************
 AskUserYes

	Display the message (asking for a y/n answer) and wait for a response.

 ******************************************************************************/

bool
JXUserNotification::AskUserYes
	(
	const JString& message
	)
{
	assert( itsCurrentDialog == nullptr );

	JXGetApplication()->PrepareForBlockingWindow();

	itsCurrentDialog = jnew JXWarningDialog(
		JXGetApplication(), JGetString("QuestionWindowTitle::JXWarningDialog"), message);
	assert( itsCurrentDialog != nullptr );
	WaitForResponse();

	return itsWarningResponse;
}

/******************************************************************************
 AskUserNo

	Display the message (asking for a y/n answer) and wait for a response.

 ******************************************************************************/

bool
JXUserNotification::AskUserNo
	(
	const JString& message
	)
{
	assert( itsCurrentDialog == nullptr );

	JXGetApplication()->PrepareForBlockingWindow();

	itsCurrentDialog = jnew JXWarningDialog(
		JXGetApplication(), JGetString("WarningWindowTitle::JXWarningDialog"), message);
	assert( itsCurrentDialog != nullptr );
	WaitForResponse();

	return itsWarningResponse;
}

/******************************************************************************
 OKToClose

 ******************************************************************************/

JUserNotification::CloseAction
JXUserNotification::OKToClose
	(
	const JString& message
	)
{
	assert( itsCurrentDialog == nullptr && itsOKToCloseDialog == nullptr );

	JXGetApplication()->PrepareForBlockingWindow();

	itsOKToCloseDialog = jnew JXOKToCloseDialog(JXGetApplication(), message);
	assert( itsOKToCloseDialog != nullptr );

	itsCurrentDialog = itsOKToCloseDialog;
	WaitForResponse();

	return itsCloseAction;
}

/******************************************************************************
 AcceptLicense

	Display the license (asking for acceptance) and wait for a response.

 ******************************************************************************/

bool
JXUserNotification::AcceptLicense()
{
	assert( itsCurrentDialog == nullptr );

	JXGetApplication()->PrepareForBlockingWindow();

	itsCurrentDialog = jnew JXAcceptLicenseDialog(JXGetApplication());
	assert( itsCurrentDialog != nullptr );
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
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		itsWarningResponse = info->Successful();
		itsCurrentDialog   = nullptr;
	}

	else if (sender == itsOKToCloseDialog && message.Is(JXOKToCloseDialog::kGotResponse))
	{
		const auto* response =
			dynamic_cast<const JXOKToCloseDialog::GotResponse*>(&message);
		assert( response != nullptr );
		itsCloseAction     = response->GetResponse();
		itsCurrentDialog   = nullptr;
		itsOKToCloseDialog = nullptr;
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
	while (itsCurrentDialog != nullptr)
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
	const char*	expr,
	const char*	file,
	int			line
	)
{
	fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", expr, file, line);
	abort();
	return 0;	// so the compiler doesn't complain
}
