/******************************************************************************
 JLatentPG.h

	Interface for the JLatentPG class.

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JLatentPG
#define _H_JLatentPG

#include "JProgressDisplay.h"
#include "JString.h"
#include "jTime.h"

class JLatentPG : public JProgressDisplay
{
public:

	JLatentPG(const JSize scaleFactor = 1);
	JLatentPG(JProgressDisplay* pg, const bool ownIt,
			  const JSize scaleFactor = 1);

	virtual ~JLatentPG();

	void	UseDefaultPG();
	void	SetPG(JProgressDisplay* pg, const bool ownIt);

	time_t	GetMaxSilentTime() const;
	void	SetMaxSilentTime(const time_t max);

	JSize	GetScaleFactor() const;
	void	SetScaleFactor(const JSize scaleFactor);

	virtual bool	IncrementProgress(const JString& message = JString::empty) override;
	virtual bool	IncrementProgress(const JSize delta) override;
	virtual bool	IncrementProgress(const JString& message,
										  const JSize delta) override;
	virtual bool	ProcessContinuing() override;
	virtual void		ProcessFinished() override;
	virtual void		DisplayBusyCursor() override;

protected:

	virtual void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
									 const JString& message, const bool allowCancel,
									 const bool allowBackground) override;

	virtual bool	CheckForCancel() override;

private:

	JProgressDisplay*	itsPG;
	bool			itsOwnsPGFlag;
	JString				itsMessage;

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

#endif
