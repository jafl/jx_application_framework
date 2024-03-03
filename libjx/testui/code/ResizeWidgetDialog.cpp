/******************************************************************************
 ResizeWidgetDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Written by John Lindal.

 ******************************************************************************/

#include "ResizeWidgetDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ResizeWidgetDialog::ResizeWidgetDialog
	(
	const JXWidget* widget
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(widget);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ResizeWidgetDialog::~ResizeWidgetDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
ResizeWidgetDialog::BuildWindow
	(
	const JXWidget* widget
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 160,120, JGetString("WindowTitle::ResizeWidgetDialog::JXLayout"));

	auto* widthLabel =
		jnew JXStaticText(JGetString("widthLabel::ResizeWidgetDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 50,20);
	widthLabel->SetToLabel(false);

	auto* heightLabel =
		jnew JXStaticText(JGetString("heightLabel::ResizeWidgetDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 50,20);
	heightLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ResizeWidgetDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 20,90, 50,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::ResizeWidgetDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 90,90, 50,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::ResizeWidgetDialog::JXLayout"));

	itsWidth =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 70,20, 70,20);
	itsWidth->SetIsRequired(false);

	itsHeight =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 70,50, 70,20);
	itsHeight->SetIsRequired(false);

// end JXLayout

	SetButtons(okButton, cancelButton);

	const JRect r = widget->GetBoundsGlobal();

	itsWidth->SetLowerLimit(50);
	itsWidth->SetUpperLimit(2000);
	itsWidth->SetValue(r.width());

	itsHeight->SetLowerLimit(50);
	itsHeight->SetUpperLimit(2000);
	itsHeight->SetValue(r.height());
}

/******************************************************************************
 GetNewSize

 ******************************************************************************/

void
ResizeWidgetDialog::GetNewSize
	(
	JCoordinate* w,
	JCoordinate* h
	)
	const
{
	const bool okW = itsWidth->GetValue(w);
	const bool okH = itsHeight->GetValue(h);
	assert( okW && okH );
}
