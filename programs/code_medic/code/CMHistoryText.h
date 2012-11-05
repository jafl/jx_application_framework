/******************************************************************************
 CMHistoryText.h

	Interface for the CMHistoryText class

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMHistoryText
#define _H_CMHistoryText

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMTextDisplayBase.h"

class CMHistoryText : public CMTextDisplayBase
{
public:

	CMHistoryText(JXMenuBar* menuBar,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual	~CMHistoryText();

	void	PlaceCursorAtEnd();

private:

	// not allowed

	CMHistoryText(const CMHistoryText& source);
	const CMHistoryText& operator=(const CMHistoryText& source);
};

#endif
