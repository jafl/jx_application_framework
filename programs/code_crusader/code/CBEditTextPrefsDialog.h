/******************************************************************************
 CBEditTextPrefsDialog.h

	Interface for the CBEditTextPrefsDialog class

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBEditTextPrefsDialog
#define _H_CBEditTextPrefsDialog

#include <JXDialogDirector.h>
#include "CBPrefsManager.h"		// need definition of kColorCount

class CBTextDocument;
class CBSampleText;

class JXTextButton;
class JXTextCheckbox;
class JXTextMenu;
class JXInputField;
class JXIntegerInput;
class JXCharInput;
class JXRadioGroup;
class JXChooseMonoFont;
class JXChooseColorDialog;

class CBEditTextPrefsDialog : public JXDialogDirector
{
public:

	CBEditTextPrefsDialog(CBTextDocument* doc);

	virtual ~CBEditTextPrefsDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBTextDocument*	itsDoc;		// not owned
	JColorID		itsColor [ CBPrefsManager::kColorCount ];
	JXTextButton*	itsColorButton [ CBPrefsManager::kColorCount ];
	JIndex			itsEmulatorIndex;
	JIndex			itsOrigEmulatorIndex;

	// used when selecting custom color

	JXChooseColorDialog*	itsChooseColorDialog;	// can be nullptr
	JIndex					itsChooseColorIndex;

// begin JXLayout

	JXIntegerInput*   itsTabCharCountInput;
	JXIntegerInput*   itsCRMLineWidthInput;
	JXIntegerInput*   itsUndoDepthInput;
	JXTextCheckbox*   itsCreateBackupCB;
	JXTextCheckbox*   itsAutoIndentCB;
	JXTextCheckbox*   itsExtraSpaceWindTitleCB;
	JXTextCheckbox*   itsOpenComplFileOnTopCB;
	JXChooseMonoFont* itsFontMenu;
	JXTextMenu*       itsEmulatorMenu;
	JXTextCheckbox*   itsUseDNDCB;
	JXTextCheckbox*   itsOnlyBackupIfNoneCB;
	JXTextCheckbox*   itsLeftToFrontOfTextCB;
	JXTextButton*     itsHelpButton;
	JXTextCheckbox*   itsBalanceWhileTypingCB;
	JXTextCheckbox*   itsScrollToBalanceCB;
	JXTextCheckbox*   itsBeepWhenTypeUnbalancedCB;
	JXTextCheckbox*   itsSmartTabCB;
	JXRadioGroup*     itsPWModRG;
	JXTextCheckbox*   itsCopyWhenSelectCB;
	JXTextButton*     itsDefColorsButton;
	JXTextButton*     itsInvColorsButton;
	JXTextCheckbox*   itsTabToSpacesCB;
	JXTextCheckbox*   itsHomeEndCB;
	JXTextCheckbox*   itsScrollCaretCB;
	JXTextCheckbox*   itsSortFnMenuCB;
	JXTextCheckbox*   itsPackFnMenuCB;
	JXTextCheckbox*   itsRightMarginCB;
	JXIntegerInput*   itsRightMarginInput;
	CBSampleText*     itsSampleText;
	JXTextCheckbox*   itsNSInFnMenuCB;
	JXTextCheckbox*   itsMiddleButtonPasteCB;

// end JXLayout

private:

	void	BuildWindow(CBTextDocument* doc);
	void	UpdateSettings();

	bool	HandleColorButton(JBroadcaster* sender);
	void		SetDefaultColors();
	void		SetReverseVideoColors();
	void		ChangeColor(const JIndex colorIndex, const JColorID color);
	void		UpdateSampleText();

	void	UpdateDisplay();

	// not allowed

	CBEditTextPrefsDialog(const CBEditTextPrefsDialog& source);
	const CBEditTextPrefsDialog& operator=(const CBEditTextPrefsDialog& source);
};

#endif
