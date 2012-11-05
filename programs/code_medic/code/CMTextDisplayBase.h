/******************************************************************************
 CMTextDisplayBase.h

	Interface for the CMTextDisplayBase class

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMTextDisplayBase
#define _H_CMTextDisplayBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTEBase.h>

class CMTextDisplayBase : public JXTEBase
{
public:

	CMTextDisplayBase(const Type type, const JBoolean breakCROnly,
					  JXMenuBar* menuBar,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~CMTextDisplayBase();

	static void	AdjustFont(JXTEBase* te);

private:

	// not allowed

	CMTextDisplayBase(const CMTextDisplayBase& source);
	const CMTextDisplayBase& operator=(const CMTextDisplayBase& source);
};

#endif
