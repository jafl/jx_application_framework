/******************************************************************************
 JXMessageDialog.h

	Interface for the JXMessageDialog class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXMessageDialog
#define _H_JXMessageDialog

#include <JXUNDialogBase.h>

class JXMessageDialog : public JXUNDialogBase
{
public:

	JXMessageDialog(JXDirector* supervisor, const JCharacter* message);

	virtual ~JXMessageDialog();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JCharacter* message);

	// not allowed

	JXMessageDialog(const JXMessageDialog& source);
	const JXMessageDialog& operator=(const JXMessageDialog& source);
};

#endif
