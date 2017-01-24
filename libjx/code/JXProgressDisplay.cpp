/******************************************************************************
 JXProgressDisplay.cpp

	Class to display the progress of a long process.  The client must call
	SetItems() with the appropriate widgets so we have something to draw to.

	We completely ignore the allowBackground flag in ProcessBeginning()
	becase the JX event loop is not reentrant.  This is not a problem if
	the process should not be backgrounded.  If one wants the process to
	run in the background, one should install an IdleTask and do work when
	the IdleTask gets time.

	When a background process is event driven, it is convenient to ignore
	the process until an event arrives.  In this case, the Cancel button
	will not work nicely if events are rare.  The CancelRequested message
	is broadcast when the process is backgrounded in order to avoid this
	problem.

	BASE CLASS = JProgressDisplay, virtual JBroadcaster

	Copyright (C) 1995 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JXProgressDisplay.h>
#include <JXPGMessageDirector.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXProgressIndicator.h>
#include <jXGlobals.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* JXProgressDisplay::kCancelRequested = "CancelRequested::JXProgressDisplay";

// Private class data

static JBoolean messageWindowLocInit = kJFalse;
static JPoint messageWindowLoc(-1,-1);

/******************************************************************************
 Constructor

 ******************************************************************************/

JXProgressDisplay::JXProgressDisplay()
	:
	JProgressDisplay()
{
	itsCancelFlag      = kJFalse;
	itsCancelButton    = NULL;
	itsCounter         = NULL;
	itsIndicator       = NULL;
	itsMessageDirector = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXProgressDisplay::~JXProgressDisplay()
{
}

/******************************************************************************
 SetItems

 ******************************************************************************/

void 
JXProgressDisplay::SetItems
	(
	JXTextButton*			cancelButton,
	JXStaticText*			counter,
	JXProgressIndicator*	indicator,
	JXTEBase*				label
	)
{
	if (itsCancelButton != NULL)
		{
		StopListening(itsCancelButton);
		}

	itsCancelButton = cancelButton;
	itsCounter      = counter;
	itsIndicator    = indicator;
	itsLabel        = label;

	if (itsCancelButton != NULL)
		{
		ListenTo(itsCancelButton);
		}
}

/******************************************************************************
 ProcessBeginning (protected)

 ******************************************************************************/

void
JXProgressDisplay::ProcessBeginning
	(
	const ProcessType	processType,
	const JSize			stepCount,
	const JCharacter*	message,
	const JBoolean		allowCancel,
	const JBoolean		allowBackground
	)
{
	itsCancelFlag = kJFalse;
	JProgressDisplay::ProcessBeginning(processType, stepCount, message,
									   allowCancel, allowBackground);

	if (!allowBackground)
		{
		DisplayBusyCursor();
		}

	if (itsLabel != NULL)
		{
		itsLabel->SetText(message);
		}

	assert( (processType == kFixedLengthProcess && itsIndicator != NULL) ||
			(processType == kVariableLengthProcess && itsCounter != NULL) );

	if (processType == kFixedLengthProcess)
		{
		itsIndicator->SetValue(0);
		itsIndicator->SetMaxValue(stepCount);
		itsIndicator->Show();
		if (itsCounter != NULL)
			{
			itsCounter->Hide();
			}
		}
	else if (processType == kVariableLengthProcess)
		{
		itsCounter->SetText(JString::empty);
		itsCounter->Show();
		if (itsIndicator != NULL)
			{
			itsIndicator->Hide();
			}
		}

	assert( !allowCancel || itsCancelButton != NULL );

	if (itsCancelButton != NULL)
		{
		itsCancelButton->SetVisible(allowCancel);
		}
}

/******************************************************************************
 IncrementProgress (virtual)

	Update the display to show that progress is being made.
	Returns kJFalse if process was cancelled by user.

	If the current process is of variable length and the message is not NULL,
	then it is displayed in the message window.

 ******************************************************************************/

JBoolean
JXProgressDisplay::IncrementProgress
	(
	const JCharacter* message
	)
{
	assert( ProcessRunning() );

	IncrementStepCount();
	return IncrementProgress1(message);
}

/******************************************************************************
 IncrementProgress (virtual)

	Update the display to show that progress is being made.
	Returns kJFalse if process was cancelled by user.

	The iteration count is incremented by the specified value.

 ******************************************************************************/

JBoolean
JXProgressDisplay::IncrementProgress
	(
	const JSize delta
	)
{
	assert( ProcessRunning() );

	IncrementStepCount(delta);
	return IncrementProgress1(NULL);
}

/******************************************************************************
 IncrementProgress (virtual)

	Update the display to show that progress is being made.
	Returns kJFalse if process was cancelled by user.

	If the current process is of variable length and the message is not NULL,
	then it is displayed in the message window.

 ******************************************************************************/

JBoolean
JXProgressDisplay::IncrementProgress
	(
	const JCharacter*	message,
	const JSize			delta
	)
{
	assert( ProcessRunning() );

	IncrementStepCount(delta);
	return IncrementProgress1(message);
}

/******************************************************************************
 IncrementProgress1 (private)

	Update the display to show that progress is being made.
	Returns kJFalse if process was cancelled by user.

	If the current process is of variable length and the message is not NULL,
	then it is displayed in the message window.

 ******************************************************************************/

JBoolean
JXProgressDisplay::IncrementProgress1
	(
	const JCharacter* message
	)
{
	const ProcessType process = GetCurrentProcessType();
	const JSize stepCount     = GetCurrentStepCount();
	if (process == kFixedLengthProcess)
		{
		itsIndicator->SetValue(stepCount);
		}
	else if (process == kVariableLengthProcess)
		{
		if (message != NULL)
			{
			AppendToMessageWindow(message);
			}
		const JString stepCountStr(stepCount, JString::kBase10);
		itsCounter->SetText(stepCountStr);
		itsCounter->Redraw();
		}
	else
		{
		std::cerr << "Unknown process type in JXProgressDisplay::IncrementProgress1()" << std::endl;
		}

	return ProcessContinuing();
}

/******************************************************************************
 ProcessContinuing (virtual)

	Save the current location of the message window.
	Returns kJFalse if process was cancelled by user.

 ******************************************************************************/

JBoolean
JXProgressDisplay::ProcessContinuing()
{
	if (itsMessageDirector != NULL)
		{
		messageWindowLocInit = kJTrue;
		messageWindowLoc =
			(itsMessageDirector->GetWindow())->GetDesktopLocation();
		}

	return JProgressDisplay::ProcessContinuing();
}

/******************************************************************************
 CheckForCancel (virtual protected)

 ******************************************************************************/

JBoolean
JXProgressDisplay::CheckForCancel()
{
	return itsCancelFlag;
}

/******************************************************************************
 ProcessFinished (virtual)

 ******************************************************************************/

void
JXProgressDisplay::ProcessFinished()
{
	JProgressDisplay::ProcessFinished();

	if (itsMessageDirector != NULL)
		{
		messageWindowLocInit = kJTrue;
		messageWindowLoc =
			(itsMessageDirector->GetWindow())->GetDesktopLocation();

		itsMessageDirector->ProcessFinished();
		itsMessageDirector = NULL;
		}
}

/******************************************************************************
 DisplayBusyCursor (virtual)

 ******************************************************************************/

void
JXProgressDisplay::DisplayBusyCursor()
{
	(JXGetApplication())->DisplayBusyCursor();
}

/******************************************************************************
 AppendToMessageWindow (virtual protected)

 ******************************************************************************/

void
JXProgressDisplay::AppendToMessageWindow
	(
	const JCharacter* message
	)
{
	if (itsMessageDirector == NULL)
		{
		itsMessageDirector = jnew JXPGMessageDirector(JXGetApplication());
		assert(itsMessageDirector != NULL);

		if (messageWindowLocInit)
			{
			(itsMessageDirector->GetWindow())->
				Place(messageWindowLoc.x, messageWindowLoc.y);
			}

		itsMessageDirector->Activate();
		}

	itsMessageDirector->AddMessageLine(message);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXProgressDisplay::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsCancelButton && message.Is(JXButton::kPushed))
		{
		const JXButton::Pushed* info =
			dynamic_cast<const JXButton::Pushed*>(&message);
		assert( info != NULL );
		itsCancelFlag = kJTrue;

		// must be last since it could delete us

		if (AllowBackground())
			{
			Broadcast(CancelRequested());
			}
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}
