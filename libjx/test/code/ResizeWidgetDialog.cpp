/******************************************************************************
 ResizeWidgetDialog.cpp

	BASE CLASS = JXDialogDirector

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
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

	JXWindow* window = new JXWindow(this, 160,120, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::ResizeWidgetDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 20,90, 50,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::ResizeWidgetDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 90,90, 50,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::ResizeWidgetDialog::shortcuts::JXLayout"));

	itsWidth =
		new JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 70,20, 70,20);
	assert( itsWidth != NULL );

	itsHeight =
		new JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 70,50, 70,20);
	assert( itsHeight != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::ResizeWidgetDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 50,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::ResizeWidgetDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 50,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

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
