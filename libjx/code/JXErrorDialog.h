/******************************************************************************
 JXErrorDialog.h

	Interface for the JXErrorDialog class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXErrorDialog
#define _H_JXErrorDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUNDialogBase.h>

class JXErrorDialog : public JXUNDialogBase
{
public:

	JXErrorDialog(JXDirector* supervisor, const JCharacter* message);

	virtual ~JXErrorDialog();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JCharacter* message);

	// not allowed

	JXErrorDialog(const JXErrorDialog& source);
	const JXErrorDialog& operator=(const JXErrorDialog& source);
};

#endif
