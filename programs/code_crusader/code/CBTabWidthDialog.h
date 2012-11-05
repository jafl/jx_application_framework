/******************************************************************************
 CBTabWidthDialog.h

	Copyright © 2004 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBTabWidthDialog
#define _H_CBTabWidthDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXIntegerInput;

class CBTabWidthDialog : public JXDialogDirector
{
public:

	CBTabWidthDialog(JXWindowDirector* supervisor, const JSize tabWidth);

	virtual ~CBTabWidthDialog();

	JSize	GetTabCharCount() const;

private:

// begin JXLayout

	JXIntegerInput* itsTabWidthInput;

// end JXLayout

private:

	void	BuildWindow(const JSize tabWidth);

	// not allowed

	CBTabWidthDialog(const CBTabWidthDialog& source);
	const CBTabWidthDialog& operator=(const CBTabWidthDialog& source);
};

#endif
