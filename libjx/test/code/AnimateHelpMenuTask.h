/******************************************************************************
 AnimateHelpMenuTask.h

	Interface for the AnimateHelpMenuTask class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_AnimateHelpMenuTask
#define _H_AnimateHelpMenuTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>
#include <JPtrArray.h>

class JXTextMenu;
class JXImage;

class AnimateHelpMenuTask : public JXIdleTask
{
public:

	AnimateHelpMenuTask(JXTextMenu* menu, const JIndex menuItem);

	virtual ~AnimateHelpMenuTask();

	void	Reset();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JXTextMenu*			itsMenu;			// we don't own this
	const JIndex		itsMenuItem;
	JPtrArray<JXImage>*	itsImageList;
	JIndex				itsCurrentImage;

private:

	// not allowed

	AnimateHelpMenuTask(const AnimateHelpMenuTask& source);
	const AnimateHelpMenuTask& operator=(const AnimateHelpMenuTask& source);
};

#endif
