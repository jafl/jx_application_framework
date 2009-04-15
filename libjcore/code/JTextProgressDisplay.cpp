/******************************************************************************
 JTextProgressDisplay.cpp

	Class for displaying the progress of a long process on the console.

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTextProgressDisplay.h>
#include <jMath.h>
#include <jAssert.h>

// Interrupt routine

volatile sig_atomic_t cancelFlag;

// prototypes

static void cancelHandler(int sig);
static void emptyHandler(int sig);

// functions

void cancelHandler(int sig)
{
	signal(SIGINT, cancelHandler);
	cancelFlag = 1;
}

// we use this as a placeholder for "nothing"

void emptyHandler(int sig)
{
}

/******************************************************************************
 Constructor

 ******************************************************************************/

JTextProgressDisplay::JTextProgressDisplay()
	:
	JProgressDisplay()
{
	itsOldSigIntHandler = emptyHandler;
}

/******************************************************************************
 Destructor

	Make sure we restore the original signal handler.

 ******************************************************************************/

JTextProgressDisplay::~JTextProgressDisplay()
{
	if (itsOldSigIntHandler != emptyHandler)
		{
		signal(SIGINT, itsOldSigIntHandler);
		}
}

/******************************************************************************
 ProcessBeginning (protected)

	Display the message.

 ******************************************************************************/

void
JTextProgressDisplay::ProcessBeginning
	(
	const ProcessType	processType,
	const JSize			stepCount,
	const JCharacter*	message,
	const JBoolean		allowCancel,
	const JBoolean		allowBackground
	)
{
	JProgressDisplay::ProcessBeginning(processType, stepCount, message,
									   allowCancel, allowBackground);

	cout << endl << endl << message << endl;
	if (allowCancel)
		{
		#if defined _J_UNIX || defined _MSC_VER
			cout << "(Press ctrl-C to cancel)";
		#else
			figure out what to say!
		#endif
		}
	cout << endl << endl;

	if (processType == kFixedLengthProcess)
		{
		cout << "0%" << endl;
		}

	// install the cancel handler

	if (allowCancel)
		{
		itsOldSigIntHandler = signal(SIGINT, cancelHandler);
		cancelFlag = 0;
		}
}

/******************************************************************************
 IncrementProgress

	Update the display to show that progress is being made.
	Returns kJFalse if process was cancelled by user.

	If the current process is of variable length and the message is not NULL,
	then it is displayed in place of the usual iteration count.

 ******************************************************************************/

JBoolean
JTextProgressDisplay::IncrementProgress
	(
	const JCharacter*	message,
	const JSize			delta
	)
{
	const ProcessType process = GetCurrentProcessType();
	assert( process != kNoRunningProcess );

	IncrementStepCount(delta);
	const JSize stepCount = GetCurrentStepCount();
	const JSize maxCount  = GetMaxStepCount();

	if (process == kFixedLengthProcess)
		{
		const double progressFraction =
			100.0 * ((double) stepCount) / ((double) maxCount);
		cout << JRound(progressFraction) << '%' << endl;
		}
	else if (process == kVariableLengthProcess)
		{
		if (message != NULL)
			{
			cout << message << endl;
			}
		else
			{
			cout << stepCount << endl;
			}
		}
	else
		{
		cerr << "Unknown process type in JTextProgressDisplay::IncrementProgress()" << endl;
		}

	return ProcessContinuing();
}

/******************************************************************************
 IncrementProgress

	Update the display to show that progress is being made.
	Returns kJFalse if process was cancelled by user.

	If the current process is of variable length and the message is not NULL,
	then it is displayed in place of the usual iteration count.

 ******************************************************************************/

JBoolean
JTextProgressDisplay::IncrementProgress
	(
	const JCharacter* message
	)
{
	return IncrementProgress(message, 1);
}

/******************************************************************************
 IncrementProgress

	The iteration count is incremented by the specified value.

	Update the display to show that progress is being made.
	Returns kJFalse if process was cancelled by user.

 ******************************************************************************/

JBoolean
JTextProgressDisplay::IncrementProgress
	(
	const JSize delta
	)
{
	return IncrementProgress(NULL, delta);
}

/******************************************************************************
 ProcessFinished

 ******************************************************************************/

void
JTextProgressDisplay::ProcessFinished()
{
	JProgressDisplay::ProcessFinished();

	cout << endl;

	// put back the original cancel handler

	if (itsOldSigIntHandler != emptyHandler)
		{
		j_sig_func* currentSigIntHandler = signal(SIGINT, itsOldSigIntHandler);
		assert( currentSigIntHandler == cancelHandler );
		itsOldSigIntHandler = emptyHandler;
		}
}

/******************************************************************************
 CheckForCancel

 ******************************************************************************/

JBoolean
JTextProgressDisplay::CheckForCancel()
{
	if (cancelFlag)
		{
		cancelFlag = 0;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 DisplayBusyCursor (virtual)

 ******************************************************************************/

void
JTextProgressDisplay::DisplayBusyCursor()
{
}
