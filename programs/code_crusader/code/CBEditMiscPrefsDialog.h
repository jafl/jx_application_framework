/******************************************************************************
 CBEditMiscPrefsDialog.h

	Interface for the CBEditMiscPrefsDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBEditMiscPrefsDialog
#define _H_CBEditMiscPrefsDialog

#include <JXDialogDirector.h>

class JXRadioGroup;
class JXTextCheckbox;

class CBEditMiscPrefsDialog : public JXDialogDirector
{
public:

	CBEditMiscPrefsDialog();

	virtual ~CBEditMiscPrefsDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextCheckbox* itsSaveAllCB;
	JXTextCheckbox* itsCloseAllCB;
	JXTextCheckbox* itsQuitCB;
	JXTextCheckbox* itsNewEditorCB;
	JXTextCheckbox* itsNewProjectCB;
	JXTextCheckbox* itsChooseFileCB;
	JXTextCheckbox* itsReopenLastCB;
	JXTextCheckbox* itsCopyWhenSelectCB;
	JXTextCheckbox* itsCloseUnsavedCB;
	JXTextCheckbox* itsMacStyleCB;
	JXTextCheckbox* itsOpenOldProjCB;
	JXTextCheckbox* itsFocusInDockCB;
	JXTextCheckbox* itsMiddleButtonPasteCB;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateSettings();

	// not allowed

	CBEditMiscPrefsDialog(const CBEditMiscPrefsDialog& source);
	const CBEditMiscPrefsDialog& operator=(const CBEditMiscPrefsDialog& source);
};

#endif
