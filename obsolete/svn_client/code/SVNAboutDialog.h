/******************************************************************************
 SVNAboutDialog.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNAboutDialog
#define _H_SVNAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;

class SVNAboutDialog : public JXDialogDirector
{
public:

	SVNAboutDialog(JXDirector* supervisor, const JCharacter* prevVersStr);

	virtual	~SVNAboutDialog();

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

	SVNAboutDialog(const SVNAboutDialog& source);
	const SVNAboutDialog& operator=(const SVNAboutDialog& source);
};

#endif
