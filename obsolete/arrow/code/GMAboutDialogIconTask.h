/******************************************************************************
 GMAboutDialogIconTask.h

	Interface for the GMAboutDialogIconTask class

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GMAboutDialogIconTask
#define _H_GMAboutDialogIconTask

#include <JXAnimationTask.h>
#include <JPtrArray.h>

class GMAboutDialogIconTask : public JXAnimationTask
{
public:

	GMAboutDialogIconTask(JXImageWidget* widget);

	virtual ~GMAboutDialogIconTask();

protected:

	virtual JSize		GetFrameCount();
	virtual void		GetFrameTime(const JIndex frameIndex,
									 Time* tmin, Time* tmax);
	virtual JXImage*	GetFrame(const JIndex frameIndex);

private:

	JPtrArray<JXImage>*	itsImageList;

private:

	// not allowed

	GMAboutDialogIconTask(const GMAboutDialogIconTask& source);
	const GMAboutDialogIconTask& operator=(const GMAboutDialogIconTask& source);
};

#endif
