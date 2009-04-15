/******************************************************************************
 JXSlider.h

	Interface for the JXSlider class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXSlider
#define _H_JXSlider

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXSliderBase.h>

class JXSlider : public JXSliderBase
{
public:

	JXSlider(JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	virtual ~JXSlider();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

private:

	// not allowed

	JXSlider(const JXSlider& source);
	const JXSlider& operator=(const JXSlider& source);
};

#endif
