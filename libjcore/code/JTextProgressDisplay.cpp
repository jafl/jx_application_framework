/******************************************************************************
 JTextProgressDisplay.cpp

	Class for displaying the progress of a long process on the console.

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#include "JTextProgressDisplay.h"
#include "JString.h"
#include "jMath.h"
#include "jAssert.h"

// Interrupt routine

volatile sig_atomic_t cancelFlag;

static void cancelHandler(int sig)
{
	signal(SIGINT, cancelHandler);
	cancelFlag = 1;
}

// we use this as a placeholder for "nothing"

static void emptyHandler(int sig)
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
	if (itsOldSigIntHandler != &emptyHandler)
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
	const JString&		message,
	const bool			allowCancel,
	const bool			modal
	)
{
	JProgressDisplay::ProcessBeginning(processType, stepCount, message,
									   allowCancel, modal);

	std::cout << std::endl << std::endl << message << std::endl;
	if (allowCancel)
	{
		#ifdef _J_UNIX
			std::cout << "(Press ctrl-C to cancel)";
		#else
			figure out what to say!
		#endif
	}
	std::cout << std::endl << std::endl;

	if (processType == kFixedLengthProcess)
	{
		std::cout << "0%" << std::endl;
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
	Returns false if process was cancelled by user.

	If the current process is of variable length and the message is not nullptr,
	then it is displayed in place of the usual iteration count.

 ******************************************************************************/

bool
JTextProgressDisplay::IncrementProgress
	(
	const JString&	message,
	const JSize		delta
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
		std::cout << JRound(progressFraction) << '%' << std::endl;
	}
	else if (process == kVariableLengthProcess)
	{
		if (!message.IsEmpty())
		{
			message.Print(std::cout);
			std::cout << std::endl;
		}
		else
		{
			std::cout << stepCount << std::endl;
		}
	}
	else
	{
		std::cerr << "Unknown process type in JTextProgressDisplay::IncrementProgress()" << std::endl;
	}

	return ProcessContinuing();
}

/******************************************************************************
 ProcessFinished

 ******************************************************************************/

void
JTextProgressDisplay::ProcessFinished()
{
	JProgressDisplay::ProcessFinished();

	std::cout << std::endl;

	// put back the original cancel handler

	if (itsOldSigIntHandler != &emptyHandler)
	{
		j_sig_func currentSigIntHandler = signal(SIGINT, itsOldSigIntHandler);
		assert( currentSigIntHandler == cancelHandler );
		itsOldSigIntHandler = emptyHandler;
	}
}

/******************************************************************************
 CheckForCancel

 ******************************************************************************/

bool
JTextProgressDisplay::CheckForCancel()
{
	if (cancelFlag)
	{
		cancelFlag = 0;
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 DisplayBusyCursor (virtual)

 ******************************************************************************/

void
JTextProgressDisplay::DisplayBusyCursor()
{
}
