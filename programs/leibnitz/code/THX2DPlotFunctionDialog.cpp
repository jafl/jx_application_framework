/******************************************************************************
 THX2DPlotFunctionDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by John Lindal.

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

	prevPlot can be nullptr.

 ******************************************************************************/

THX2DPlotFunctionDialog::THX2DPlotFunctionDialog
	(
	JXDirector*					supervisor,
	const THXVarList*			varList,
	const THX2DPlotDirector*	prevPlot
	)
	:
	JXDialogDirector(supervisor, true)
{
	BuildWindow(varList, prevPlot);
}

THX2DPlotFunctionDialog::THX2DPlotFunctionDialog
	(
	JXDirector*			supervisor,
	const THXVarList*	varList,
	const JFunction&	f,
	const JString&		curveName,
	const JFloat		min,
	const JFloat		max
	)
	:
	JXDialogDirector(supervisor, true)
{
	BuildWindow(varList, nullptr);

	itsExprWidget->SetFunction(varList, f.Copy());
	itsPlotMenu->Hide();
	itsCurveName->GetText()->SetText(curveName);
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

	auto* window = jnew JXWindow(this, 330,320, JString::empty);
	assert( window != nullptr );

	auto* exprEditorSet =
		jnew JXExprEditorSet(varList, &itsExprWidget, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 330,150);
	assert( exprEditorSet != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::THX2DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,290, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::THX2DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,290, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::THX2DPlotFunctionDialog::shortcuts::JXLayout"));

	auto* curveLabel =
		jnew JXStaticText(JGetString("curveLabel::THX2DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 30,220, 80,20);
	assert( curveLabel != nullptr );
	curveLabel->SetToLabel();

	itsCurveName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 110,220, 180,20);
	assert( itsCurveName != nullptr );

	auto* rangeLabel =
		jnew JXStaticText(JGetString("rangeLabel::THX2DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 30,250, 80,20);
	assert( rangeLabel != nullptr );
	rangeLabel->SetToLabel();

	itsMinInput =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 110,250, 80,20);
	assert( itsMinInput != nullptr );

	auto* toLabel =
		jnew JXStaticText(JGetString("toLabel::THX2DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 190,250, 20,20);
	assert( toLabel != nullptr );
	toLabel->SetToLabel();

	itsMaxInput =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 210,250, 80,20);
	assert( itsMaxInput != nullptr );

	itsPlotMenu =
		jnew JXTextMenu(JGetString("itsPlotMenu::THX2DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 30,170, 260,30);
	assert( itsPlotMenu != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::THX2DPlotFunctionDialog"));
	SetButtons(okButton, cancelButton);

	(THXGetApplication())->BuildPlotMenu(itsPlotMenu, prevPlot, &itsPlotIndex);
	itsPlotMenu->SetToPopupChoice(true, itsPlotIndex);
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
	*curveName = itsCurveName->GetText()->GetText();

	if (curveName->IsEmpty())
		{
		*curveName = (**f).Print();
		}

	JFloat min, max;

	bool ok = itsMinInput->GetValue(&min);
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
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
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

bool
THX2DPlotFunctionDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return false;
		}
	else if (Cancelled())
		{
		return true;
		}

	else if (itsExprWidget->ContainsUIF())
		{
		JGetUserNotification()->ReportError(JGetString("FinishFunction::THX2DPlotFunctionDialog"));
		itsExprWidget->Focus();
		return false;
		}
	else
		{
		return true;
		}
}
