/******************************************************************************
 JXChooseFontSizeDialog.h

	Interface for the JXChooseFontSizeDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXChooseFontSizeDialog
#define _H_JXChooseFontSizeDialog

#include "JXModalDialogDirector.h"

class JXIntegerInput;

class JXChooseFontSizeDialog : public JXModalDialogDirector
{
public:

	JXChooseFontSizeDialog(const JSize fontSize);

	~JXChooseFontSizeDialog() override;

	JSize	GetFontSize() const;

private:

// begin JXLayout

	JXIntegerInput* itsFontSize;

// end JXLayout

private:

	void	BuildWindow(const JSize fontSize);
};

#endif
