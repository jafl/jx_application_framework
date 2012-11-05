/******************************************************************************
 CBExtEditorDialog.h

	Interface for the CBExtEditorDialog class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBExtEditorDialog
#define _H_CBExtEditorDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JString;
class JXTextCheckbox;
class JXInputField;

class CBExtEditorDialog : public JXDialogDirector
{
public:

	CBExtEditorDialog(JXDirector* supervisor,
					  const JBoolean editLocally,
					  const JCharacter* editFileCmd,
					  const JCharacter* editFileLineCmd,
					  const JBoolean editBinaryLocally,
					  const JCharacter* editBinaryFileCmd);

	virtual ~CBExtEditorDialog();

	void	GetPrefs(JBoolean* editTextLocally,
					 JString* editTextFileCmd,
					 JString* editTextFileLineCmd,
					 JBoolean* editBinaryLocally,
					 JString* editBinaryFileCmd) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

// begin JXLayout

	JXInputField*   itsEditTextFileCmdInput;
	JXInputField*   itsEditTextFileLineCmdInput;
	JXTextCheckbox* itsExtTextEditorCB;
	JXInputField*   itsEditBinaryFileCmdInput;
	JXTextCheckbox* itsExtBinaryEditorCB;

// end JXLayout

private:

	void	BuildWindow(const JBoolean editTextLocally,
						const JCharacter* editTextFileCmd,
						const JCharacter* editTextFileLineCmd,
						const JBoolean editBinaryLocally,
						const JCharacter* editBinaryFileCmd);

	void	UpdateDisplay();

	// not allowed

	CBExtEditorDialog(const CBExtEditorDialog& source);
	const CBExtEditorDialog& operator=(const CBExtEditorDialog& source);
};

#endif
