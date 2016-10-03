/******************************************************************************
 GNBAboutDialog.h

	Interface for the GNBAboutDialog class

	Copyright (C) 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GNBAboutDialog
#define _H_GNBAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;

class GNBAboutDialog : public JXDialogDirector
{
public:

	GNBAboutDialog(JXDirector* supervisor, const JCharacter* prevVersStr);

	virtual ~GNBAboutDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JBoolean				itsIsUpgradeFlag;

// begin JXLayout

    JXTextButton* itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* prevVersStr);

	// not allowed

	GNBAboutDialog(const GNBAboutDialog& source);
	const GNBAboutDialog& operator=(const GNBAboutDialog& source);
};

#endif
