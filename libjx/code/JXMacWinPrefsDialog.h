/******************************************************************************
 JXMacWinPrefsDialog.h

	Interface for the JXMacWinPrefsDialog class

	Copyright © 2002 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXMacWinPrefsDialog
#define _H_JXMacWinPrefsDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXTextCheckbox;

class JXMacWinPrefsDialog : public JXDialogDirector
{
public:

	static void	EditPrefs();

	virtual ~JXMacWinPrefsDialog();

protected:

	JXMacWinPrefsDialog();

	virtual void	Receive(JBroadcaster* sender, const Message& message);

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

	// not allowed

	JXMacWinPrefsDialog(const JXMacWinPrefsDialog& source);
	const JXMacWinPrefsDialog& operator=(const JXMacWinPrefsDialog& source);
};

#endif
