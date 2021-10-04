/******************************************************************************
 JXSplashWindow.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSplashWindow
#define _H_JXSplashWindow

#include "jx-af/jx/JXWindowDirector.h"
#include "jx-af/jcore/JXPM.h"
#include <time.h>

class JXImage;

class JXSplashWindow : public JXWindowDirector
{
public:

	JXSplashWindow(const JXPM& imageData, const JString& text,
				   const time_t displayInterval);
	JXSplashWindow(JXImage* image, const JString& text,
				   const time_t displayInterval);

	virtual ~JXSplashWindow();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	void	BuildWindow(JXImage* image, const JString& text,
						const time_t displayInterval);
};

#endif
