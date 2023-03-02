/******************************************************************************
 JXMessageDialog.h

	Interface for the JXMessageDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXMessageDialog
#define _H_JXMessageDialog

#include "JXUNDialogBase.h"

class JXMessageDialog : public JXUNDialogBase
{
public:

	JXMessageDialog(const JString& message);

	~JXMessageDialog() override;

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& message);
};

#endif
