/******************************************************************************
 MDPrefsDialog.h

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_MDPrefsDialog
#define _H_MDPrefsDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

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

	// not allowed

	MDPrefsDialog(const MDPrefsDialog& source);
	const MDPrefsDialog& operator=(const MDPrefsDialog& source);
};

#endif
