/******************************************************************************
 JXMessageDialog.h

	Interface for the JXMessageDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXMessageDialog
#define _H_JXMessageDialog

#include "jx-af/jx/JXUNDialogBase.h"

class JXMessageDialog : public JXUNDialogBase
{
public:

	JXMessageDialog(JXDirector* supervisor, const JString& message);

	~JXMessageDialog();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& message);
};

#endif
