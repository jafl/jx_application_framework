/******************************************************************************
 GPMAboutDialog.h

	Copyright © 2001 by New Planet Software. All rights reserved.

 ******************************************************************************/

#ifndef _H_GPMAboutDialog
#define _H_GPMAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;

class GPMAboutDialog : public JXDialogDirector
{
public:

	GPMAboutDialog(JXDirector* supervisor, const JCharacter* prevVersStr);

	virtual ~GPMAboutDialog();

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

	GPMAboutDialog(const GPMAboutDialog& source);
	const GPMAboutDialog& operator=(const GPMAboutDialog& source);
};

#endif
