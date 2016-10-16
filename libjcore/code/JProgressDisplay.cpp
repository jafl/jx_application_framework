/******************************************************************************
 JProgressDisplay.cpp

	Abstract base class for displaying the progress of a long process.

	It provides the framework for letting the user:
		1)  cancel the process
		2)  continue doing other things while the process is running
			(this is only relevant on systems with cooperative multitasking)

	It can handle both fixed length and variable length processes.  In both
	cases, you only have to keep calling IncrementProgress().  (For a fixed
	length process, you must also specify how many steps there will be.)

	ProcessContinuing should be used when the time between IncrementProgress()
	calls is longer than about 1/2 second.  By calling ProcessContinuing at
	various points in your loop, you give other processes (like the foreground
	process) a larger share of the time.  (This is obviously not of much
	concern if your process is neither backgroundable nor cancellable.)

	Even if a process is cancelled, you must call ProcessFinished.
	This forces you to write a cleaner loop.

	Derived classes must override:

		ProcessBeginning
			Create some sort of display to represent the progress.
			Remember to call JProgressDisplay::ProcessBeginning().

		IncrementProgress(const JUtf8Byte* message)
			Increment the step count by 1 and update the display to show that
			progress is being made.  If the message is not NULL, it should
			be displayed.  Return kJFalse if process was cancelled by user.

		IncrementProgress(const JSize delta)
			Increment the step count by delta and update the display to show
			that progress is being made.  Return kJFalse if process was
			cancelled by user.

		ProcessFinished
			Clean up the display that was created.
			Remember to call JProgressDisplay::ProcessFinished().

		CheckForCancel
			Return kJTrue if the user wishes to cancel the operation.

		DisplayBusyCursor
			Change the mouse cursor to indicate that the program is busy.
			(can be used if process is not in the background)

	They should also override:

		ProcessContinuing
			Give the OS some background time if AllowBackground() and the
			system is cooperatively multitasked.  Do not increment the step
			count or update the display.  Return the result of
			JProgressDisplay::ProcessContinuing().

	BASE CLASS = none

	Copyright (C) 1994-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JProgressDisplay.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JProgressDisplay::JProgressDisplay()
{
	itsCurrentProcess = kNoRunningProcess;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JProgressDisplay::~JProgressDisplay()
{
	assert( itsCurrentProcess == kNoRunningProcess );
}

/******************************************************************************
 ProcessBeginning (virtual protected)

	Initialize the data members.  Derived classes should create some sort of
	display to represent the progress.

 ******************************************************************************/

void
JProgressDisplay::ProcessBeginning
	(
	const ProcessType	processType,
	const JSize			stepCount,
	const JUtf8Byte*	message,
	const JBoolean		allowCancel,
	const JBoolean		allowBackground
	)
{
	assert( itsCurrentProcess == kNoRunningProcess );

	itsCurrentProcess      = processType;
	itsAllowCancelFlag     = allowCancel;
	itsAllowBackgroundFlag = allowBackground;

	itsMaxStepCount     = stepCount;
	itsCurrentStepCount = 0;
}

/******************************************************************************
 ProcessContinuing (virtual)

	Derived classes should give the system some background time (system
	dependent) without updating the progress display.

	Returns kJFalse if process was cancelled by user.

 ******************************************************************************/

JBoolean
JProgressDisplay::ProcessContinuing()
{
	assert( itsCurrentProcess != kNoRunningProcess );

	if (itsAllowCancelFlag)
		{
		return !CheckForCancel();
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 ProcessFinished (virtual)

	Derived classes must call this and then clean up the display they created.

 ******************************************************************************/

void
JProgressDisplay::ProcessFinished()
{
	assert( itsCurrentProcess != kNoRunningProcess );

	itsCurrentProcess = kNoRunningProcess;
}

/******************************************************************************
 IncrementStepCount (protected)

 ******************************************************************************/

void
JProgressDisplay::IncrementStepCount
	(
	const JSize delta
	)
{
	itsCurrentStepCount += delta;
	assert( itsCurrentProcess == kVariableLengthProcess ||
			itsCurrentStepCount <= itsMaxStepCount );
}
