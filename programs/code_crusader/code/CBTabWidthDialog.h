/******************************************************************************
 CBTabWidthDialog.h

	Copyright © 2004 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBTabWidthDialog
#define _H_CBTabWidthDialog

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
