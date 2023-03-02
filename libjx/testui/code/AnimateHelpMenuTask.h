/******************************************************************************
 AnimateHelpMenuTask.h

	Interface for the AnimateHelpMenuTask class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_AnimateHelpMenuTask
#define _H_AnimateHelpMenuTask

#include <jx-af/jx/JXIdleTask.h>
#include <jx-af/jcore/JPtrArray.h>

class JXTextMenu;
class JXImage;

class AnimateHelpMenuTask : public JXIdleTask
{
public:

	AnimateHelpMenuTask(JXTextMenu* menu, const JIndex menuItem);

	~AnimateHelpMenuTask() override;

	void	Reset();

protected:

	void	Perform(const Time delta) override;

private:

	JXTextMenu*			itsMenu;			// we don't own this
	const JIndex		itsMenuItem;
	JPtrArray<JXImage>*	itsImageList;
	JIndex				itsCurrentImage;
};

#endif
