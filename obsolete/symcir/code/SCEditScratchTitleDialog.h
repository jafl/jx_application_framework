/******************************************************************************
 SCEditScratchTitleDialog.h

	Interface for the SCEditScratchTitleDialog class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCEditScratchTitleDialog
#define _H_SCEditScratchTitleDialog

#include <JXDialogDirector.h>

class JXInputField;

class SCEditScratchTitleDialog : public JXDialogDirector
{
public:

	SCEditScratchTitleDialog(JXWindowDirector* supervisor);

	virtual ~SCEditScratchTitleDialog();

protected:

	virtual JBoolean	OKToDeactivate();

private:

	JXWindow*	itsWindowToEdit;

// begin JXLayout

	JXInputField* itsTitle;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* title);

	// not allowed

	SCEditScratchTitleDialog(const SCEditScratchTitleDialog& source);
	const SCEditScratchTitleDialog& operator=(const SCEditScratchTitleDialog& source);
};

#endif
