/******************************************************************************
 PrefsDialog.h

	Copyright (C) <Year> by <Company>.

 ******************************************************************************/

#ifndef _H_PrefsDialog
#define _H_PrefsDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class PrefsDialog : public JXModalDialogDirector
{
public:

	PrefsDialog(const JString& data);

	~PrefsDialog() override;

	void	GetValues(JString* data) const;

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JString& data);
};

#endif
