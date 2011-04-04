/******************************************************************************
 SCAddToPlotDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCAddToPlotDialog.h"
#include "SCCircuitDocument.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXInputField.h>
#include <JXFloatInput.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCAddToPlotDialog::SCAddToPlotDialog
	(
	SCCircuitDocument* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsPlotIndex = 1;
	BuildWindow(supervisor);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCAddToPlotDialog::~SCAddToPlotDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
SCAddToPlotDialog::BuildWindow
	(
	SCCircuitDocument* doc
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 300,170, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::SCAddToPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,140, 60,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::SCAddToPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 180,140, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::SCAddToPlotDialog::shortcuts::JXLayout"));

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::SCAddToPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,70, 80,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	itsCurveName =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,70, 180,20);
	assert( itsCurveName != NULL );

	itsMinValue =
		new JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,100, 50,20);
	assert( itsMinValue != NULL );

	itsMaxValue =
		new JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,100, 50,20);
	assert( itsMaxValue != NULL );

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::SCAddToPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,100, 140,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::SCAddToPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 210,100, 20,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

	itsPlotMenu =
		new JXTextMenu(JGetString("itsPlotMenu::SCAddToPlotDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 260,30);
	assert( itsPlotMenu != NULL );

// end JXLayout

	window->SetTitle("Add to plot");
	SetButtons(okButton, cancelButton);

	doc->BuildPlotMenu(itsPlotMenu);
	itsPlotMenu->SetToPopupChoice(kJTrue, itsPlotIndex);
	ListenTo(itsPlotMenu);

	itsCurveName->SetText("New curve");
	itsCurveName->SetIsRequired();

	itsMinValue->SetValue(1.0);
	itsMinValue->SetLowerLimit(0.0);

	itsMaxValue->SetValue(1000.0);
	itsMaxValue->SetLowerLimit(0.0);
}

/******************************************************************************
 GetSettings

 ******************************************************************************/

void
SCAddToPlotDialog::GetSettings
	(
	JIndex*		plotIndex,
	JString*	curveName,
	JFloat*		fMin,
	JFloat*		fMax
	)
	const
{
	*plotIndex = itsPlotIndex;
	*curveName = itsCurveName->GetText();

	JBoolean ok = itsMinValue->GetValue(fMin);
	assert( ok );

	ok = itsMaxValue->GetValue(fMax);
	assert( ok );
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
SCAddToPlotDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPlotMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		itsPlotMenu->CheckItem(itsPlotIndex);
		}
	else if (sender == itsPlotMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		itsPlotIndex = selection->GetIndex();
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
