/******************************************************************************
 JXLevelControl.h

	Interface for the JXLevelControl class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXLevelControl
#define _H_JXLevelControl

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXSliderBase.h>

class JXLevelControl : public JXSliderBase
{
public:

	JXLevelControl(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~JXLevelControl();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);	
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

private:

	// not allowed

	JXLevelControl(const JXLevelControl& source);
	const JXLevelControl& operator=(const JXLevelControl& source);
};

#endif
