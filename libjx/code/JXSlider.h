/******************************************************************************
 JXSlider.h

	Interface for the JXSlider class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSlider
#define _H_JXSlider

#include "JXSliderBase.h"

class JXSlider : public JXSliderBase
{
public:

	JXSlider(JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	~JXSlider() override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;
};

#endif
