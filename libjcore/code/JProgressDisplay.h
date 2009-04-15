/******************************************************************************
 JProgressDisplay.h

	Interface for the JProgressDisplay class.

	Copyright © 1994-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JProgressDisplay
#define _H_JProgressDisplay

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

class JProgressDisplay
{
public:

	enum ProcessType
	{
		kNoRunningProcess,
		kFixedLengthProcess,
		kVariableLengthProcess
	};

public:

	JProgressDisplay();

	virtual ~JProgressDisplay();

	void	FixedLengthProcessBeginning(const JSize stepCount, const JCharacter* message,
										const JBoolean allowCancel,
										const JBoolean allowBackground);
	void	VariableLengthProcessBeginning(const JCharacter* message,
										   const JBoolean allowCancel,
										   const JBoolean allowBackground);

	virtual JBoolean	IncrementProgress(const JCharacter* message = NULL) = 0;
	virtual JBoolean	IncrementProgress(const JSize delta) = 0;
	virtual JBoolean	IncrementProgress(const JCharacter* message,
										  const JSize delta) = 0;
	virtual JBoolean	ProcessContinuing();
	virtual void		ProcessFinished();

	JBoolean	ProcessRunning() const;
	ProcessType	GetCurrentProcessType() const;
	JSize		GetCurrentStepCount() const;

	// called automatically when not in background

	virtual void	DisplayBusyCursor() = 0;

protected:

	virtual void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
									 const JCharacter* message, const JBoolean allowCancel,
									 const JBoolean allowBackground);

	virtual JBoolean	CheckForCancel() = 0;
	JBoolean			AllowCancel() const;
	JBoolean			AllowBackground() const;

	JSize	GetMaxStepCount() const;			// only for fixed length processes
	void	IncrementStepCount(const JSize delta = 1);

private:

	ProcessType	itsCurrentProcess;		// type of process currently running
	JBoolean	itsAllowCancelFlag;		// kJTrue if we accept cancel requests
	JBoolean	itsAllowBackgroundFlag;	// kJTrue if process can go into background

	JSize		itsMaxStepCount;		// total number of steps for fixed length process
	JSize		itsCurrentStepCount;	// current step of process

private:

	// not allowed

	JProgressDisplay(const JProgressDisplay& source);
	const JProgressDisplay& operator=(const JProgressDisplay& source);
};


/******************************************************************************
 FixedLengthProcessBeginning

 ******************************************************************************/

inline void
JProgressDisplay::FixedLengthProcessBeginning
	(
	const JSize			stepCount,
	const JCharacter*	message,
	const JBoolean		allowCancel,
	const JBoolean		allowBackground
	)
{
	ProcessBeginning(kFixedLengthProcess, stepCount,
					 message, allowCancel, allowBackground);
}

/******************************************************************************
 VariableLengthProcessBeginning

 ******************************************************************************/

inline void
JProgressDisplay::VariableLengthProcessBeginning
	(
	const JCharacter*	message,
	const JBoolean		allowCancel,
	const JBoolean		allowBackground
	)
{
	ProcessBeginning(kVariableLengthProcess, 0,
					 message, allowCancel, allowBackground);
}

/******************************************************************************
 ProcessRunning

 ******************************************************************************/

inline JBoolean
JProgressDisplay::ProcessRunning()
	const
{
	return JConvertToBoolean( itsCurrentProcess != kNoRunningProcess );
}

/******************************************************************************
 GetCurrentProcessType

 ******************************************************************************/

inline JProgressDisplay::ProcessType
JProgressDisplay::GetCurrentProcessType()
	const
{
	return itsCurrentProcess;
}

/******************************************************************************
 AllowCancel (protected)

 ******************************************************************************/

inline JBoolean
JProgressDisplay::AllowCancel()
	const
{
	return itsAllowCancelFlag;
}

/******************************************************************************
 AllowBackground (protected)

 ******************************************************************************/

inline JBoolean
JProgressDisplay::AllowBackground()
	const
{
	return itsAllowBackgroundFlag;
}

/******************************************************************************
 GetMaxStepCount (protected)

 ******************************************************************************/

inline JSize
JProgressDisplay::GetMaxStepCount()
	const
{
	return itsMaxStepCount;
}

/******************************************************************************
 GetCurrentStepCount

 ******************************************************************************/

inline JSize
JProgressDisplay::GetCurrentStepCount()
	const
{
	return itsCurrentStepCount;
}

#endif
