/******************************************************************************
 CBProjectFileInput.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBProjectFileInput
#define _H_CBProjectFileInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXFileInput.h>

class CBProjectFileInput : public JXFileInput
{
public:

	CBProjectFileInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~CBProjectFileInput();

protected:

	virtual void	AdjustStylesBeforeRecalc(const JString& buffer, JRunArray<Font>* styles,
											 JIndexRange* recalcRange, JIndexRange* redrawRange,
											 const JBoolean deletion);

private:

	// not allowed

	CBProjectFileInput(const CBProjectFileInput& source);
	const CBProjectFileInput& operator=(const CBProjectFileInput& source);
};

#endif
