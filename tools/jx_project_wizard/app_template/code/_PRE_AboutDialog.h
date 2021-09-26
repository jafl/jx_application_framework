/******************************************************************************
 <PRE>AboutDialog.h

	Copyright (C) <Year> by <Company>.

 ******************************************************************************/

#ifndef _H_<PRE>AboutDialog
#define _H_<PRE>AboutDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXTextButton;

class <PRE>AboutDialog : public JXDialogDirector
{
public:

	<PRE>AboutDialog(JXDirector* supervisor, const JString& prevVersStr);

	virtual	~<PRE>AboutDialog();

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
