/******************************************************************************
 GFGAboutDialog.h

	Copyright © 2002 by New Planet Software. All rights reserved.

 ******************************************************************************/

#ifndef _H_GFGAboutDialog
#define _H_GFGAboutDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXTextButton;

class GFGAboutDialog : public JXDialogDirector
{
public:

	GFGAboutDialog(JXDirector* supervisor, const JCharacter* prevVersStr);

	virtual	~GFGAboutDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JBoolean	itsIsUpgradeFlag;

// begin JXLayout

	JXTextButton* itsHelpButton;
	JXTextButton* itsCreditsButton;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* prevVersStr);

	// not allowed

	GFGAboutDialog(const GFGAboutDialog& source);
	const GFGAboutDialog& operator=(const GFGAboutDialog& source);
};

#endif
