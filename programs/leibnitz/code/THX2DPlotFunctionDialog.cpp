/******************************************************************************
 THX2DPlotFunctionDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "THX2DPlotFunctionDialog.h"
#include "THXVarList.h"
#include "thxGlobals.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXFloatInput.h>
#include <JXExprEditorSet.h>
#include <JXExprEditor.h>
#include <JFunction.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	prevPlot can be NULL.

 ******************************************************************************/

THX2DPlotFunctionDialog::THX2DPlotFunctionDialog
	(
	JXDirector*					supervisor,
	const THXVarList*			varList,
	const THX2DPlotDirector*	prevPlot
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(varList, prevPlot);
}

THX2DPlotFunctionDialog::THX2DPlotFunctionDialog
	(
	JXDirector*				supervisor,
	const THXVarList*		varList,
	const JFunction&		f,
	const JCharacter*		curveName,
	const JFloat			min,
	const JFloat			max
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(varList, NULL);

	itsExprWidget->SetFunction(varList, f.Copy());
	itsPlotMenu->Hide();
	itsCurveName->SetText(curveName);
	itsMinInput->SetValue(min);
	itsMaxInput->SetValue(max);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THX2DPlotFunctionDialog::~THX2DPlotFunctionDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
THX2DPlotFunctionDialog::BuildWindow
	(
	const THXVarList*			varList,
	const THX2DPlotDirector*	prevPlot
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 330,320, "");
	assert( window != NULL );

	JXExprEditorSet* exprEditorSet =
		jnew JXExprEditorSet(varList, &itsExprWidget, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 330,150);
	assert( exprEditorSet != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::THX2DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,290, 60,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::THX2DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,290, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::THX2DPlotFunctionDialog::shortcuts::JXLayout"));

	JXStaticText* curveLabel =
		jnew JXStaticText(JGetString("curveLabel::THX2DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 30,220, 80,20);
	assert( curveLabel != NULL );
	curveLabel->SetToLabel();

	itsCurveName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 110,220, 180,20);
	assert( itsCurveName != NULL );

	JXStaticText* rangeLabel =
		jnew JXStaticText(JGetString("rangeLabel::THX2DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 30,250, 80,20);
	assert( rangeLabel != NULL );
	rangeLabel->SetToLabel();

	itsMinInput =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 110,250, 80,20);
	assert( itsMinInput != NULL );

	JXStaticText* toLabel =
		jnew JXStaticText(JGetString("toLabel::THX2DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 190,250, 20,20);
	assert( toLabel != NULL );
	toLabel->SetToLabel();

	itsMaxInput =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 210,250, 80,20);
	assert( itsMaxInput != NULL );

	itsPlotMenu =
		jnew JXTextMenu(JGetString("itsPlotMenu::THX2DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 30,170, 260,30);
	assert( itsPlotMenu != NULL );

// end JXLayout

	window->SetTitle("Plot 2D function");
	SetButtons(okButton, cancelButton);

	(THXGetApplication())->BuildPlotMenu(itsPlotMenu, prevPlot, &itsPlotIndex);
	itsPlotMenu->SetToPopupChoice(kJTrue, itsPlotIndex);
	ListenTo(itsPlotMenu);

	itsCurveName->ShareEditMenu(itsExprWidget->GetEditMenu());

	itsMinInput->SetValue(0.0);
	itsMinInput->ShareEditMenu(itsExprWidget->GetEditMenu());

	itsMaxInput->SetValue(1.0);
	itsMaxInput->ShareEditMenu(itsExprWidget->GetEditMenu());
}

/******************************************************************************
 GetSettings

 ******************************************************************************/

void
THX2DPlotFunctionDialog::GetSettings
	(
	JIndex*				plotIndex,
	const JFunction**	f,
	JString*			curveName,
	JFloat*				fMin,
	JFloat*				fMax
	)
	const
{
	*plotIndex = itsPlotIndex;
	*f         = itsExprWidget->GetFunction();
	*curveName = itsCurveName->GetText();

	if (curveName->IsEmpty())
		{
		*curveName = (**f).Print();
		}

	JFloat min, max;

	JBoolean ok = itsMinInput->GetValue(&min);
	assert( ok );

	ok = itsMaxInput->GetValue(&max);
	assert( ok );

	*fMin = JMin(min, max);
	*fMax = JMax(min, max);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
THX2DPlotFunctionDialog::Receive
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
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsPlotIndex = selection->GetIndex();
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
THX2DPlotFunctionDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	else if (itsExprWidget->ContainsUIF())
		{
		(JGetUserNotification())->ReportError("Please finish entering the function.");
		itsExprWidget->Focus();
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}
