/******************************************************************************
 TestInputFieldsDialog.h

	Interface for the TestInputFieldsDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestInputFieldsDialog
#define _H_TestInputFieldsDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXIntegerInput;
class JXFileInput;
class JXPathInput;
class JXTextButton;

class TestInputFieldsDialog : public JXModalDialogDirector
{
public:

	TestInputFieldsDialog();

	~TestInputFieldsDialog() override;

protected:

	bool	OKToDeactivate() override;

private:

// begin JXLayout

	JXTextButton*   itsChoosePathButton;
	JXTextButton*   itsChooseFileButton;
	JXIntegerInput* itsLowerValue;
	JXIntegerInput* itsUpperValue;
	JXPathInput*    itsPathInput;
	JXFileInput*    itsFileInput;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
