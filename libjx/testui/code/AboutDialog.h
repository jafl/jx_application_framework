/******************************************************************************
 AboutDialog.h

	Interface for the AboutDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_AboutDialog
#define _H_AboutDialog

#include <JXDialogDirector.h>

class AboutDialog : public JXDialogDirector
{
public:

	AboutDialog(JXDirector* supervisor);

	virtual ~AboutDialog();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	AboutDialog(const AboutDialog& source);
	const AboutDialog& operator=(const AboutDialog& source);
};

#endif
