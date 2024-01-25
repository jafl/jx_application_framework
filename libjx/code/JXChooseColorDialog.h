/******************************************************************************
 JXChooseColorDialog.h

	Interface for the JXChooseColorDialog class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXChooseColorDialog
#define _H_JXChooseColorDialog

#include "JXModalDialogDirector.h"
#include <jx-af/jcore/jColor.h>

class JXSlider;
class JXIntegerInput;
class JXFlatRect;
class JXColorWheel;

class JXChooseColorDialog : public JXModalDialogDirector
{
public:

	JXChooseColorDialog(const JColorID colorIndex);

	~JXChooseColorDialog() override;

	JColorID	GetColor() const;

protected:

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXColorWheel*   itsColorWheel;
	JXFlatRect*     itsColorSample;
	JXSlider*       itsHSlider;
	JXSlider*       itsSSlider;
	JXSlider*       itsVSlider;
	JXIntegerInput* itsRInput;
	JXIntegerInput* itsGInput;
	JXIntegerInput* itsBInput;

// end JXLayout

private:

	void	BuildWindow(const JColorID colorIndex);
	void	UpdateWheelColor();
	void	UpdateHSBColor();
	void	UpdateRGBColor();
	void	UpdateColorWheel(const JRGB& color);
	void	UpdateSliders(const JRGB& color);
	void	UpdateInputFields(const JRGB& color);
};

#endif
