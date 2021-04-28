/******************************************************************************
 JXWarningDialog.h

	Interface for the JXWarningDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWarningDialog
#define _H_JXWarningDialog

#include "JXUNDialogBase.h"

class JXWarningDialog : public JXUNDialogBase
{
public:

	JXWarningDialog(JXDirector* supervisor,
					const JString& title, const JString& message);

	virtual ~JXWarningDialog();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& title, const JString& message);

	// not allowed

	JXWarningDialog(const JXWarningDialog& source);
	const JXWarningDialog& operator=(const JXWarningDialog& source);
};

#endif
