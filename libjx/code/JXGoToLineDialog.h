/******************************************************************************
 JXGoToLineDialog.h

	Interface for the JXGoToLineDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXGoToLineDialog
#define _H_JXGoToLineDialog

#include "JXModalDialogDirector.h"

class JXIntegerInput;
class JXTextCheckbox;

class JXGoToLineDialog : public JXModalDialogDirector
{
public:

	JXGoToLineDialog(const JIndex lineIndex, const JIndex maxLine,
					 const bool physicalLineIndexFlag = false);

	~JXGoToLineDialog() override;

	JIndex	GetLineIndex(bool* physicalLineIndexFlag) const;

private:

	const JSize	itsMaxLineCount;

// begin JXLayout

	JXTextCheckbox* itsPhysicalLineIndexCB;
	JXIntegerInput* itsLineNumber;

// end JXLayout

private:

	void	BuildWindow(const JIndex lineIndex, const bool physicalLineIndexFlag);
};

#endif
