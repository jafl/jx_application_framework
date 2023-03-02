/******************************************************************************
 TestPopupChoiceDialog.h

	Interface for the TestPopupChoiceDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPopupChoiceDialog
#define _H_TestPopupChoiceDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class TestPopupChoiceDialog : public JXModalDialogDirector
{
public:

	TestPopupChoiceDialog();

	~TestPopupChoiceDialog() override;

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();
};

#endif
