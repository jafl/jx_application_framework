/******************************************************************************
 CBTEColIndexInput.h

	Interface for the CBTEColIndexInput class

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBTEColIndexInput
#define _H_CBTEColIndexInput

#include "CBTECaretInputBase.h"

class CBTELineIndexInput;

class CBTEColIndexInput : public CBTECaretInputBase
{
public:

	CBTEColIndexInput(CBTELineIndexInput* lineInput,
					  JXStaticText* label, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~CBTEColIndexInput();

protected:

	virtual void	Act(JXTEBase* te, const JIndex value);
	virtual JIndex	GetValue(JXTEBase* te) const;
	virtual JIndex	GetValue(const JTextEditor::CaretLocationChanged& info) const;

private:

	CBTELineIndexInput*	itsLineInput;	// not owned

private:

	// not allowed

	CBTEColIndexInput(const CBTEColIndexInput& source);
	const CBTEColIndexInput& operator=(const CBTEColIndexInput& source);
};

#endif
