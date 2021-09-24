/******************************************************************************
 <PRE>PrefsDialog.h

	Copyright (C) <Year> by <Company>.

 ******************************************************************************/

#ifndef _H_<PRE>PrefsDialog
#define _H_<PRE>PrefsDialog

#include <JXDialogDirector.h>

class <PRE>PrefsDialog : public JXDialogDirector
{
public:

	<PRE>PrefsDialog(JXDirector* supervisor, const JString& data);

	virtual	~<PRE>PrefsDialog();

	void	GetValues(JString* data) const;

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& data);
};

#endif
