/******************************************************************************
 JThreadPG.cpp

	Encapsulates the link back to the main thread which displays the progress.

	We used a cancelFlag because std::jthread is not always available.

	Copyright © 2007-2024 by John Lindal.

 ******************************************************************************/

#include "JThreadPG.h"
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* JThreadPG::kProcessBeginning  = "ProcessBeginning::JThreadPG";
const JUtf8Byte* JThreadPG::kIncrementProgress = "IncrementProgress::JThreadPG";
const JUtf8Byte* JThreadPG::kProcessFinished   = "ProcessFinished::JThreadPG";

/******************************************************************************
 Constructor

 ******************************************************************************/

JThreadPG::JThreadPG
	(
	boost::fibers::buffered_channel<JBroadcaster::Message*>* channel,

	std::atomic_bool*	cancelFlag,
	const JSize			scaleFactor
	)
	:
	JProgressDisplay(),
	itsChannel(channel),
	itsCancelFlag(cancelFlag),
	itsScaleFactor(scaleFactor),
	itsCounter(0)
{
}

/******************************************************************************
 Destructor

	Make sure we restore the original signal handler.

 ******************************************************************************/

JThreadPG::~JThreadPG()
{
}

/******************************************************************************
 WaitForProcessFinished

	This must run in the main thread.

 ******************************************************************************/

void
JThreadPG::WaitForProcessFinished
	(
	JProgressDisplay* pg
	)
{
	JBroadcaster::Message* m;
	while (itsChannel->pop(m) == boost::fibers::channel_op_status::success)
	{
		if (m->Is(kProcessBeginning))
		{
			auto& info = dynamic_cast<class ProcessBeginning&>(*m);

			const JString* msg;
			if (!info.GetMessage(&msg))
			{
				msg = &JString::empty;
			}

			if (info.GetProcessType() == kFixedLengthProcess)
			{
				pg->FixedLengthProcessBeginning(info.GetStepCount(), *msg,
												info.AllowCancel(), false);
			}
			else
			{
				assert( info.GetProcessType() == kVariableLengthProcess );

				pg->VariableLengthProcessBeginning(*msg, info.AllowCancel(), false);
			}
		}
		else if (m->Is(kIncrementProgress))
		{
			auto& info = dynamic_cast<class IncrementProgress&>(*m);

			const JString* msg;
			if (!info.GetMessage(&msg))
			{
				msg = &JString::empty;
			}

			if (!pg->IncrementProgress(*msg, info.GetDelta()) &&
				itsCancelFlag != nullptr)
			{
				*itsCancelFlag = true;
			}
		}
		else if (m->Is(kProcessFinished))
		{
			pg->ProcessFinished();
			jdelete m;
			break;
		}

		jdelete m;
	}
}

/******************************************************************************
 ProcessBeginning (protected)

	Display the message.

 ******************************************************************************/

void
JThreadPG::ProcessBeginning
	(
	const ProcessType	processType,
	const JSize			stepCount,
	const JString&		message,
	const bool			allowCancel,
	const bool			modal
	)
{
	assert( !allowCancel || itsCancelFlag != nullptr );
	assert( !modal );

	itsCounter = 0;

	JProgressDisplay::ProcessBeginning(processType, stepCount, message,
									   allowCancel, modal);

	itsChannel->push(
		jnew class ProcessBeginning(
			processType, stepCount, message, allowCancel));
}

/******************************************************************************
 IncrementProgress

	Update the display to show that progress is being made.
	Returns false if process was cancelled by user.

 ******************************************************************************/

bool
JThreadPG::IncrementProgress
	(
	const JString&	message,
	const JSize		delta
	)
{
	IncrementStepCount(delta);

	itsCounter += delta;
	if (itsCounter >= itsScaleFactor)
	{
		itsChannel->push(jnew class IncrementProgress(message, itsCounter));
		itsCounter = 0;
	}

	return ProcessContinuing();
}

/******************************************************************************
 ProcessFinished

 ******************************************************************************/

void
JThreadPG::ProcessFinished()
{
	itsChannel->push(jnew class ProcessFinished);

	JProgressDisplay::ProcessFinished();
}

/******************************************************************************
 CheckForCancel

 ******************************************************************************/

bool
JThreadPG::CheckForCancel()
{
	return itsCancelFlag != nullptr && *itsCancelFlag;
}

/******************************************************************************
 DisplayBusyCursor (virtual)

 ******************************************************************************/

void
JThreadPG::DisplayBusyCursor()
{
}

/******************************************************************************
 JThreadPG messages

 ******************************************************************************/

JThreadPG::ProcessBeginning::ProcessBeginning
	(
	const ProcessType	processType,
	const JSize			stepCount,
	const JString&		message,
	const bool			allowCancel
	)
	:
	JBroadcaster::Message(JThreadPG::kProcessBeginning),
	itsProcessType(processType),
	itsStepCount(stepCount),
	itsMessage(nullptr),
	itsAllowCancelFlag(allowCancel)
{
	if (!message.IsEmpty())
	{
		itsMessage = jnew JString(message);
	}
}

JThreadPG::ProcessBeginning::~ProcessBeginning()
{
	jdelete itsMessage;
}

JThreadPG::IncrementProgress::IncrementProgress
	(
	const JString&	message,
	const JSize		delta
	)
	:
	JBroadcaster::Message(JThreadPG::kIncrementProgress),
	itsMessage(nullptr),
	itsDelta(delta)
{
	if (!message.IsEmpty())
	{
		itsMessage = jnew JString(message);
	}
}

JThreadPG::IncrementProgress::~IncrementProgress()
{
	jdelete itsMessage;
}
