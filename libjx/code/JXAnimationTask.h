/******************************************************************************
 JXAnimationTask.h

	Interface for the JXAnimationTask class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXAnimationTask
#define _H_JXAnimationTask

#include "jx-af/jx/JXIdleTask.h"
#include <jx-af/jcore/JKLRand.h>

class JXImage;
class JXImageWidget;

class JXAnimationTask : public JXIdleTask
{
public:

	JXAnimationTask(JXImageWidget* widget);

	~JXAnimationTask() override;

	void	Perform(const Time delta, Time* maxSleepTime) override;

protected:

	virtual JSize		GetFrameCount() = 0;
	virtual void		GetFrameTime(const JIndex frameIndex,
									 Time* tmin, Time* tmax) = 0;
	virtual JXImage*	GetFrame(const JIndex frameIndex) = 0;

private:

	JXImageWidget*	itsWidget;			// we don't own this
	JIndex			itsCurrentImage;
	JKLRand			itsRNG;
};

#endif
