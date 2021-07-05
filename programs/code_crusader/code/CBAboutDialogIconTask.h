/******************************************************************************
 CBAboutDialogIconTask.h

	Interface for the CBAboutDialogIconTask class

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBAboutDialogIconTask
#define _H_CBAboutDialogIconTask

#include <JXAnimationTask.h>
#include <JPtrArray.h>

class CBAboutDialogIconTask : public JXAnimationTask
{
public:

	CBAboutDialogIconTask(JXImageWidget* widget);

	virtual ~CBAboutDialogIconTask();

protected:

	virtual JSize		GetFrameCount();
	virtual void		GetFrameTime(const JIndex frameIndex,
									 Time* tmin, Time* tmax);
	virtual JXImage*	GetFrame(const JIndex frameIndex);

private:

	JPtrArray<JXImage>*	itsImageList;

private:

	// not allowed

	CBAboutDialogIconTask(const CBAboutDialogIconTask& source);
	const CBAboutDialogIconTask& operator=(const CBAboutDialogIconTask& source);
};

#endif
