/******************************************************************************
 TestButtonsDialog.h

	Interface for the TestButtonsDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestButtonsDialog
#define _H_TestButtonsDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXRadioGroup;
class JXTextCheckbox;

class TestButtonsDialog : public JXModalDialogDirector
{
public:

	TestButtonsDialog();

	~TestButtonsDialog() override;

protected:

	bool	OKToDeactivate() override;

private:

// begin JXLayout

	JXRadioGroup*   itsRG1;
	JXRadioGroup*   itsRG2;
	JXTextCheckbox* itsEnable1CB;
	JXTextCheckbox* itsEnable2CB;
	JXTextCheckbox* itsEnable3CB;
	JXTextCheckbox* its1CB;
	JXTextCheckbox* its2CB;
	JXTextCheckbox* its3CB;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
