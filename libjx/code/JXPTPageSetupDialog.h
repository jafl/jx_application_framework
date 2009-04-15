/******************************************************************************
 JXPTPageSetupDialog.h

	Interface for the JXPTPageSetupDialog class

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPTPageSetupDialog
#define _H_JXPTPageSetupDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXPTPrinter;
class JXTextButton;
class JXTextCheckbox;
class JXInputField;
class JXIntegerInput;

class JXPTPageSetupDialog : public JXDialogDirector
{
public:

	static JXPTPageSetupDialog*
		Create(const JCharacter* printCmd,
			   const JSize pageWidth, const JSize pageHeight,
			   const JSize minPageHeight,
			   const JBoolean printReverseOrder);

	virtual ~JXPTPageSetupDialog();

	JBoolean	SetParameters(JXPTPrinter* p) const;

protected:

	JXPTPageSetupDialog();

	void	SetObjects(JXTextButton* okButton, JXTextButton* cancelButton,
					   JXInputField* printCmdInput, const JCharacter* printCmd,
					   JXIntegerInput* widthInput, const JSize pageWidth,
					   JXIntegerInput* heightInput, const JSize pageHeight,
					   const JSize minPageHeight, JXTextButton* printTestButton,
					   JXTextCheckbox* printReverseOrderCB, const JBoolean printReverseOrder);

	JXIntegerInput*	GetWidthInput() const;
	JXIntegerInput*	GetHeightInput() const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

// begin JXLayout

    JXInputField*   itsPrintCmd;
    JXIntegerInput* itsWidth;
    JXIntegerInput* itsHeight;
    JXTextButton*   itsPrintTestButton;
    JXTextCheckbox* itsPrintReverseOrderCB;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* printCmd,
						const JSize pageWidth, const JSize pageHeight,
						const JSize minPageHeight,
						const JBoolean printReverseOrder);

	void	PrintTestPage();

	// not allowed

	JXPTPageSetupDialog(const JXPTPageSetupDialog& source);
	const JXPTPageSetupDialog& operator=(const JXPTPageSetupDialog& source);
};


/******************************************************************************
 Get input fields (protected)

	These are provided so derived classes can adjust the limits.

 ******************************************************************************/

inline JXIntegerInput*
JXPTPageSetupDialog::GetWidthInput()
	const
{
	return itsWidth;
}

inline JXIntegerInput*
JXPTPageSetupDialog::GetHeightInput()
	const
{
	return itsHeight;
}

#endif
