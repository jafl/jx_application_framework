/******************************************************************************
 JXChooseColorDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXChooseColorDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXColorWheel.h>
#include <JXSlider.h>
#include <JXIntegerInput.h>
#include <JXFlatRect.h>
#include <JXStaticText.h>
#include <JXColormap.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXChooseColorDialog::JXChooseColorDialog
	(
	JXWindowDirector*	supervisor,
	const JColorIndex	colorIndex
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
	const JColorIndex colorIndex
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 510,250, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 410,220, 60,20);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXChooseColorDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,220, 60,20);
    assert( cancelButton != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 340,25, 60,20);
    assert( obj1_JXLayout != NULL );
    obj1_JXLayout->SetToLabel();

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,70, 50,20);
    assert( obj2_JXLayout != NULL );
    obj2_JXLayout->SetToLabel();

    JXStaticText* obj3_JXLayout =
        new JXStaticText(JGetString("obj3_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,90, 50,20);
    assert( obj3_JXLayout != NULL );
    obj3_JXLayout->SetToLabel();

    JXStaticText* obj4_JXLayout =
        new JXStaticText(JGetString("obj4_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,110, 50,20);
    assert( obj4_JXLayout != NULL );
    obj4_JXLayout->SetToLabel();

    itsHSlider =
        new JXSlider(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 340,70, 150,20);
    assert( itsHSlider != NULL );

    itsSSlider =
        new JXSlider(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 340,90, 150,20);
    assert( itsSSlider != NULL );

    itsVSlider =
        new JXSlider(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 340,110, 150,20);
    assert( itsVSlider != NULL );

    itsColorSample =
        new JXFlatRect(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 400,20, 30,30);
    assert( itsColorSample != NULL );

    JXStaticText* obj5_JXLayout =
        new JXStaticText(JGetString("obj5_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,140, 50,20);
    assert( obj5_JXLayout != NULL );
    obj5_JXLayout->SetToLabel();

    JXStaticText* obj6_JXLayout =
        new JXStaticText(JGetString("obj6_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,160, 50,20);
    assert( obj6_JXLayout != NULL );
    obj6_JXLayout->SetToLabel();

    JXStaticText* obj7_JXLayout =
        new JXStaticText(JGetString("obj7_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 290,180, 50,20);
    assert( obj7_JXLayout != NULL );
    obj7_JXLayout->SetToLabel();

    itsRInput =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 420,140, 70,20);
    assert( itsRInput != NULL );

    itsGInput =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 420,160, 70,20);
    assert( itsGInput != NULL );

    itsBInput =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 420,180, 70,20);
    assert( itsBInput != NULL );

    JXStaticText* obj8_JXLayout =
        new JXStaticText(JGetString("obj8_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 340,140, 80,20);
    assert( obj8_JXLayout != NULL );
    obj8_JXLayout->SetToLabel();

    JXStaticText* obj9_JXLayout =
        new JXStaticText(JGetString("obj9_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 340,160, 80,20);
    assert( obj9_JXLayout != NULL );
    obj9_JXLayout->SetToLabel();

    JXStaticText* obj10_JXLayout =
        new JXStaticText(JGetString("obj10_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 340,180, 80,20);
    assert( obj10_JXLayout != NULL );
    obj10_JXLayout->SetToLabel();

    itsColorWheel =
        new JXColorWheel(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 250,220);
    assert( itsColorWheel != NULL );

// end JXLayout

	window->SetTitle("Choose color");
	SetButtons(okButton, cancelButton);

	const JRGB rgb = (GetColormap())->JColormap::GetRGB(colorIndex);
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

JColorIndex
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
	JXColormap* colormap = GetColormap();

	JRGB newColor = itsColorWheel->GetRGB();
	UpdateSliders(newColor);
	UpdateInputFields(newColor);

	const JColorIndex newColorIndex = colormap->JColormap::GetColor(newColor);
	itsColorSample->SetBackColor(newColorIndex);
}

/******************************************************************************
 UpdateHSBColor (private)

 ******************************************************************************/

void
JXChooseColorDialog::UpdateHSBColor()
{
	JXColormap* colormap = GetColormap();

	JRGB newColor = JHSB(JRound(itsHSlider->GetValue()),
						 JRound(itsSSlider->GetValue()),
						 JRound(itsVSlider->GetValue()));
	UpdateColorWheel(newColor);
	UpdateInputFields(newColor);

	const JColorIndex newColorIndex = colormap->JColormap::GetColor(newColor);
	itsColorSample->SetBackColor(newColorIndex);
}

/******************************************************************************
 UpdateRGBColor (private)

 ******************************************************************************/

void
JXChooseColorDialog::UpdateRGBColor()
{
	JXColormap* colormap = GetColormap();

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

	const JColorIndex newColorIndex = colormap->JColormap::GetColor(newColor);
	itsColorSample->SetBackColor(newColorIndex);
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
