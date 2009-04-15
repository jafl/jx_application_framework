/******************************************************************************
 JXFlatRect.h

	Interface for the JXFlatRect class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFlatRect
#define _H_JXFlatRect

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDecorRect.h>

class JXFlatRect : public JXDecorRect
{
public:

	JXFlatRect(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~JXFlatRect();

protected:

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

private:

	// not allowed

	JXFlatRect(const JXFlatRect& source);
	const JXFlatRect& operator=(const JXFlatRect& source);
};

#endif
