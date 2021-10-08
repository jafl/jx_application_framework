/******************************************************************************
 JXWarningDialog.h

	Interface for the JXWarningDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWarningDialog
#define _H_JXWarningDialog

#include "jx-af/jx/JXUNDialogBase.h"

class JXWarningDialog : public JXUNDialogBase
{
public:

	JXWarningDialog(JXDirector* supervisor,
					const JString& title, const JString& message);

	~JXWarningDialog();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& title, const JString& message);
};

#endif
