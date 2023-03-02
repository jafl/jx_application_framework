/******************************************************************************
 AboutDialog.h

	Interface for the AboutDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_AboutDialog
#define _H_AboutDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class AboutDialog : public JXModalDialogDirector
{
public:

	AboutDialog();

	~AboutDialog() override;

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();
};

#endif
