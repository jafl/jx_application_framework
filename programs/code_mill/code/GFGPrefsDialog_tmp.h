/******************************************************************************
 GFGPrefsDialog.h

	Interface for the GFGPrefsDialog class

	Copyright © 2002 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#ifndef _H_GFGPrefsDialog
#define _H_GFGPrefsDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class GFGPrefsDialog : public JXDialogDirector
{
public:

	GFGPrefsDialog()
	virtual ~GFGPrefsDialog();



protected:

	virtual JBoolean	OKToDeactivate();


private:

	GFGPrefsDialog(const GFGPrefsDialog& source);
	const GFGPrefsDialog& operator=(const GFGPrefsDialog& source);


};

#endif
