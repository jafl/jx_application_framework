/******************************************************************************
 JXUpRect.h

	Interface for the JXUpRect class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXUpRect
#define _H_JXUpRect

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDecorRect.h>

class JXUpRect : public JXDecorRect
{
public:

	JXUpRect(JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	virtual ~JXUpRect();

protected:

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

private:

	// not allowed

	JXUpRect(const JXUpRect& source);
	const JXUpRect& operator=(const JXUpRect& source);
};

#endif
