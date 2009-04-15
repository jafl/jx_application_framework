/******************************************************************************
 JXAcceptLicenseDialog.h

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXAcceptLicenseDialog
#define _H_JXAcceptLicenseDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

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

	// not allowed

	JXAcceptLicenseDialog(const JXAcceptLicenseDialog& source);
	const JXAcceptLicenseDialog& operator=(const JXAcceptLicenseDialog& source);
};

#endif
