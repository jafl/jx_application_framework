/******************************************************************************
 SyGAboutDialog.h

	Interface for the SyGAboutDialog class

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGAboutDialog
#define _H_SyGAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;

class SyGAboutDialog : public JXDialogDirector
{
public:

	SyGAboutDialog(JXDirector* supervisor, const JCharacter* prevVersStr);

	virtual ~SyGAboutDialog();

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

	SyGAboutDialog(const SyGAboutDialog& source);
	const SyGAboutDialog& operator=(const SyGAboutDialog& source);
};

#endif
