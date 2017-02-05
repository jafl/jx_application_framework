/******************************************************************************
 TestPopupChoiceDialog.h

	Interface for the TestPopupChoiceDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPopupChoiceDialog
#define _H_TestPopupChoiceDialog

#include <JXDialogDirector.h>

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

	// not allowed

	TestPopupChoiceDialog(const TestPopupChoiceDialog& source);
	const TestPopupChoiceDialog& operator=(const TestPopupChoiceDialog& source);
};

#endif
