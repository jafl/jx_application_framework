/******************************************************************************
 JXGoToLineDialog.h

	Interface for the JXGoToLineDialog class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXGoToLineDialog
#define _H_JXGoToLineDialog

#include "JXDialogDirector.h"

class JXIntegerInput;
class JXTextCheckbox;

class JXGoToLineDialog : public JXDialogDirector
{
public:

	JXGoToLineDialog(JXDirector* supervisor,
					 const JIndex lineIndex, const JIndex maxLine,
					 const bool physicalLineIndexFlag = false);

	virtual ~JXGoToLineDialog();

	JIndex	GetLineIndex(bool* physicalLineIndexFlag) const;

private:

	const JSize	itsMaxLineCount;

// begin JXLayout

	JXIntegerInput* itsLineNumber;
	JXTextCheckbox* itsPhysicalLineIndexCB;

// end JXLayout

private:

	void	BuildWindow(const JIndex lineIndex, const bool physicalLineIndexFlag);

	// not allowed

	JXGoToLineDialog(const JXGoToLineDialog& source);
	const JXGoToLineDialog& operator=(const JXGoToLineDialog& source);
};

#endif
