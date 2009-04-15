/******************************************************************************
 JXDownRect.h

	Interface for the JXDownRect class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDownRect
#define _H_JXDownRect

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDecorRect.h>

class JXDownRect : public JXDecorRect
{
public:

	JXDownRect(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~JXDownRect();

protected:

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

private:

	// not allowed

	JXDownRect(const JXDownRect& source);
	const JXDownRect& operator=(const JXDownRect& source);
};

#endif
