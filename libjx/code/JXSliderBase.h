/******************************************************************************
 JXSliderBase.h

	Interface for the JXSliderBase class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSliderBase
#define _H_JXSliderBase

#include "jx-af/jx/JXWidget.h"
#include <jx-af/jcore/JSliderBase.h>

class JXSliderBase : public JXWidget, public JSliderBase
{
public:

	JXSliderBase(const JCoordinate thumbHalfSize, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXSliderBase();

protected:

	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, 
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	void		SliderRedraw() const override;
	JCoordinate	SliderGetWidth() const override;
	JCoordinate	SliderGetHeight() const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;
};

#endif
