/******************************************************************************
 SyGAboutDialog.h

	Interface for the SyGAboutDialog class

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_SyGAboutDialog
#define _H_SyGAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;

class SyGAboutDialog : public JXDialogDirector
{
public:

	SyGAboutDialog(JXDirector* supervisor, const JString& prevVersStr);

	virtual ~SyGAboutDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JBoolean	itsIsUpgradeFlag;

// begin JXLayout

	JXTextButton* itsHelpButton;
	JXTextButton* itsCreditsButton;

// end JXLayout

private:

	void	BuildWindow(const JString& prevVersStr);

	// not allowed

	SyGAboutDialog(const SyGAboutDialog& source);
	const SyGAboutDialog& operator=(const SyGAboutDialog& source);
};

#endif
