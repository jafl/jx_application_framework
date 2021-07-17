/******************************************************************************
 SVNAboutDialog.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNAboutDialog
#define _H_SVNAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;

class SVNAboutDialog : public JXDialogDirector
{
public:

	SVNAboutDialog(JXDirector* supervisor, const JString& prevVersStr);

	virtual	~SVNAboutDialog();

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

	SVNAboutDialog(const SVNAboutDialog& source);
	const SVNAboutDialog& operator=(const SVNAboutDialog& source);
};

#endif
