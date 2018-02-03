/******************************************************************************
 JXSliderBase.h

	Interface for the JXSliderBase class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSliderBase
#define _H_JXSliderBase

#include <JXWidget.h>
#include <JSliderBase.h>

class JXSliderBase : public JXWidget, public JSliderBase
{
public:

	JXSliderBase(const JCoordinate thumbHalfSize, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXSliderBase();

protected:

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, 
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual void		SliderRedraw() const;
	virtual JCoordinate	SliderGetWidth() const;
	virtual JCoordinate	SliderGetHeight() const;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	// not allowed

	JXSliderBase(const JXSliderBase& source);
	const JXSliderBase& operator=(const JXSliderBase& source);
};

#endif
