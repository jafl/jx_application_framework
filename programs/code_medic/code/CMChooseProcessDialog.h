/******************************************************************************
 CMChooseProcessDialog.h

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMChooseProcessDialog
#define _H_CMChooseProcessDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	virtual void	Receive(JBroadcaster* sender, const Message& message);

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
