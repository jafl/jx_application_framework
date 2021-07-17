/******************************************************************************
 THXAboutDialog.h

	Interface for the THXAboutDialog class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_THXAboutDialog
#define _H_THXAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;

class THXAboutDialog : public JXDialogDirector
{
public:

	THXAboutDialog(JXDirector* supervisor, const JString& prevVersStr);

	virtual ~THXAboutDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsIsUpgradeFlag;

// begin JXLayout

	JXTextButton* itsCreditsButton;
	JXTextButton* itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const JString& prevVersStr);

	// not allowed

	THXAboutDialog(const THXAboutDialog& source);
	const THXAboutDialog& operator=(const THXAboutDialog& source);
};

#endif
