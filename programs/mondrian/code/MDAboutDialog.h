/******************************************************************************
 MDAboutDialog.h

	Copyright (C) 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_MDAboutDialog
#define _H_MDAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;

class MDAboutDialog : public JXDialogDirector
{
public:

	MDAboutDialog(JXDirector* supervisor, const JCharacter* prevVersStr);

	virtual	~MDAboutDialog();

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

	MDAboutDialog(const MDAboutDialog& source);
	const MDAboutDialog& operator=(const MDAboutDialog& source);
};

#endif
