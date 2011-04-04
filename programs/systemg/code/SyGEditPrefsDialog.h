/******************************************************************************
 SyGEditPrefsDialog.h

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGEditPrefsDialog
#define _H_SyGEditPrefsDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>
#include <JPtrArray-JString.h>

class JXInputField;
class JXRadioGroup;
class JXTextCheckbox;

class SyGEditPrefsDialog : public JXDialogDirector
{
public:

	SyGEditPrefsDialog(const JCharacter* terminalCmd,
					   const JCharacter* manViewCmd,
					   const JCharacter* gitStatusCmd,
					   const JCharacter* gitHistoryCmd,
					   const JCharacter* postCheckoutCmd,
					   const JBoolean del, const JBoolean newWindows,
					   const JBoolean perFolderPrefs);

	virtual ~SyGEditPrefsDialog();

	void	GetPrefs(JString* terminalCmd, JString* manViewCmd,
					 JString* gitStatusCmd, JString* gitHistoryCmd,
					 JString* postCheckoutCmd, JBoolean* del,
					 JBoolean* newWindows, JBoolean* perFolderPrefs) const;

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

	void	BuildWindow(const JCharacter* terminalCmd,
						const JCharacter* manViewCmd,
						const JCharacter* gitStatusCmd,
						const JCharacter* gitHistoryCmd,
						const JCharacter* postCheckoutCmd,
						const JBoolean del, const JBoolean newWindows,
						const JBoolean perFolderPrefs);

	// not allowed

	SyGEditPrefsDialog(const SyGEditPrefsDialog& source);
	const SyGEditPrefsDialog& operator=(const SyGEditPrefsDialog& source);
};

#endif
