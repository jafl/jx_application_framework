/******************************************************************************
 JXBusyIconTask.h

	Copyright (C) 1997-2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXBusyIconTask
#define _H_JXBusyIconTask

#include "jx-af/jx/JXAnimationTask.h"
#include <jx-af/jcore/JPtrArray.h>

class JXBusyIconTask : public JXAnimationTask
{
public:

	JXBusyIconTask(JXImageWidget* widget);

	~JXBusyIconTask() override;

protected:

	JSize		GetFrameCount() override;
	void		GetFrameTime(const JIndex frameIndex,
							 Time* tmin, Time* tmax) override;
	JXImage*	GetFrame(const JIndex frameIndex) override;

private:

	JPtrArray<JXImage>*	itsImageList;
};

#endif
