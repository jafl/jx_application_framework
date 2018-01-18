/******************************************************************************
 GLAboutDialog.h

	Interface for the GLAboutDialog class

	Copyright (C) 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GLAboutDialog
#define _H_GLAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;

class GLAboutDialog : public JXDialogDirector
{
public:

	GLAboutDialog(JXDirector* supervisor, const JCharacter* prevVersStr);

	virtual ~GLAboutDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JBoolean		itsIsUpgradeFlag;

// begin JXLayout

	JXTextButton* itsHelpButton;
	JXTextButton* itsCreditsButton;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* prevVersStr);

	// not allowed

	GLAboutDialog(const GLAboutDialog& source);
	const GLAboutDialog& operator=(const GLAboutDialog& source);
};

#endif
