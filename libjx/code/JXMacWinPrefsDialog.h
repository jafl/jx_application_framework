/******************************************************************************
 JXMacWinPrefsDialog.h

	Interface for the JXMacWinPrefsDialog class

	Copyright (C) 2002 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXMacWinPrefsDialog
#define _H_JXMacWinPrefsDialog

#include "jx-af/jx/JXDialogDirector.h"

class JXTextCheckbox;

class JXMacWinPrefsDialog : public JXDialogDirector
{
public:

	static void	EditPrefs();

	~JXMacWinPrefsDialog();

protected:

	JXMacWinPrefsDialog();

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextCheckbox* itsMacStyleCB;
	JXTextCheckbox* itsHomeEndCB;
	JXTextCheckbox* itsScrollCaretCB;
	JXTextCheckbox* itsCopyWhenSelectCB;
	JXTextCheckbox* itsFocusInDockCB;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateSettings();
};

#endif
