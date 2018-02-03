/******************************************************************************
 MDPrefsDialog.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_MDPrefsDialog
#define _H_MDPrefsDialog

#include <JXDialogDirector.h>

class MDPrefsDialog : public JXDialogDirector
{
public:

	MDPrefsDialog(JXDirector* supervisor, const JString& data);

	virtual	~MDPrefsDialog();

	void	GetValues(JString* data) const;

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& data);

	// not allowed

	MDPrefsDialog(const MDPrefsDialog& source);
	const MDPrefsDialog& operator=(const MDPrefsDialog& source);
};

#endif
