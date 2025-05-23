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
#include "JXUrgentFunctionTask.h"
#include "jXGlobals.h"
#include <stdio.h>
#include <stdlib.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUserNotification::JXUserNotification()
	:
	JUserNotification()
{
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
	if (!IsSilent())
	{
		auto* dlog = jnew JXMessageDialog(message);
		dlog->DoDialog();
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
	if (IsSilent())
	{
		return;
	}

	if (JXApplication::IsWorkerFiber())
	{
		auto* dlog = jnew JXErrorDialog(message);
		dlog->DoDialog();
	}
	else	// during startup or in thread
	{
		auto* task = jnew JXUrgentFunctionTask(JXGetApplication(), [message]()
		{
			auto* dlog = jnew JXErrorDialog(message);
			dlog->DoDialog();
		},
		"JXUserNotification::ReportError");
		task->Go();
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
	auto* dlog = jnew JXWarningDialog(JGetString("QuestionWindowTitle::JXWarningDialog"), message);
	return dlog->DoDialog();
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
	auto* dlog = jnew JXWarningDialog(JGetString("WarningWindowTitle::JXWarningDialog"), message);
	return dlog->DoDialog();
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
	auto* dlog = jnew JXOKToCloseDialog(message);

	dlog->DoDialog();
	return dlog->GetCloseAction();
}

/******************************************************************************
 AcceptLicense

	Display the license and ask for acceptance.

 ******************************************************************************/

bool
JXUserNotification::AcceptLicense()
{
	auto* dlog = jnew JXAcceptLicenseDialog();

	return dlog->DoDialog();
}
