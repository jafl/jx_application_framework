/******************************************************************************
 TestInputFieldsDialog.h

	Interface for the TestInputFieldsDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestInputFieldsDialog
#define _H_TestInputFieldsDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXIntegerInput;

class TestInputFieldsDialog : public JXDialogDirector
{
public:

	TestInputFieldsDialog(JXWindowDirector* supervisor);

	virtual ~TestInputFieldsDialog();

protected:

	virtual JBoolean	OKToDeactivate();

private:

// begin JXLayout

    JXIntegerInput* itsLowerValue;
    JXIntegerInput* itsUpperValue;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	TestInputFieldsDialog(const TestInputFieldsDialog& source);
	const TestInputFieldsDialog& operator=(const TestInputFieldsDialog& source);
};

#endif
