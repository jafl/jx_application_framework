/******************************************************************************
 SetElasticDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Written by John Lindal.

 ******************************************************************************/

#include "SetElasticDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SetElasticDialog::SetElasticDialog
	(
	const JIndex index,
	const JIndex maxIndex
	)
	:
	JXModalDialogDirector()
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

	auto* window = jnew JXWindow(this, 200,90, JString::empty);

	auto* elasticLabel =
		jnew JXStaticText(JGetString("elasticLabel::SetElasticDialog::JXLayout"),window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 130,20);
	elasticLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::SetElasticDialog::JXLayout"),window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 30,60, 50,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::SetElasticDialog::JXLayout"),window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 119,59, 52,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::SetElasticDialog::JXLayout"));

	itsElasticIndex =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 140,20, 40,20);

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::SetElasticDialog"));
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
	const bool ok = itsElasticIndex->GetValue(&size);
	assert( ok );
	return size;
}
