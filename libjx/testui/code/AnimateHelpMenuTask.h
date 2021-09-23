/******************************************************************************
 AnimateHelpMenuTask.h

	Interface for the AnimateHelpMenuTask class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_AnimateHelpMenuTask
#define _H_AnimateHelpMenuTask

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
};

#endif
