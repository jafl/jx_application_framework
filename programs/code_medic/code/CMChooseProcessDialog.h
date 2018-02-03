/******************************************************************************
 CMChooseProcessDialog.h

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_CMChooseProcessDialog
#define _H_CMChooseProcessDialog

#include <JXDialogDirector.h>

class JXIntegerInput;
class CMProcessText;

class CMChooseProcessDialog : public JXDialogDirector
{
public:

	CMChooseProcessDialog(JXDirector* supervisor,
						  const JBoolean attachToSelection = kJTrue,
						  const JBoolean stopProgram = kJFalse);

	virtual	~CMChooseProcessDialog();

	void	SetProcessID(const JInteger value);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CMProcessText*	itsText;
	const JBoolean	itsAttachToSelectionFlag;
	const JBoolean	itsStopProgramFlag;

// begin JXLayout

	JXIntegerInput* itsProcessIDInput;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	CMChooseProcessDialog(const CMChooseProcessDialog& source);
	const CMChooseProcessDialog& operator=(const CMChooseProcessDialog& source);
};

#endif
