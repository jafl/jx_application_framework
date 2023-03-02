/******************************************************************************
 JXAcceptLicenseDialog.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXAcceptLicenseDialog
#define _H_JXAcceptLicenseDialog

#include "JXModalDialogDirector.h"

class JXAcceptLicenseDialog : public JXModalDialogDirector
{
public:

	JXAcceptLicenseDialog();

	~JXAcceptLicenseDialog() override;

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();
};

#endif
