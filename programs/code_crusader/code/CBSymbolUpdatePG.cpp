/******************************************************************************
 CBSymbolUpdatePG.cpp

	Encapsulates the link back to the parent process which displays the progress.

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#include "CBSymbolUpdatePG.h"
#include "CBProjectDocument.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSymbolUpdatePG::CBSymbolUpdatePG
	(
	std::ostream&	link,
	const JSize	scaleFactor
	)
	:
	JProgressDisplay(),
	itsLink(link),
	itsScaleFactor(scaleFactor)
{
}

/******************************************************************************
 Destructor

	Make sure we restore the original signal handler.

 ******************************************************************************/

CBSymbolUpdatePG::~CBSymbolUpdatePG()
{
}

/******************************************************************************
 ProcessBeginning (protected)

	Display the message.

 ******************************************************************************/

void
CBSymbolUpdatePG::ProcessBeginning
	(
	const ProcessType	processType,
	const JSize			stepCount,
	const JString&		message,
	const bool		allowCancel,
	const bool		allowBackground
	)
{
	JProgressDisplay::ProcessBeginning(processType, stepCount, message,
									   allowCancel, allowBackground);

	if (processType == kVariableLengthProcess)
		{
		itsLink << CBProjectDocument::kVariableLengthStart;
		}
	else
		{
		assert( processType == kFixedLengthProcess );
		itsLink << CBProjectDocument::kFixedLengthStart;
		itsLink << ' ' << stepCount;
		}

	itsLink << ' ' << JString(message) << std::endl;
}

/******************************************************************************
 IncrementProgress

	Update the display to show that progress is being made.
	Returns false if process was cancelled by user.

 ******************************************************************************/

bool
CBSymbolUpdatePG::IncrementProgress
	(
	const JString&	message,
	const JSize		delta
	)
{
	IncrementStepCount(delta);
	const JSize stepCount = GetCurrentStepCount();

	if (stepCount % itsScaleFactor == 0)
		{
		itsLink << CBProjectDocument::kProgressIncrement << ' ' << itsScaleFactor << std::endl;
		}

	return ProcessContinuing();
}

/******************************************************************************
 IncrementProgress

	Update the display to show that progress is being made.
	Returns false if process was cancelled by user.

 ******************************************************************************/

bool
CBSymbolUpdatePG::IncrementProgress
	(
	const JString& message
	)
{
	return IncrementProgress(message, 1);
}

/******************************************************************************
 IncrementProgress

	The iteration count is incremented by the specified value.

	Update the display to show that progress is being made.
	Returns false if process was cancelled by user.

 ******************************************************************************/

bool
CBSymbolUpdatePG::IncrementProgress
	(
	const JSize delta
	)
{
	return IncrementProgress(JString::empty, delta);
}

/******************************************************************************
 ProcessFinished

 ******************************************************************************/

void
CBSymbolUpdatePG::ProcessFinished()
{
	if (GetCurrentProcessType() == kFixedLengthProcess)
		{
		const JSize delta = GetCurrentStepCount() % itsScaleFactor;
		if (delta > 0)
			{
			itsLink << CBProjectDocument::kProgressIncrement << ' ' << delta << std::endl;
			}
		}

	JProgressDisplay::ProcessFinished();
}

/******************************************************************************
 CheckForCancel

 ******************************************************************************/

bool
CBSymbolUpdatePG::CheckForCancel()
{
	return false;
}

/******************************************************************************
 DisplayBusyCursor (virtual)

 ******************************************************************************/

void
CBSymbolUpdatePG::DisplayBusyCursor()
{
}
