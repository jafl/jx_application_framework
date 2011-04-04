/******************************************************************************
 JXGoToLineDialog.h

	Interface for the JXGoToLineDialog class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXGoToLineDialog
#define _H_JXGoToLineDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXIntegerInput;
class JXTextCheckbox;

class JXGoToLineDialog : public JXDialogDirector
{
public:

	JXGoToLineDialog(JXDirector* supervisor,
					 const JIndex lineIndex, const JIndex maxLine,
					 const JBoolean physicalLineIndexFlag = kJFalse);

	virtual ~JXGoToLineDialog();

	JIndex	GetLineIndex(JBoolean* physicalLineIndexFlag) const;

private:

	const JSize	itsMaxLineCount;

// begin JXLayout

	JXIntegerInput* itsLineNumber;
	JXTextCheckbox* itsPhysicalLineIndexCB;

// end JXLayout

private:

	void	BuildWindow(const JIndex lineIndex, const JBoolean physicalLineIndexFlag);

	// not allowed

	JXGoToLineDialog(const JXGoToLineDialog& source);
	const JXGoToLineDialog& operator=(const JXGoToLineDialog& source);
};

#endif
