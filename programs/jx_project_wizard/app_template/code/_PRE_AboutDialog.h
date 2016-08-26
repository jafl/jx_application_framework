/******************************************************************************
 <PRE>AboutDialog.h

	Copyright © <Year> by <Company>. All rights reserved.

 ******************************************************************************/

#ifndef _H_<PRE>AboutDialog
#define _H_<PRE>AboutDialog

#include <JXDialogDirector.h>

class JXTextButton;

class <PRE>AboutDialog : public JXDialogDirector
{
public:

	<PRE>AboutDialog(JXDirector* supervisor, const JCharacter* prevVersStr);

	virtual	~<PRE>AboutDialog();

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

	<PRE>AboutDialog(const <PRE>AboutDialog& source);
	const <PRE>AboutDialog& operator=(const <PRE>AboutDialog& source);
};

#endif
