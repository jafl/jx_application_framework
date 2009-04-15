/******************************************************************************
 JXWarningDialog.h

	Interface for the JXWarningDialog class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXWarningDialog
#define _H_JXWarningDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUNDialogBase.h>

class JXWarningDialog : public JXUNDialogBase
{
public:

	JXWarningDialog(JXDirector* supervisor, const JCharacter* message);

	virtual ~JXWarningDialog();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JCharacter* message);

	// not allowed

	JXWarningDialog(const JXWarningDialog& source);
	const JXWarningDialog& operator=(const JXWarningDialog& source);
};

#endif
