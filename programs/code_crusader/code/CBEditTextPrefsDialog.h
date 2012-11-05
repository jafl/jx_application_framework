/******************************************************************************
 CBEditTextPrefsDialog.h

	Interface for the CBEditTextPrefsDialog class

	Copyright © 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBEditTextPrefsDialog
#define _H_CBEditTextPrefsDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CBTextDocument*	itsDoc;		// not owned
	JColorIndex		itsColor [ CBPrefsManager::kColorCount ];
	JXTextButton*	itsColorButton [ CBPrefsManager::kColorCount ];
	JIndex			itsEmulatorIndex;
	JIndex			itsOrigEmulatorIndex;

	// used when selecting custom color

	JXChooseColorDialog*	itsChooseColorDialog;	// can be NULL
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

	JBoolean	HandleColorButton(JBroadcaster* sender);
	void		SetDefaultColors();
	void		SetReverseVideoColors();
	void		ChangeColor(const JIndex colorIndex, const JColorIndex color);
	void		UpdateSampleText();

	void	UpdateDisplay();

	// not allowed

	CBEditTextPrefsDialog(const CBEditTextPrefsDialog& source);
	const CBEditTextPrefsDialog& operator=(const CBEditTextPrefsDialog& source);
};

#endif
