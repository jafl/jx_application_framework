/******************************************************************************
 JXSplashWindow.h

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXSplashWindow
#define _H_JXSplashWindow

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JXPM.h>
#include <time.h>

class JXImage;

class JXSplashWindow : public JXWindowDirector
{
public:

	JXSplashWindow(const JXPM& imageData, const JCharacter* text,
				   const time_t displayInterval);
	JXSplashWindow(JXImage* image, const JCharacter* text,
				   const time_t displayInterval);

	virtual ~JXSplashWindow();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	void	BuildWindow(JXImage* image, const JCharacter* text,
						const time_t displayInterval);

	// not allowed

	JXSplashWindow(const JXSplashWindow& source);
	const JXSplashWindow& operator=(const JXSplashWindow& source);
};

#endif
