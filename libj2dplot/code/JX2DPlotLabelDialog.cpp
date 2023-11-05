/******************************************************************************
 JX2DPlotLabelDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "JX2DPlotLabelDialog.h"
#include <jx-af/jx/JXFontNameMenu.h>
#include <jx-af/jx/JXFontSizeMenu.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JX2DPlotLabelDialog::JX2DPlotLabelDialog
	(
	const JString&		title,
	const JString&		xLabel,
	const JString&		yLabel,
	const JString&		font,
	const JSize			size,

	const J2DPlotWidget::LabelSelection selection
	)
	:
	JXModalDialogDirector(),
	itsSelection(selection)
{
	BuildWindow();
	itsPlotTitle->GetText()->SetText(title);
	itsXAxisLabel->GetText()->SetText(xLabel);
	itsYAxisLabel->GetText()->SetText(yLabel);

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

	auto* window = jnew JXWindow(this, 540,150, JString::empty);

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JX2DPlotLabelDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,120, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JX2DPlotLabelDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JX2DPlotLabelDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,120, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::JX2DPlotLabelDialog::shortcuts::JXLayout"));

	itsPlotTitle =
		jnew JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,20, 220,20);

	auto* titleLabel =
		jnew JXStaticText(JGetString("titleLabel::JX2DPlotLabelDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 90,20);
	assert( titleLabel != nullptr );
	titleLabel->SetToLabel();

	itsXAxisLabel =
		jnew JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,50, 220,20);

	auto* xAxisLabel =
		jnew JXStaticText(JGetString("xAxisLabel::JX2DPlotLabelDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 90,20);
	assert( xAxisLabel != nullptr );
	xAxisLabel->SetToLabel();

	itsYAxisLabel =
		jnew JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,80, 220,20);

	auto* yAxisLabel =
		jnew JXStaticText(JGetString("yAxisLabel::JX2DPlotLabelDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 90,20);
	assert( yAxisLabel != nullptr );
	yAxisLabel->SetToLabel();

	itsFontMenu =
		jnew JXFontNameMenu(JGetString("FontNameMenuTitle::JX2DPlotLabelDialog"), false, window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 350,20, 170,30);
	assert( itsFontMenu != nullptr );

	itsSizeMenu =
		jnew JXFontSizeMenu(itsFontMenu, JGetString("FontSizeMenuTitle::JX2DPlotLabelDialog"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 350,60, 170,30);
	assert( itsSizeMenu != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JX2DPlotLabelDialog"));
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
	*title  = itsPlotTitle->GetText()->GetText();
	*xLabel = itsXAxisLabel->GetText()->GetText();
	*yLabel = itsYAxisLabel->GetText()->GetText();
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
	JXModalDialogDirector::Activate();
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
