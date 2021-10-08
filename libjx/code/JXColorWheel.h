/******************************************************************************
 JXColorWheel.h

	Interface for the JXColorWheel class

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXColorWheel
#define _H_JXColorWheel

#include "jx-af/jx/JXWidget.h"
#include <jx-af/jcore/jColor.h>

class JXSlider;
class JXImage;

class JXColorWheel : public JXWidget
{
public:

	JXColorWheel(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	~JXColorWheel();

	JRGB	GetRGB() const;
	void	SetColor(const JRGB& rgb);

	JHSB	GetHSB() const;
	void	SetColor(const JHSB& hsb);

protected:

	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, 
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JHSB		itsColor;
	JXSlider*	itsBrightnessSlider;
	JXImage*	itsImage;
	JSize		itsLastDrawBrightness;

private:

	void	SetColor(const JPoint& pt);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kColorChanged;

	class ColorChanged : public JBroadcaster::Message
		{
		public:

			ColorChanged()
				:
				JBroadcaster::Message(kColorChanged)
				{ };
		};
};


/******************************************************************************
 RGB

 ******************************************************************************/

inline JRGB
JXColorWheel::GetRGB()
	const
{
	return JRGB(itsColor);
}

inline void
JXColorWheel::SetColor
	(
	const JRGB& rgb
	)
{
	JHSB hsb(rgb);
	SetColor(hsb);
}

/******************************************************************************
 HSB

 ******************************************************************************/

inline JHSB
JXColorWheel::GetHSB()
	const
{
	return itsColor;
}

#endif
