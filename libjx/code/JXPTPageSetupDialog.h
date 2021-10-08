/******************************************************************************
 JXPTPageSetupDialog.h

	Interface for the JXPTPageSetupDialog class

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPTPageSetupDialog
#define _H_JXPTPageSetupDialog

#include "jx-af/jx/JXDialogDirector.h"

class JXPTPrinter;
class JXTextButton;
class JXTextCheckbox;
class JXInputField;
class JXIntegerInput;

class JXPTPageSetupDialog : public JXDialogDirector
{
public:

	static JXPTPageSetupDialog*
		Create(const JString& printCmd,
			   const JSize pageWidth, const JSize pageHeight,
			   const JSize minPageHeight,
			   const bool printReverseOrder);

	~JXPTPageSetupDialog() override;

	bool	SetParameters(JXPTPrinter* p) const;

protected:

	JXPTPageSetupDialog();

	void	SetObjects(JXTextButton* okButton, JXTextButton* cancelButton,
					   JXInputField* printCmdInput, const JString& printCmd,
					   JXIntegerInput* widthInput, const JSize pageWidth,
					   JXIntegerInput* heightInput, const JSize pageHeight,
					   const JSize minPageHeight, JXTextButton* printTestButton,
					   JXTextCheckbox* printReverseOrderCB, const bool printReverseOrder);

	JXIntegerInput*	GetWidthInput() const;
	JXIntegerInput*	GetHeightInput() const;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXInputField*   itsPrintCmd;
	JXIntegerInput* itsWidth;
	JXIntegerInput* itsHeight;
	JXTextButton*   itsPrintTestButton;
	JXTextCheckbox* itsPrintReverseOrderCB;

// end JXLayout

private:

	void	BuildWindow(const JString& printCmd,
						const JSize pageWidth, const JSize pageHeight,
						const JSize minPageHeight,
						const bool printReverseOrder);

	void	PrintTestPage();
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
