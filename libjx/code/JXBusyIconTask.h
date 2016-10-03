/******************************************************************************
 JXBusyIconTask.h

	Copyright (C) 1997-2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXBusyIconTask
#define _H_JXBusyIconTask

#include <JXAnimationTask.h>
#include <JPtrArray.h>

class JXBusyIconTask : public JXAnimationTask
{
public:

	JXBusyIconTask(JXImageWidget* widget);

	virtual ~JXBusyIconTask();

protected:

	virtual JSize		GetFrameCount();
	virtual void		GetFrameTime(const JIndex frameIndex,
									 Time* tmin, Time* tmax);
	virtual JXImage*	GetFrame(const JIndex frameIndex);

private:

	JPtrArray<JXImage>*	itsImageList;

private:

	// not allowed

	JXBusyIconTask(const JXBusyIconTask& source);
	const JXBusyIconTask& operator=(const JXBusyIconTask& source);
};

#endif
