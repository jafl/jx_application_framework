/******************************************************************************
 CBAboutDialog.h

	Interface for the CBAboutDialog class

	Copyright © 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBAboutDialog
#define _H_CBAboutDialog

#include <JXDialogDirector.h>

class JXTextButton;
class CBAboutDialogIconTask;

class CBAboutDialog : public JXDialogDirector
{
public:

	CBAboutDialog(JXDirector* supervisor, const JString& prevVersStr);

	virtual ~CBAboutDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool				itsIsUpgradeFlag;
	CBAboutDialogIconTask*	itsAnimTask;

// begin JXLayout

	JXTextButton* itsHelpButton;
	JXTextButton* itsCreditsButton;

// end JXLayout

private:

	void	BuildWindow(const JString& prevVersStr);

	// not allowed

	CBAboutDialog(const CBAboutDialog& source);
	const CBAboutDialog& operator=(const CBAboutDialog& source);
};

#endif
