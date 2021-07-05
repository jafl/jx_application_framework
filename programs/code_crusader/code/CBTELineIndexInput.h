/******************************************************************************
 CBTELineIndexInput.h

	Interface for the CBTELineIndexInput class

	Copyright © 1997 by John Lindal.

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

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	virtual void	Act(JXTEBase* te, const JIndex value) override;
	virtual JIndex	GetValue(JXTEBase* te) const override;
	virtual JIndex	GetValue(const JTextEditor::CaretLocationChanged& info) const override;

private:

	CBTECaretInputBase*	itsColInput;

private:

	// not allowed

	CBTELineIndexInput(const CBTELineIndexInput& source);
	const CBTELineIndexInput& operator=(const CBTELineIndexInput& source);
};

#endif
