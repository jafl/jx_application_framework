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

	JXWarningDialog(const JString& title, const JString& message);

	~JXWarningDialog() override;

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& title, const JString& message);
};

#endif
