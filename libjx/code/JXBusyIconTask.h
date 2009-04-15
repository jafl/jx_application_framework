/******************************************************************************
 JXBusyIconTask.h

	Copyright © 1997-2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXBusyIconTask
#define _H_JXBusyIconTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
