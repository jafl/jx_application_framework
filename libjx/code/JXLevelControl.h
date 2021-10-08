/******************************************************************************
 JXLevelControl.h

	Interface for the JXLevelControl class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXLevelControl
#define _H_JXLevelControl

#include "jx-af/jx/JXSliderBase.h"

class JXLevelControl : public JXSliderBase
{
public:

	JXLevelControl(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	~JXLevelControl() override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;	
	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
};

#endif
