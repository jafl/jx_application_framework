/******************************************************************************
 JXChooseColorDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXChooseColorDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
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

	JXColormap* colormap = GetColormap();
	colormap->PrepareForMassColorAllocation();
	colormap->UsingColor(colorIndex);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXChooseColorDialog::~JXChooseColorDialog()
{
	JXColormap* colormap = GetColormap();
	colormap->MassColorAllocationFinished();
	colormap->DeallocateColor(GetColor());
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

    JXWindow* window = new JXWindow(this, 240,250, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 139,219, 62,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXChooseColorDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,220, 60,20);
    assert( cancelButton != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,25, 50,20);
    assert( obj1_JXLayout != NULL );

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 50,20);
    assert( obj2_JXLayout != NULL );

    JXStaticText* obj3_JXLayout =
        new JXStaticText(JGetString("obj3_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 50,20);
    assert( obj3_JXLayout != NULL );

    JXStaticText* obj4_JXLayout =
        new JXStaticText(JGetString("obj4_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,110, 50,20);
    assert( obj4_JXLayout != NULL );

    itsHSlider =
        new JXSlider(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 70,70, 150,20);
    assert( itsHSlider != NULL );

    itsSSlider =
        new JXSlider(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 70,90, 150,20);
    assert( itsSSlider != NULL );

    itsVSlider =
        new JXSlider(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 70,110, 150,20);
    assert( itsVSlider != NULL );

    itsColorSample =
        new JXFlatRect(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 130,20, 30,30);
    assert( itsColorSample != NULL );

    JXStaticText* obj5_JXLayout =
        new JXStaticText(JGetString("obj5_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 50,20);
    assert( obj5_JXLayout != NULL );

    JXStaticText* obj6_JXLayout =
        new JXStaticText(JGetString("obj6_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 50,20);
    assert( obj6_JXLayout != NULL );

    JXStaticText* obj7_JXLayout =
        new JXStaticText(JGetString("obj7_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 50,20);
    assert( obj7_JXLayout != NULL );

    itsRInput =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 150,140, 70,20);
    assert( itsRInput != NULL );

    itsGInput =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 150,160, 70,20);
    assert( itsGInput != NULL );

    itsBInput =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 150,180, 70,20);
    assert( itsBInput != NULL );

    JXStaticText* obj8_JXLayout =
        new JXStaticText(JGetString("obj8_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,140, 80,20);
    assert( obj8_JXLayout != NULL );

    JXStaticText* obj9_JXLayout =
        new JXStaticText(JGetString("obj9_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,160, 80,20);
    assert( obj9_JXLayout != NULL );

    JXStaticText* obj10_JXLayout =
        new JXStaticText(JGetString("obj10_JXLayout::JXChooseColorDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,180, 80,20);
    assert( obj10_JXLayout != NULL );

// end JXLayout

	window->SetTitle("Choose color");
	SetButtons(okButton, cancelButton);

	const JRGB rgb = (GetColormap())->JColormap::GetRGB(colorIndex);
	const JHSV hsv(rgb);

	itsHSlider->SetRange(0.0, kJMaxHSVValueF);
	itsHSlider->SetValue(hsv.hue);
	ListenTo(itsHSlider);

	itsSSlider->SetRange(0.0, kJMaxHSVValueF);
	itsSSlider->SetValue(hsv.saturation);
	ListenTo(itsSSlider);

	itsVSlider->SetRange(0.0, kJMaxHSVValueF);
	itsVSlider->SetValue(hsv.value);
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
	if (message.Is(JSliderBase::kMoved))
		{
		UpdateColor(kJTrue);
		}
	else if (message.Is(JXWidget::kLostFocus))
		{
		UpdateColor(kJFalse);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateColor (private)

 ******************************************************************************/

void
JXChooseColorDialog::UpdateColor
	(
	const JBoolean sliderChanged
	)
{
	JXColormap* colormap             = GetColormap();
	const JColorIndex origColorIndex = GetColor();
	const JRGB origColor             = colormap->JColormap::GetRGB(origColorIndex);

	JRGB newColor;
	if (sliderChanged)
		{
		newColor = JHSV(JRound(itsHSlider->GetValue()),
						JRound(itsSSlider->GetValue()),
						JRound(itsVSlider->GetValue()));
		itsRInput->SetValue(newColor.red);
		itsGInput->SetValue(newColor.green);
		itsBInput->SetValue(newColor.blue);
		}
	else
		{
		JInteger r,g,b;
		JBoolean ok = itsRInput->GetValue(&r);
		assert( ok );
		ok = itsGInput->GetValue(&g);
		assert( ok );
		ok = itsBInput->GetValue(&b);
		assert( ok );
		newColor.Set(r,g,b);

		StopListening(itsHSlider);
		StopListening(itsSSlider);
		StopListening(itsVSlider);
		JHSV hsv(newColor);
		itsHSlider->SetValue(hsv.hue);
		itsSSlider->SetValue(hsv.saturation);
		itsVSlider->SetValue(hsv.value);
		ListenTo(itsHSlider);
		ListenTo(itsSSlider);
		ListenTo(itsVSlider);
		}

	JColorIndex newColorIndex;
	colormap->DeallocateColor(origColorIndex);
	if (colormap->JColormap::AllocateStaticColor(newColor, &newColorIndex) ||
		colormap->JColormap::AllocateStaticColor(origColor, &newColorIndex))
		{
		itsColorSample->SetBackColor(newColorIndex);
		}
	else
		{
		itsColorSample->SetBackColor(colormap->GetDefaultBackColor());
		}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

	Update the color based on the latest input field values.

 ******************************************************************************/

JBoolean
JXChooseColorDialog::OKToDeactivate()
{
	UpdateColor(kJFalse);
	return JXDialogDirector::OKToDeactivate();
}
