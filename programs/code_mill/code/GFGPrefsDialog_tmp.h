/******************************************************************************
 GFGPrefsDialog.h

	Interface for the GFGPrefsDialog class

	Copyright (C) 2002 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GFGPrefsDialog
#define _H_GFGPrefsDialog

#include <JXDialogDirector.h>

class GFGPrefsDialog : public JXDialogDirector
{
public:

	GFGPrefsDialog()
	virtual ~GFGPrefsDialog();



protected:

	virtual bool	OKToDeactivate();


private:

	GFGPrefsDialog(const GFGPrefsDialog& source);
	const GFGPrefsDialog& operator=(const GFGPrefsDialog& source);


};

#endif
