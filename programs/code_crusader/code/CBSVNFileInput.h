/******************************************************************************
 CBSVNFileInput.h

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBSVNFileInput
#define _H_CBSVNFileInput

#include <JXFileInput.h>

class CBSVNFileInput : public JXFileInput
{
public:

	CBSVNFileInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~CBSVNFileInput();

protected:

	virtual void	AdjustStylesBeforeRecalc(const JString& buffer, JRunArray<JFont>* styles,
											 JIndexRange* recalcRange, JIndexRange* redrawRange,
											 const JBoolean deletion);

private:

	// not allowed

	CBSVNFileInput(const CBSVNFileInput& source);
	const CBSVNFileInput& operator=(const CBSVNFileInput& source);
};

#endif
