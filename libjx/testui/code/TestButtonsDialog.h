/******************************************************************************
 TestButtonsDialog.h

	Interface for the TestButtonsDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestButtonsDialog
#define _H_TestButtonsDialog

#include <JXDialogDirector.h>

class JXRadioGroup;
class JXTextCheckbox;

class TestButtonsDialog : public JXDialogDirector
{
public:

	TestButtonsDialog(JXWindowDirector* supervisor);

	virtual ~TestButtonsDialog();

protected:

	virtual bool	OKToDeactivate() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXRadioGroup*   itsRG1;
	JXRadioGroup*   itsRG2;
	JXTextCheckbox* itsEnable1CB;
	JXTextCheckbox* its1CB;
	JXTextCheckbox* itsEnable2CB;
	JXTextCheckbox* its2CB;
	JXTextCheckbox* itsEnable3CB;
	JXTextCheckbox* its3CB;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	TestButtonsDialog(const TestButtonsDialog& source);
	const TestButtonsDialog& operator=(const TestButtonsDialog& source);
};

#endif
