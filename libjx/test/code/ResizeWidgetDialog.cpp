/******************************************************************************
 ResizeWidgetDialog.cpp

	BASE CLASS = JXDialogDirector

	Written by John Lindal.

 ******************************************************************************/

#include "ResizeWidgetDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXIntegerInput.h>
#include <JXStaticText.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ResizeWidgetDialog::ResizeWidgetDialog
	(
	JXWindowDirector*	supervisor,
	const JXWidget*		widget
	)
	:
	JXDialogDirector(supervisor, kJTrue)
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

	JXWindow* window = jnew JXWindow(this, 160,120, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ResizeWidgetDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 20,90, 50,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::ResizeWidgetDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 90,90, 50,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::ResizeWidgetDialog::shortcuts::JXLayout"));

	itsWidth =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 70,20, 70,20);
	assert( itsWidth != NULL );

	itsHeight =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 70,50, 70,20);
	assert( itsHeight != NULL );

	JXStaticText* widthLabel =
		jnew JXStaticText(JGetString("widthLabel::ResizeWidgetDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 50,20);
	assert( widthLabel != NULL );
	widthLabel->SetToLabel();

	JXStaticText* heightLabel =
		jnew JXStaticText(JGetString("heightLabel::ResizeWidgetDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 50,20);
	assert( heightLabel != NULL );
	heightLabel->SetToLabel();

// end JXLayout

	window->SetTitle("Change widget size");
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
	const JBoolean okW = itsWidth->GetValue(w);
	const JBoolean okH = itsHeight->GetValue(h);
	assert( okW && okH );
}
