/******************************************************************************
 PrefsDialog.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_PrefsDialog
#define _H_PrefsDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXInputField;

class PrefsDialog : public JXDialogDirector
{
public:

	PrefsDialog(JXDirector* supervisor, const JString& openCmd);

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
