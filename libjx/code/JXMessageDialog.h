/******************************************************************************
 JXMessageDialog.h

	Interface for the JXMessageDialog class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXMessageDialog
#define _H_JXMessageDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
