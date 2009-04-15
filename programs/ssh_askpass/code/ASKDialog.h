/******************************************************************************
 ASKDialog.h

	Copyright © 2006 by New Planet Software, Inc.. All rights reserved.

 *****************************************************************************/

#ifndef _H_ASKDialog
#define _H_ASKDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXPasswordInput;

class ASKDialog : public JXDialogDirector
{
public:

	ASKDialog(JXDirector* supervisor);

	virtual	~ASKDialog();

protected:

	virtual JBoolean	OKToDeactivate();

private:

// begin JXLayout

    JXPasswordInput* itsPassphraseInput;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	ASKDialog(const ASKDialog& source);
	const ASKDialog& operator=(const ASKDialog& source);
};

#endif
