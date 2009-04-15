/******************************************************************************
 AboutDialog.h

	Interface for the AboutDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_AboutDialog
#define _H_AboutDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
