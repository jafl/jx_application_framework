/******************************************************************************
 GFGAboutDialog.h

	Copyright (C) 2002 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GFGAboutDialog
#define _H_GFGAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;

class GFGAboutDialog : public JXDialogDirector
{
public:

	GFGAboutDialog(JXDirector* supervisor, const JString& prevVersStr);

	virtual	~GFGAboutDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsIsUpgradeFlag;

// begin JXLayout

	JXTextButton* itsHelpButton;
	JXTextButton* itsCreditsButton;

// end JXLayout

private:

	void	BuildWindow(const JString& prevVersStr);

	// not allowed

	GFGAboutDialog(const GFGAboutDialog& source);
	const GFGAboutDialog& operator=(const GFGAboutDialog& source);
};

#endif
