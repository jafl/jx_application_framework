/******************************************************************************
 MDPrefsDialog.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_MDPrefsDialog
#define _H_MDPrefsDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
