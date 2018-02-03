/******************************************************************************
 CMCommandOutputDisplay.h

	Interface for the CMCommandOutputDisplay class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_CMCommandOutputDisplay
#define _H_CMCommandOutputDisplay

#include "CMTextDisplayBase.h"

class CMCommandOutputDisplay : public CMTextDisplayBase
{
public:

	CMCommandOutputDisplay(JXMenuBar* menuBar,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual	~CMCommandOutputDisplay();

	void	PlaceCursorAtEnd();

private:

	// not allowed

	CMCommandOutputDisplay(const CMCommandOutputDisplay& source);
	const CMCommandOutputDisplay& operator=(const CMCommandOutputDisplay& source);
};

#endif
