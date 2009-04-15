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
    SetWindow(window);

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::ResizeWidgetDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 20,90, 50,20);
    assert( cancelButton != NULL );

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::ResizeWidgetDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 89,89, 52,22);
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

    JXStaticText* obj1 =
        new JXStaticText(JGetString("obj1::ResizeWidgetDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 50,20);
    assert( obj1 != NULL );

    JXStaticText* obj2 =
        new JXStaticText(JGetString("obj2::ResizeWidgetDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 50,20);
    assert( obj2 != NULL );

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
