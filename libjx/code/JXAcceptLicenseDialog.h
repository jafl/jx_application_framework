/******************************************************************************
 JXAcceptLicenseDialog.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXAcceptLicenseDialog
#define _H_JXAcceptLicenseDialog

#include "jx-af/jx/JXDialogDirector.h"

class JXAcceptLicenseDialog : public JXDialogDirector
{
public:

	JXAcceptLicenseDialog(JXDirector* supervisor);

	virtual ~JXAcceptLicenseDialog();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();
};

#endif
