/******************************************************************************
 TestInputFieldsDialog.h

	Interface for the TestInputFieldsDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestInputFieldsDialog
#define _H_TestInputFieldsDialog

#include <JXDialogDirector.h>

class JXIntegerInput;

class TestInputFieldsDialog : public JXDialogDirector
{
public:

	TestInputFieldsDialog(JXWindowDirector* supervisor);

	virtual ~TestInputFieldsDialog();

protected:

	virtual bool	OKToDeactivate();

private:

// begin JXLayout

	JXIntegerInput* itsLowerValue;
	JXIntegerInput* itsUpperValue;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
