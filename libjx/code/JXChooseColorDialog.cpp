/******************************************************************************
 JXChooseColorDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JXChooseColorDialog.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXColorWheel.h"
#include "JXSlider.h"
#include "JXIntegerInput.h"
#include "JXFlatRect.h"
#include "JXStaticText.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXChooseColorDialog::JXChooseColorDialog
	(
	const JColorID colorIndex
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(colorIndex);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXChooseColorDialog::~JXChooseColorDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
JXChooseColorDialog::BuildWindow
	(
	const JColorID colorIndex
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 510,250, JGetString("WindowTitle::JXChooseColorDialog::JXLayout"));

	itsColorWheel =
		jnew JXColorWheel(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 250,220);

	itsColorSample =
		jnew JXFlatRect(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 400,20, 30,30);
	itsColorSample->SetBackColor(JColorManager::GetColorID(JRGB(52428, 52428, 52428)));

	auto* sampleLabel =
		jnew JXStaticText(JGetString("sampleLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 340,25, 60,20);
	sampleLabel->SetToLabel(false);

	auto* hueLabel =
		jnew JXStaticText(JGetString("hueLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,70, 50,20);
	hueLabel->SetToLabel(false);

	itsHSlider =
		jnew JXSlider(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 340,70, 150,20);

	auto* saturationLabel =
		jnew JXStaticText(JGetString("saturationLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,90, 50,20);
	saturationLabel->SetToLabel(false);

	itsSSlider =
		jnew JXSlider(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 340,90, 150,20);

	auto* brightnessLabel =
		jnew JXStaticText(JGetString("brightnessLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,110, 50,20);
	brightnessLabel->SetToLabel(false);

	itsVSlider =
		jnew JXSlider(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 340,110, 150,20);

	auto* redLabel =
		jnew JXStaticText(JGetString("redLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,140, 50,20);
	redLabel->SetToLabel(false);

	auto* redRangeLabel =
		jnew JXStaticText(JGetString("redRangeLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 340,140, 80,20);
	redRangeLabel->SetToLabel(false);

	auto* greenLabel =
		jnew JXStaticText(JGetString("greenLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,160, 50,20);
	greenLabel->SetToLabel(false);

	auto* greenRangeLabel =
		jnew JXStaticText(JGetString("greenRangeLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 340,160, 80,20);
	greenRangeLabel->SetToLabel(false);

	auto* blueLabel =
		jnew JXStaticText(JGetString("blueLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,180, 50,20);
	blueLabel->SetToLabel(false);

	auto* blueRangeLabel =
		jnew JXStaticText(JGetString("blueRangeLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 340,180, 80,20);
	blueRangeLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,220, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,220, 60,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::JXChooseColorDialog::JXLayout"));

	itsRInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 420,140, 70,20);
	itsRInput->SetIsRequired(false);

	itsGInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 420,160, 70,20);
	itsGInput->SetIsRequired(false);

	itsBInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 420,180, 70,20);
	itsBInput->SetIsRequired(false);

// end JXLayout

	SetButtons(okButton, cancelButton);

	const JRGB rgb = JColorManager::GetRGB(colorIndex);
	const JHSB hsb(rgb);

	itsHSlider->SetRange(0.0, kJMaxHSBValueF);
	itsHSlider->SetValue(hsb.hue);
	ListenTo(itsHSlider);

	itsSSlider->SetRange(0.0, kJMaxHSBValueF);
	itsSSlider->SetValue(hsb.saturation);
	ListenTo(itsSSlider);

	itsVSlider->SetRange(0.0, kJMaxHSBValueF);
	itsVSlider->SetValue(hsb.brightness);
	ListenTo(itsVSlider);

	itsRInput->SetIsRequired();
	itsRInput->SetLimits(0, kJMaxRGBValue);
	itsRInput->SetValue(rgb.red);
	ListenTo(itsRInput);

	itsGInput->SetIsRequired();
	itsGInput->SetLimits(0, kJMaxRGBValue);
	itsGInput->SetValue(rgb.green);
	ListenTo(itsGInput);

	itsBInput->SetIsRequired();
	itsBInput->SetLimits(0, kJMaxRGBValue);
	itsBInput->SetValue(rgb.blue);
	ListenTo(itsBInput);

	itsColorWheel->SetColor(hsb);
	ListenTo(itsColorWheel);

	itsColorSample->SetBackColor(colorIndex);
}

/******************************************************************************
 GetColor

 ******************************************************************************/

