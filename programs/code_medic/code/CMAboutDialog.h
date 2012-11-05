/******************************************************************************
 CMAboutDialog.h

	Interface for the CMAboutDialog class

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMAboutDialog
#define _H_CMAboutDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXTextButton;

class CMAboutDialog : public JXDialogDirector
{
public:

	CMAboutDialog(JXDirector* supervisor, const JCharacter* prevVersStr);

	virtual ~CMAboutDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JBoolean				itsIsUpgradeFlag;

// begin JXLayout

	JXTextButton* itsHelpButton;
	JXTextButton* itsCreditsButton;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* prevVersStr);

	// not allowed

	CMAboutDialog(const CMAboutDialog& source);
	const CMAboutDialog& operator=(const CMAboutDialog& source);
};

#endif
