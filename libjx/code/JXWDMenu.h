/******************************************************************************
 JXWDMenu.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWDMenu
#define _H_JXWDMenu

#include "jx-af/jx/JXTextMenu.h"

class JXWDManager;

class JXWDMenu : public JXTextMenu
{
public:

	JXWDMenu(const JString& title, JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	JXWDMenu(JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	~JXWDMenu() override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXWDManager*	itsWDMgr;

private:

	void	JXWDMenuX();
};

#endif
