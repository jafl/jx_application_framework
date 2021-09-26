/******************************************************************************
 TestPopupChoiceDialog.h

	Interface for the TestPopupChoiceDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPopupChoiceDialog
#define _H_TestPopupChoiceDialog

#include <jx-af/jx/JXDialogDirector.h>

class TestPopupChoiceDialog : public JXDialogDirector
{
public:

	TestPopupChoiceDialog(JXWindowDirector* supervisor);

	virtual ~TestPopupChoiceDialog();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();
};

#endif
