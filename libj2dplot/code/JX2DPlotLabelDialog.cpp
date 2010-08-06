/******************************************************************************
 JX2DPlotLabelDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JX2DPlotLabelDialog.h>
#include <JXFontNameMenu.h>
#include <JXFontSizeMenu.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JX2DPlotLabelDialog::JX2DPlotLabelDialog
	(
	JXWindowDirector*	supervisor,
	const JString&		title,
	const JString&		xLabel,
	const JString&		yLabel,
	const JString&		font,
	const JSize		size,
	const J2DPlotWidget::LabelSelection selection
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	itsSelection(selection)
{
	BuildWindow();
	itsPlotTitle->SetText(title);
	itsXAxisLabel->SetText(xLabel);
	itsYAxisLabel->SetText(yLabel);

	itsFontMenu->SetFontName(font);
	itsSizeMenu->SetFontSize(size);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JX2DPlotLabelDialog::~JX2DPlotLabelDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JX2DPlotLabelDialog::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 540,150, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JX2DPlotLabelDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 350,120, 70,20);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JX2DPlotLabelDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JX2DPlotLabelDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,120, 70,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts(JGetString("cancelButton::JX2DPlotLabelDialog::shortcuts::JXLayout"));

    itsPlotTitle =
        new JXInputField(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,20, 220,20);
    assert( itsPlotTitle != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::JX2DPlotLabelDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 90,20);
    assert( obj1_JXLayout != NULL );
    obj1_JXLayout->SetToLabel();

    itsXAxisLabel =
        new JXInputField(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,50, 220,20);
    assert( itsXAxisLabel != NULL );

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::JX2DPlotLabelDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 90,20);
    assert( obj2_JXLayout != NULL );
    obj2_JXLayout->SetToLabel();

    itsYAxisLabel =
        new JXInputField(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,80, 220,20);
    assert( itsYAxisLabel != NULL );

    JXStaticText* obj3_JXLayout =
        new JXStaticText(JGetString("obj3_JXLayout::JX2DPlotLabelDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 90,20);
    assert( obj3_JXLayout != NULL );
    obj3_JXLayout->SetToLabel();

    itsFontMenu =
        new JXFontNameMenu("Font:", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 350,20, 170,30);
    assert( itsFontMenu != NULL );

    itsSizeMenu =
        new JXFontSizeMenu(itsFontMenu, "Size:", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 350,60, 170,30);
    assert( itsSizeMenu != NULL );

// end JXLayout

	window->SetTitle("Edit Plot Labels");
	SetButtons(okButton, cancelButton);

	itsFontMenu->SetToPopupChoice();
	itsSizeMenu->SetToPopupChoice();
}

/******************************************************************************
 GetLabels

 ******************************************************************************/

void
JX2DPlotLabelDialog::GetLabels
	(
	JString* title,
	JString* xLabel,
	JString* yLabel
	)
	const
{
	*title  = itsPlotTitle->GetText();
	*xLabel = itsXAxisLabel->GetText();
	*yLabel = itsYAxisLabel->GetText();
}

/******************************************************************************
 GetFontName

 ******************************************************************************/

JString
JX2DPlotLabelDialog::GetFontName()
	const
{
	return itsFontMenu->GetFontName();
}

/******************************************************************************
 GetFontSize

 ******************************************************************************/

JSize
JX2DPlotLabelDialog::GetFontSize()
	const
{
	return itsSizeMenu->GetFontSize();
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JX2DPlotLabelDialog::Activate()
{
	JXDialogDirector::Activate();
	if (itsSelection == J2DPlotWidget::kTitle)
		{
		itsPlotTitle->Focus();
		}
	else if (itsSelection == J2DPlotWidget::kXLabel)
		{
		itsXAxisLabel->Focus();
		}
	else if (itsSelection == J2DPlotWidget::kYLabel)
		{
		itsYAxisLabel->Focus();
		}
}
