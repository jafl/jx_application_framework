/******************************************************************************
 JXWDMenu.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXWDMenu
#define _H_JXWDMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTextMenu.h>

class JXWDManager;

class JXWDMenu : public JXTextMenu
{
public:

	JXWDMenu(const JCharacter* title, JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	JXWDMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~JXWDMenu();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXWDManager*	itsWDMgr;

private:

	void	JXWDMenuX();

	// not allowed

	JXWDMenu(const JXWDMenu& source);
	const JXWDMenu& operator=(const JXWDMenu& source);
};

#endif
