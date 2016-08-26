/******************************************************************************
 SCAboutDialog.h

	Interface for the SCAboutDialog class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCAboutDialog
#define _H_SCAboutDialog

#include <JXDialogDirector.h>

class SCAboutDialog : public JXDialogDirector
{
public:

	SCAboutDialog(JXDirector* supervisor);

	virtual ~SCAboutDialog();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	SCAboutDialog(const SCAboutDialog& source);
	const SCAboutDialog& operator=(const SCAboutDialog& source);
};

#endif
