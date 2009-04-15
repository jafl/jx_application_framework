/******************************************************************************
 JLatentPG.h

	Interface for the JLatentPG class.

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JLatentPG
#define _H_JLatentPG

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JProgressDisplay.h>
#include <jTime.h>

class JLatentPG : public JProgressDisplay
{
public:

	JLatentPG(const JSize scaleFactor = 1);
	JLatentPG(JProgressDisplay* pg, const JBoolean ownIt,
			  const JSize scaleFactor = 1);

	virtual ~JLatentPG();

	void	UseDefaultPG();
	void	SetPG(JProgressDisplay* pg, const JBoolean ownIt);

	time_t	GetMaxSilentTime() const;
	void	SetMaxSilentTime(const time_t max);

	JSize	GetScaleFactor() const;
	void	SetScaleFactor(const JSize scaleFactor);

	virtual JBoolean	IncrementProgress(const JCharacter* message = NULL);
	virtual JBoolean	IncrementProgress(const JSize delta);
	virtual JBoolean	IncrementProgress(const JCharacter* message,
										  const JSize delta);
	virtual JBoolean	ProcessContinuing();
	virtual void		ProcessFinished();
	virtual void		DisplayBusyCursor();

protected:

	virtual void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
									 const JCharacter* message, const JBoolean allowCancel,
									 const JBoolean allowBackground);

	virtual JBoolean	CheckForCancel();

private:

	JProgressDisplay*	itsPG;
	JBoolean			itsOwnsPGFlag;
	JString*			itsMessage;

	time_t	itsStartTime;
	time_t	itsMaxSilentTime;
	JSize	itsCounter;
	JSize	itsScaleFactor;

private:

	void	StartInternalProcess();
	int		TimeToStart() const;
	int		TimeToUpdate() const;

	// not allowed

	JLatentPG(const JLatentPG& source);
	const JLatentPG& operator=(const JLatentPG& source);
};


/******************************************************************************
 Maximum silent time

 ******************************************************************************/

inline time_t
JLatentPG::GetMaxSilentTime()
	const
{
	return itsMaxSilentTime;
}

inline void
JLatentPG::SetMaxSilentTime
	(
	const time_t max
	)
{
	itsMaxSilentTime = max;
}

/******************************************************************************
 Scale factor for how often to redraw

 ******************************************************************************/

inline JSize
JLatentPG::GetScaleFactor()
	const
{
	return itsScaleFactor;
}

inline void
JLatentPG::SetScaleFactor
	(
	const JSize scaleFactor
	)
{
	itsScaleFactor = scaleFactor;
}

/******************************************************************************
 When to increment display (private)

 ******************************************************************************/

inline int
JLatentPG::TimeToStart()
	const
{
	return (itsCounter % itsScaleFactor == 0 &&				// avoid calling time() too often
			time(NULL) - itsStartTime >= itsMaxSilentTime &&
			(GetCurrentProcessType() == kVariableLengthProcess ||
			 GetCurrentStepCount() < GetMaxStepCount()));
}

inline int
JLatentPG::TimeToUpdate()
	const
{
	return (itsCounter >= itsScaleFactor);
}

#endif
