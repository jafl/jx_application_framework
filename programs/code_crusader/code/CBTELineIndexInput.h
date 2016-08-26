/******************************************************************************
 CBTELineIndexInput.h

	Interface for the CBTELineIndexInput class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBTELineIndexInput
#define _H_CBTELineIndexInput

#include "CBTECaretInputBase.h"

class CBTELineIndexInput : public CBTECaretInputBase
{
public:

	CBTELineIndexInput(JXStaticText* label, JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~CBTELineIndexInput();

	void	SetTE(JXTEBase* te, CBTECaretInputBase* colInput);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	Act(JXTEBase* te, const JIndex value);
	virtual JIndex	GetValue(JXTEBase* te) const;
	virtual JIndex	GetValue(const JTextEditor::CaretLocationChanged& info) const;

private:

	CBTECaretInputBase*	itsColInput;

private:

	// not allowed

	CBTELineIndexInput(const CBTELineIndexInput& source);
	const CBTELineIndexInput& operator=(const CBTELineIndexInput& source);
};

#endif
