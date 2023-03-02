/******************************************************************************
 JXErrorDialog.h

	Interface for the JXErrorDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXErrorDialog
#define _H_JXErrorDialog

#include "JXUNDialogBase.h"

class JXErrorDialog : public JXUNDialogBase
{
public:

	JXErrorDialog(const JString& message);

	~JXErrorDialog() override;

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& message);
};

#endif
