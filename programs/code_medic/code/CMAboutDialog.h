/******************************************************************************
 CMAboutDialog.h

	Interface for the CMAboutDialog class

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_CMAboutDialog
#define _H_CMAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;

class CMAboutDialog : public JXDialogDirector
{
public:

	CMAboutDialog(JXDirector* supervisor, const JString& prevVersStr);

	virtual ~CMAboutDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool				itsIsUpgradeFlag;

// begin JXLayout

	JXTextButton* itsHelpButton;
	JXTextButton* itsCreditsButton;

// end JXLayout

private:

	void	BuildWindow(const JString& prevVersStr);

	// not allowed

	CMAboutDialog(const CMAboutDialog& source);
	const CMAboutDialog& operator=(const CMAboutDialog& source);
};

#endif
