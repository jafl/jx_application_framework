/******************************************************************************
 PrefsDialog.h

	Copyright (C) <Year> by <Company>.

 ******************************************************************************/

#ifndef _H_PrefsDialog
#define _H_PrefsDialog

#include <jx-af/jx/JXDialogDirector.h>

class PrefsDialog : public JXDialogDirector
{
public:

	PrefsDialog(JXDirector* supervisor, const JString& data);

	virtual ~PrefsDialog();

	void	GetValues(JString* data) const;

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& data);
};

#endif
