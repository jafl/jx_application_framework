/******************************************************************************
 SetElasticDialog.cpp

	BASE CLASS = JXDialogDirector

	Written by John Lindal.

 ******************************************************************************/

#include "SetElasticDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXIntegerInput.h>
#include <JXStaticText.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SetElasticDialog::SetElasticDialog
	(
	JXWindowDirector*	supervisor,
	const JIndex		index,
	const JIndex		maxIndex
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(index, maxIndex);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SetElasticDialog::~SetElasticDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
SetElasticDialog::BuildWindow
	(
	const JIndex index,
	const JIndex maxIndex
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 200,90, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::SetElasticDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 30,60, 50,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::SetElasticDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 120,60, 50,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::SetElasticDialog::shortcuts::JXLayout"));

	itsElasticIndex =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 140,20, 40,20);
	assert( itsElasticIndex != NULL );

	JXStaticText* obj1_JXLayout =
		jnew JXStaticText(JGetString("obj1_JXLayout::SetElasticDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 130,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

// end JXLayout

	window->SetTitle("Set elastic compartment");
	SetButtons(okButton, cancelButton);

	itsElasticIndex->SetValue(index);
	itsElasticIndex->SetLimits(0, maxIndex);
}

/******************************************************************************
 GetElasticIndex

 ******************************************************************************/

JIndex
SetElasticDialog::GetElasticIndex()
	const
{
	JInteger size;
	const JBoolean ok = itsElasticIndex->GetValue(&size);
	assert( ok );
	return size;
}
