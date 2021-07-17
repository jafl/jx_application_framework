/******************************************************************************
 CBExtEditorDialog.h

	Interface for the CBExtEditorDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBExtEditorDialog
#define _H_CBExtEditorDialog

#include <JXDialogDirector.h>

class JString;
class JXTextCheckbox;
class JXInputField;

class CBExtEditorDialog : public JXDialogDirector
{
public:

	CBExtEditorDialog(JXDirector* supervisor,
					  const bool editLocally,
					  const JString& editFileCmd,
					  const JString& editFileLineCmd,
					  const bool editBinaryLocally,
					  const JString& editBinaryFileCmd);

	virtual ~CBExtEditorDialog();

	void	GetPrefs(bool* editTextLocally,
					 JString* editTextFileCmd,
					 JString* editTextFileLineCmd,
					 bool* editBinaryLocally,
					 JString* editBinaryFileCmd) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXInputField*   itsEditTextFileCmdInput;
	JXInputField*   itsEditTextFileLineCmdInput;
	JXTextCheckbox* itsExtTextEditorCB;
	JXInputField*   itsEditBinaryFileCmdInput;
	JXTextCheckbox* itsExtBinaryEditorCB;

// end JXLayout

private:

	void	BuildWindow(const bool editTextLocally,
						const JString& editTextFileCmd,
						const JString& editTextFileLineCmd,
						const bool editBinaryLocally,
						const JString& editBinaryFileCmd);

	void	UpdateDisplay();

	// not allowed

	CBExtEditorDialog(const CBExtEditorDialog& source);
	const CBExtEditorDialog& operator=(const CBExtEditorDialog& source);
};

#endif
