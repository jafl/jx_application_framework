/******************************************************************************
 JProgressDisplay.h

	Interface for the JProgressDisplay class.

	Copyright (C) 1994-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JProgressDisplay
#define _H_JProgressDisplay

#include "JString.h"

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

	void	FixedLengthProcessBeginning(const JSize stepCount, const JString& message,
										const bool allowCancel,
										const bool modal);
	void	VariableLengthProcessBeginning(const JString& message,
										   const bool allowCancel,
										   const bool modal);

	virtual bool	IncrementProgress(const JString& message = JString::empty);
	virtual bool	IncrementProgress(const JSize delta);
	virtual bool	IncrementProgress(const JString& message, const JSize delta) = 0;
	virtual bool	ProcessContinuing();
	virtual void	ProcessFinished();

	bool		ProcessRunning() const;
	ProcessType	GetCurrentProcessType() const;
	JSize		GetCurrentStepCount() const;

	// called automatically when not in background

	virtual void	DisplayBusyCursor() = 0;

protected:

	virtual void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
									 const JString& message, const bool allowCancel,
									 const bool modal);

	virtual bool	CheckForCancel() = 0;
	bool			AllowCancel() const;
	bool			IsModal() const;

	JSize	GetMaxStepCount() const;			// only for fixed length processes
	void	IncrementStepCount(const JSize delta = 1);

private:

	ProcessType	itsCurrentProcess;		// type of process currently running
	bool		itsAllowCancelFlag;		// true if we accept cancel requests
	bool		itsModalFlag;			// true if process can go into background

	JSize		itsMaxStepCount;		// total number of steps for fixed length process
	JSize		itsCurrentStepCount;	// current step of process

private:

	// not allowed

	JProgressDisplay(const JProgressDisplay&) = delete;
	JProgressDisplay& operator=(const JProgressDisplay&) = delete;
};


/******************************************************************************
 FixedLengthProcessBeginning

 ******************************************************************************/

inline void
JProgressDisplay::FixedLengthProcessBeginning
	(
	const JSize		stepCount,
	const JString&	message,
	const bool		allowCancel,
	const bool		modal
	)
{
	ProcessBeginning(kFixedLengthProcess, stepCount,
					 message, allowCancel, modal);
}

/******************************************************************************
 VariableLengthProcessBeginning

 ******************************************************************************/

inline void
JProgressDisplay::VariableLengthProcessBeginning
	(
	const JString&	message,
	const bool		allowCancel,
	const bool		modal
	)
{
	ProcessBeginning(kVariableLengthProcess, 0,
					 message, allowCancel, modal);
}

/******************************************************************************
 ProcessRunning

 ******************************************************************************/

inline bool
JProgressDisplay::ProcessRunning()
	const
{
	return itsCurrentProcess != kNoRunningProcess;
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

inline bool
JProgressDisplay::AllowCancel()
	const
{
	return itsAllowCancelFlag;
}

/******************************************************************************
 IsModal (protected)

 ******************************************************************************/

inline bool
JProgressDisplay::IsModal()
	const
{
	return itsModalFlag;
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
