/******************************************************************************
 JXAnimationTask.h

	Interface for the JXAnimationTask class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXAnimationTask
#define _H_JXAnimationTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>
#include <JKLRand.h>

class JXImage;
class JXImageWidget;

class JXAnimationTask : public JXIdleTask
{
public:

	JXAnimationTask(JXImageWidget* widget);

	virtual ~JXAnimationTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

protected:

	virtual JSize		GetFrameCount() = 0;
	virtual void		GetFrameTime(const JIndex frameIndex,
									 Time* tmin, Time* tmax) = 0;
	virtual JXImage*	GetFrame(const JIndex frameIndex) = 0;

private:

	JXImageWidget*	itsWidget;			// we don't own this
	JIndex			itsCurrentImage;
	JKLRand			itsRNG;

private:

	// not allowed

	JXAnimationTask(const JXAnimationTask& source);
	const JXAnimationTask& operator=(const JXAnimationTask& source);
};

#endif
