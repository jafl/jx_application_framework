/******************************************************************************
 JXChooseColorDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include <JXChooseColorDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXColorWheel.h>
#include <JXSlider.h>
#include <JXIntegerInput.h>
#include <JXFlatRect.h>
#include <JXStaticText.h>
#include <JColorManager.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXChooseColorDialog::JXChooseColorDialog
	(
	JXWindowDirector*	supervisor,
	const JColorID	colorIndex
	)
	:
	JXDialogDirector(supervisor, kJTrue)
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

	JXWindow* window = jnew JXWindow(this, 510,250, JString::empty);
	assert( window != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,220, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::JXChooseColorDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,220, 60,20);
	assert( cancelButton != NULL );

	JXStaticText* sampleLabel =
		jnew JXStaticText(JGetString("sampleLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 340,25, 60,20);
	assert( sampleLabel != NULL );
	sampleLabel->SetToLabel();

	JXStaticText* hueLabel =
		jnew JXStaticText(JGetString("hueLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,70, 50,20);
	assert( hueLabel != NULL );
	hueLabel->SetToLabel();

	JXStaticText* saturationLabel =
		jnew JXStaticText(JGetString("saturationLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,90, 50,20);
	assert( saturationLabel != NULL );
	saturationLabel->SetToLabel();

	JXStaticText* brightnessLabel =
		jnew JXStaticText(JGetString("brightnessLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,110, 50,20);
	assert( brightnessLabel != NULL );
	brightnessLabel->SetToLabel();

	itsHSlider =
		jnew JXSlider(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 340,70, 150,20);
	assert( itsHSlider != NULL );

	itsSSlider =
		jnew JXSlider(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 340,90, 150,20);
	assert( itsSSlider != NULL );

	itsVSlider =
		jnew JXSlider(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 340,110, 150,20);
	assert( itsVSlider != NULL );

	itsColorSample =
		jnew JXFlatRect(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 400,20, 30,30);
	assert( itsColorSample != NULL );

	JXStaticText* redLabel =
		jnew JXStaticText(JGetString("redLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,140, 50,20);
	assert( redLabel != NULL );
	redLabel->SetToLabel();

	JXStaticText* greenLabel =
		jnew JXStaticText(JGetString("greenLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,160, 50,20);
	assert( greenLabel != NULL );
	greenLabel->SetToLabel();

	JXStaticText* blueLabel =
		jnew JXStaticText(JGetString("blueLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,180, 50,20);
	assert( blueLabel != NULL );
	blueLabel->SetToLabel();

	itsRInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 420,140, 70,20);
	assert( itsRInput != NULL );

	itsGInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 420,160, 70,20);
	assert( itsGInput != NULL );

	itsBInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 420,180, 70,20);
	assert( itsBInput != NULL );

	JXStaticText* redRangeLabel =
		jnew JXStaticText(JGetString("redRangeLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 340,140, 80,20);
	assert( redRangeLabel != NULL );
	redRangeLabel->SetToLabel();

	JXStaticText* greenRangeLabel =
		jnew JXStaticText(JGetString("greenRangeLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 340,160, 80,20);
	assert( greenRangeLabel != NULL );
	greenRangeLabel->SetToLabel();

	JXStaticText* blueRangeLabel =
		jnew JXStaticText(JGetString("blueRangeLabel::JXChooseColorDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 340,180, 80,20);
	assert( blueRangeLabel != NULL );
	blueRangeLabel->SetToLabel();

	itsColorWheel =
		jnew JXColorWheel(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 250,220);
	assert( itsColorWheel != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXChooseColorDialog"));
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
		JXDialogDirector::Receive(sender, message);
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
	JBoolean ok = itsRInput->GetValue(&r);
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

JBoolean
JXChooseColorDialog::OKToDeactivate()
{
	UpdateRGBColor();
	return JXDialogDirector::OKToDeactivate();
}
