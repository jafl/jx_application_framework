/******************************************************************************
 GMAboutDialog.h

	Interface for the GMAboutDialog class

	Copyright (C) 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GMAboutDialog
#define _H_GMAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;
class GMAboutDialogIconTask;

class GMAboutDialog : public JXDialogDirector
{
public:

	GMAboutDialog(JXDirector* supervisor, const JCharacter* prevVersStr);

	virtual ~GMAboutDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JBoolean				itsIsUpgradeFlag;
	GMAboutDialogIconTask*	itsAnimTask;

// begin JXLayout

	JXTextButton* itsHelpButton;
	JXTextButton* itsCreditsButton;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* prevVersStr);

	// not allowed

	GMAboutDialog(const GMAboutDialog& source);
	const GMAboutDialog& operator=(const GMAboutDialog& source);
};

#endif
