/******************************************************************************
 CBAboutDialog.h

	Interface for the CBAboutDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBAboutDialog
#define _H_CBAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;
class CBAboutDialogIconTask;

class CBAboutDialog : public JXDialogDirector
{
public:

	CBAboutDialog(JXDirector* supervisor, const JCharacter* prevVersStr);

	virtual ~CBAboutDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JBoolean				itsIsUpgradeFlag;
	CBAboutDialogIconTask*	itsAnimTask;

// begin JXLayout

	JXTextButton* itsHelpButton;
	JXTextButton* itsCreditsButton;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* prevVersStr);

	// not allowed

	CBAboutDialog(const CBAboutDialog& source);
	const CBAboutDialog& operator=(const CBAboutDialog& source);
};

#endif