JColorID
JXChooseColorDialog::GetColor()
	const
{
	return itsColorSample->GetBackColor();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXChooseColorDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsColorWheel && message.Is(JXColorWheel::kColorChanged))
	{
		UpdateWheelColor();
	}

	else if (message.Is(JSliderBase::kMoved))
	{
		UpdateHSBColor();
	}
	else if (message.Is(JXWidget::kLostFocus))
	{
		UpdateRGBColor();
	}

	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateWheelColor (private)

 ******************************************************************************/

void
JXChooseColorDialog::UpdateWheelColor()
{
	JRGB newColor = itsColorWheel->GetRGB();
	UpdateSliders(newColor);
	UpdateInputFields(newColor);

	const JColorID newColorID = JColorManager::GetColorID(newColor);
	itsColorSample->SetBackColor(newColorID);
}

/******************************************************************************
 UpdateHSBColor (private)

 ******************************************************************************/

void
JXChooseColorDialog::UpdateHSBColor()
{
	JRGB newColor = JHSB(JRound(itsHSlider->GetValue()),
						 JRound(itsSSlider->GetValue()),
						 JRound(itsVSlider->GetValue()));
	UpdateColorWheel(newColor);
	UpdateInputFields(newColor);

	const JColorID newColorID = JColorManager::GetColorID(newColor);
	itsColorSample->SetBackColor(newColorID);
}

/******************************************************************************
 UpdateRGBColor (private)

 ******************************************************************************/

void
JXChooseColorDialog::UpdateRGBColor()
{
	JInteger r,g,b;
	bool ok = itsRInput->GetValue(&r);
	assert( ok );
	ok = itsGInput->GetValue(&g);
	assert( ok );
	ok = itsBInput->GetValue(&b);
	assert( ok );

	JRGB newColor(r,g,b);
	UpdateColorWheel(newColor);
	UpdateSliders(newColor);

	const JColorID newColorID = JColorManager::GetColorID(newColor);
	itsColorSample->SetBackColor(newColorID);
}

/******************************************************************************
 UpdateColorWheel (private)

 ******************************************************************************/

void
JXChooseColorDialog::UpdateColorWheel
	(
	const JRGB& color
	)
{
	StopListening(itsColorWheel);
	itsColorWheel->SetColor(color);
	ListenTo(itsColorWheel);
}

/******************************************************************************
 UpdateSliders (private)

 ******************************************************************************/

void
JXChooseColorDialog::UpdateSliders
	(
	const JRGB& color
	)
{
	StopListening(itsHSlider);
	StopListening(itsSSlider);
	StopListening(itsVSlider);

	JHSB hsb(color);
	itsHSlider->SetValue(hsb.hue);
	itsSSlider->SetValue(hsb.saturation);
	itsVSlider->SetValue(hsb.brightness);

	ListenTo(itsHSlider);
	ListenTo(itsSSlider);
	ListenTo(itsVSlider);
}

/******************************************************************************
 UpdateInputFields (private)

 ******************************************************************************/

void
JXChooseColorDialog::UpdateInputFields
	(
	const JRGB& color
	)
{
	itsRInput->SetValue(color.red);
	itsGInput->SetValue(color.green);
	itsBInput->SetValue(color.blue);
}

/******************************************************************************
 OKToDeactivate (virtual protected)

	Update the color based on the latest input field values.

 ******************************************************************************/

bool
JXChooseColorDialog::OKToDeactivate()
{
	UpdateRGBColor();
	return JXModalDialogDirector::OKToDeactivate();
}
