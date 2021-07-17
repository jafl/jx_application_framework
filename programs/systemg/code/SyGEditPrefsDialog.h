/******************************************************************************
 SyGEditPrefsDialog.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_SyGEditPrefsDialog
#define _H_SyGEditPrefsDialog

#include <JXDialogDirector.h>
#include <JPtrArray-JString.h>

class JXInputField;
class JXRadioGroup;
class JXTextCheckbox;

class SyGEditPrefsDialog : public JXDialogDirector
{
public:

	SyGEditPrefsDialog(const JString& terminalCmd,
					   const JString& manViewCmd,
					   const JString& gitStatusCmd,
					   const JString& gitHistoryCmd,
					   const JString& postCheckoutCmd,
					   const bool del, const bool newWindows,
					   const bool perFolderPrefs);

	virtual ~SyGEditPrefsDialog();

	void	GetPrefs(JString* terminalCmd, JString* manViewCmd,
					 JString* gitStatusCmd, JString* gitHistoryCmd,
					 JString* postCheckoutCmd, bool* del,
					 bool* newWindows, bool* perFolderPrefs) const;

private:

// begin JXLayout

	JXInputField*   itsManInput;
	JXInputField*   itsTerminalInput;
	JXInputField*   itsGitStatusInput;
	JXTextCheckbox* itsDelCB;
	JXTextCheckbox* itsAllowSpaceCB;
	JXInputField*   itsGitHistoryInput;
	JXInputField*   itsPostCheckoutInput;
	JXTextCheckbox* itsOpenNewWindowsCB;
	JXTextCheckbox* itsFolderPrefsCB;

// end JXLayout

private:

	void	BuildWindow(const JString& terminalCmd,
						const JString& manViewCmd,
						const JString& gitStatusCmd,
						const JString& gitHistoryCmd,
						const JString& postCheckoutCmd,
						const bool del, const bool newWindows,
						const bool perFolderPrefs);

	// not allowed

	SyGEditPrefsDialog(const SyGEditPrefsDialog& source);
	const SyGEditPrefsDialog& operator=(const SyGEditPrefsDialog& source);
};

#endif
