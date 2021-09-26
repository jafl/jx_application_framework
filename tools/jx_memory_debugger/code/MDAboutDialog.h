/******************************************************************************
 MDAboutDialog.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_MDAboutDialog
#define _H_MDAboutDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXTextButton;

class MDAboutDialog : public JXDialogDirector
{
public:

	MDAboutDialog(JXDirector* supervisor, const JString& prevVersStr);

	virtual	~MDAboutDialog();

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
};

#endif
