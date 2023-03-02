/******************************************************************************
 PrefsDialog.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_PrefsDialog
#define _H_PrefsDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXInputField;

class PrefsDialog : public JXModalDialogDirector
{
public:

	PrefsDialog(const JString& openCmd);

	~PrefsDialog() override;

	void	GetValues(JString* openCmd) const;

private:

// begin JXLayout

	JXInputField* itsOpenFileInput;

// end JXLayout

private:

	void	BuildWindow(const JString& openCmd);
};

#endif
