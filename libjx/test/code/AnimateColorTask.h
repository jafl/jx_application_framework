/******************************************************************************
 AnimateColorTask.h

	Interface for the AnimateColorTask class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_AnimateColorTask
#define _H_AnimateColorTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>
#include <jColor.h>
#include <JArray.h>

class JXColormap;

class AnimateColorTask : public JXIdleTask
{
public:

	AnimateColorTask(JXColormap* colormap,
					 JArray<JDynamicColorInfo>* colorList);

	virtual ~AnimateColorTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JXColormap*					itsColormap;
	JArray<JDynamicColorInfo>*	itsColorList;
	JIndex						itsWavefrontIndex;	// gets pattern started
	Time						itsElapsedTime;		// milliseconds

private:

	// not allowed

	AnimateColorTask(const AnimateColorTask& source);
	const AnimateColorTask& operator=(const AnimateColorTask& source);
};

#endif
