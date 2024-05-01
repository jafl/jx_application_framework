/******************************************************************************
 JXProgressDisplay.cpp

	Class to display the progress of a long process.  The client must call
	SetItems() with the appropriate widgets so we have something to draw to.

	When a background process is event driven, it is convenient to ignore
	the process until an event arrives.  In this case, the Cancel button
	will not work nicely if events are rare.  The CancelRequested message
	is broadcast when the process is backgrounded in order to avoid this
	problem.

	BASE CLASS = JProgressDisplay, virtual JBroadcaster

	Copyright (C) 1995 by Glenn W. Bach.
	Copyright (C) 1997-2024 by John Lindal.

 ******************************************************************************/

#include "JXProgressDisplay.h"
#include "JXPGMessageDirector.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXStaticText.h"
#include "JXProgressIndicator.h"
#include "JXFunctionTask.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* JXProgressDisplay::kCancelRequested = "CancelRequested::JXProgressDisplay";

// Private class data

static bool messageWindowLocInit = false;
static JPoint messageWindowLoc(-1,-1);

/******************************************************************************
 Constructor

 ******************************************************************************/

JXProgressDisplay::JXProgressDisplay()
	:
	JProgressDisplay(),
	itsCancelFlag(false),
	itsCancelButton(nullptr),
	itsCounter(nullptr),
	itsIndicator(nullptr),
	itsLabel(nullptr),
	itsMessageDirector(nullptr),
	itsContinueTask(nullptr),
	itsContinueFlag(false)
{
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
	if (itsCancelButton != nullptr)
	{
		StopListening(itsCancelButton);
	}

	itsCancelButton = cancelButton;
	itsCounter      = counter;
	itsIndicator    = indicator;
	itsLabel        = label;

	if (itsCancelButton != nullptr)
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
	const JString&		message,
	const bool			allowCancel,
	const bool			modal
	)
{
	itsCancelFlag = false;
	JProgressDisplay::ProcessBeginning(processType, stepCount, message,
									   allowCancel, modal);

	if (itsLabel != nullptr)
	{
		itsLabel->GetText()->SetText(message);
	}

	assert( (processType == kFixedLengthProcess && itsIndicator != nullptr) ||
			(processType == kVariableLengthProcess && itsCounter != nullptr) );

	if (processType == kFixedLengthProcess)
	{
		itsIndicator->SetValue(0);
		itsIndicator->SetMaxValue(stepCount);
		itsIndicator->Show();
		if (itsCounter != nullptr)
		{
			itsCounter->Hide();
		}
	}
	else if (processType == kVariableLengthProcess)
	{
		itsCounter->GetText()->SetText(JString::empty);
		itsCounter->Show();
		if (itsIndicator != nullptr)
		{
			itsIndicator->Hide();
		}
	}

	assert( !allowCancel || itsCancelButton != nullptr );

	if (itsCancelButton != nullptr)
	{
		itsCancelButton->SetVisible(allowCancel);
	}

	// set up the idle task to run the event loop

	if (modal)
	{
		itsContinueTask = jnew JXFunctionTask(0, [this]()
		{
			std::unique_lock lock(itsMutex);
			itsContinueFlag = true;
			lock.unlock();

			itsCondition.notify_one();
			boost::this_fiber::yield();
		});
		itsContinueTask->Start();
	}
}

/******************************************************************************
 IncrementProgress (virtual)

	Update the display to show that progress is being made.
	Returns false if process was cancelled by user.

	If the current process is of variable length and the message is not nullptr,
	then it is displayed in the message window.

 ******************************************************************************/

bool
JXProgressDisplay::IncrementProgress
	(
	const JString&	message,
	const JSize		delta
	)
{
	assert( ProcessRunning() );

	IncrementStepCount(delta);
	return CalledByIncrementProgress(message);
}

/******************************************************************************
 CalledByIncrementProgress (private)

	Update the display to show that progress is being made.
	Returns false if process was cancelled by user.

	If the current process is of variable length and the message is not nullptr,
	then it is displayed in the message window.

 ******************************************************************************/

bool
JXProgressDisplay::CalledByIncrementProgress
	(
	const JString& message
	)
{
	const ProcessType process = GetCurrentProcessType();
	const JSize stepCount     = GetCurrentStepCount();
	if (process == kFixedLengthProcess)
	{
		itsIndicator->SetValue(stepCount);
		itsIndicator->Redraw();
	}
	else if (process == kVariableLengthProcess)
	{
		if (!message.IsEmpty())
		{
			AppendToMessageWindow(message);
		}
		const JString stepCountStr(stepCount);
		itsCounter->GetText()->SetText(stepCountStr);
		itsCounter->Redraw();
	}
	else
	{
		std::cerr << "Unknown process type in JXProgressDisplay::CalledByIncrementProgress()" << std::endl;
	}

	return ProcessContinuing();
}

/******************************************************************************
 ProcessContinuing (virtual)

	Save the current location of the message window.
	Returns false if process was cancelled by user.

 ******************************************************************************/

bool
JXProgressDisplay::ProcessContinuing()
{
	if (itsMessageDirector != nullptr)
	{
		messageWindowLocInit = true;
		messageWindowLoc     = itsMessageDirector->GetWindow()->GetDesktopLocation();
	}

	if (!JProgressDisplay::ProcessContinuing())
	{
		return false;
	}

	if (IsModal())
	{
		assert( JXApplication::IsWorkerFiber() );

		std::unique_lock lock(itsMutex);
		itsCondition.wait(lock, [this](){ return itsContinueFlag; });
		itsContinueFlag = false;
	}
	return true;
}

/******************************************************************************
 CheckForCancel (virtual protected)

 ******************************************************************************/

bool
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
	if (itsMessageDirector != nullptr)
	{
		messageWindowLocInit = true;
		messageWindowLoc =
			itsMessageDirector->GetWindow()->GetDesktopLocation();

		itsMessageDirector->ProcessFinished();
		itsMessageDirector = nullptr;
	}

	if (itsContinueTask != nullptr)
	{
		itsContinueTask->Stop();
		jdelete itsContinueTask;
		itsContinueTask = nullptr;
	}

	JProgressDisplay::ProcessFinished();
}

/******************************************************************************
 DisplayBusyCursor (virtual)

 ******************************************************************************/

void
JXProgressDisplay::DisplayBusyCursor()
{
	JXGetApplication()->DisplayBusyCursor();
}

/******************************************************************************
 AppendToMessageWindow (virtual protected)

 ******************************************************************************/

void
JXProgressDisplay::AppendToMessageWindow
	(
	const JString& message
	)
{
	if (itsMessageDirector == nullptr)
	{
		itsMessageDirector = jnew JXPGMessageDirector(JXGetApplication());
		if (messageWindowLocInit)
		{
			itsMessageDirector->GetWindow()->
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
		auto* info = dynamic_cast<const JXButton::Pushed*>(&message);
		assert( info != nullptr );
		itsCancelFlag = true;

		// must be last since it could delete us

		if (!IsModal())
		{
			Broadcast(CancelRequested());
		}
	}
	else
	{
		JBroadcaster::Receive(sender, message);
	}
}
