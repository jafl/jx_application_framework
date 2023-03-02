/******************************************************************************
 JXMacWinPrefsDialog.h

	Interface for the JXMacWinPrefsDialog class

	Copyright (C) 2002 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXMacWinPrefsDialog
#define _H_JXMacWinPrefsDialog

#include "JXModalDialogDirector.h"

class JXTextCheckbox;

class JXMacWinPrefsDialog : public JXModalDialogDirector
{
public:

	static void	EditPrefs();

	~JXMacWinPrefsDialog() override;

protected:

	JXMacWinPrefsDialog();

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
