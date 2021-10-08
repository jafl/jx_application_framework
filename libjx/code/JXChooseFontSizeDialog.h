/******************************************************************************
 JXChooseFontSizeDialog.h

	Interface for the JXChooseFontSizeDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXChooseFontSizeDialog
#define _H_JXChooseFontSizeDialog

#include "jx-af/jx/JXDialogDirector.h"

class JXIntegerInput;

class JXChooseFontSizeDialog : public JXDialogDirector
{
public:

	JXChooseFontSizeDialog(JXWindowDirector* supervisor, const JSize fontSize);

	~JXChooseFontSizeDialog();

	JSize	GetFontSize() const;

private:

// begin JXLayout

	JXIntegerInput* itsFontSize;

// end JXLayout

private:

	void	BuildWindow(const JSize fontSize);
};

#endif
