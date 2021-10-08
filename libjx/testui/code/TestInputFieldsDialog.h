/******************************************************************************
 TestInputFieldsDialog.h

	Interface for the TestInputFieldsDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestInputFieldsDialog
#define _H_TestInputFieldsDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXIntegerInput;

class TestInputFieldsDialog : public JXDialogDirector
{
public:

	TestInputFieldsDialog(JXWindowDirector* supervisor);

	~TestInputFieldsDialog() override;

protected:

	bool	OKToDeactivate() override;

private:

// begin JXLayout

	JXIntegerInput* itsLowerValue;
	JXIntegerInput* itsUpperValue;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
