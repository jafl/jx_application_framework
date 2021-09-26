/******************************************************************************
 MDPrefsDialog.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_MDPrefsDialog
#define _H_MDPrefsDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXInputField;

class MDPrefsDialog : public JXDialogDirector
{
public:

	MDPrefsDialog(JXDirector* supervisor, const JString& openCmd);

	virtual	~MDPrefsDialog();

	void	GetValues(JString* openCmd) const;

private:

// begin JXLayout

	JXInputField* itsOpenFileInput;

// end JXLayout

private:

	void	BuildWindow(const JString& openCmd);
};

#endif
