/******************************************************************************
 AboutDialog.h

	Interface for the AboutDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_AboutDialog
#define _H_AboutDialog

#include <jx-af/jx/JXDialogDirector.h>

class AboutDialog : public JXDialogDirector
{
public:

	AboutDialog(JXDirector* supervisor);

	~AboutDialog();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();
};

#endif
