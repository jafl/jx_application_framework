/******************************************************************************
 THX3DPlotFunctionDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998-2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "THX3DPlotFunctionDialog.h"
#include "THXVarList.h"
#include "thxGlobals.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXFloatInput.h>
#include <JXIntegerInput.h>
#include <JXExprEditorSet.h>
#include <JXExprEditor.h>
#include <JFunction.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	prevPlot can be NULL.

 ******************************************************************************/

THX3DPlotFunctionDialog::THX3DPlotFunctionDialog
	(
	JXDirector*					supervisor,
	const THXVarList*			varList,
	const THX3DPlotDirector*	prevPlot
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(varList);
}

THX3DPlotFunctionDialog::THX3DPlotFunctionDialog
	(
	JXDirector*				supervisor,
	const THXVarList*		varList,
	const JFunction&		f,
	const JCharacter*		surfaceName,
	const JFloat			xMin,
	const JFloat			xMax,
	const JSize				xCount,
	const JFloat			yMin,
	const JFloat			yMax,
	const JSize				yCount
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(varList);

	itsExprWidget->SetFunction(varList, f.Copy());
	itsSurfaceName->SetText(surfaceName);
	itsXMinInput->SetValue(xMin);
	itsXMaxInput->SetValue(xMax);
	itsXCountInput->SetValue(xCount);
	itsYMinInput->SetValue(yMin);
	itsYMaxInput->SetValue(yMax);
	itsYCountInput->SetValue(yCount);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THX3DPlotFunctionDialog::~THX3DPlotFunctionDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
THX3DPlotFunctionDialog::BuildWindow
	(
	const THXVarList* varList
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 440,300, "");
	assert( window != NULL );

	JXExprEditorSet* obj1_JXLayout =
		new JXExprEditorSet(varList, &itsExprWidget, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 440,150);
	assert( obj1_JXLayout != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::THX3DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 110,270, 60,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::THX3DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 270,270, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::THX3DPlotFunctionDialog::shortcuts::JXLayout"));

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::THX3DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,170, 90,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::THX3DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,200, 90,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::THX3DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 180,200, 20,20);
	assert( obj4_JXLayout != NULL );
	obj4_JXLayout->SetToLabel();

	JXStaticText* obj5_JXLayout =
		new JXStaticText(JGetString("obj5_JXLayout::THX3DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,230, 90,20);
	assert( obj5_JXLayout != NULL );
	obj5_JXLayout->SetToLabel();

	JXStaticText* obj6_JXLayout =
		new JXStaticText(JGetString("obj6_JXLayout::THX3DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 180,230, 20,20);
	assert( obj6_JXLayout != NULL );
	obj6_JXLayout->SetToLabel();

	JXStaticText* obj7_JXLayout =
		new JXStaticText(JGetString("obj7_JXLayout::THX3DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 280,200, 30,20);
	assert( obj7_JXLayout != NULL );
	obj7_JXLayout->SetToLabel();

	JXStaticText* obj8_JXLayout =
		new JXStaticText(JGetString("obj8_JXLayout::THX3DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 390,200, 40,20);
	assert( obj8_JXLayout != NULL );
	obj8_JXLayout->SetToLabel();

	JXStaticText* obj9_JXLayout =
		new JXStaticText(JGetString("obj9_JXLayout::THX3DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 280,230, 30,20);
	assert( obj9_JXLayout != NULL );
	obj9_JXLayout->SetToLabel();

	JXStaticText* obj10_JXLayout =
		new JXStaticText(JGetString("obj10_JXLayout::THX3DPlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 390,230, 40,20);
	assert( obj10_JXLayout != NULL );
	obj10_JXLayout->SetToLabel();

	itsSurfaceName =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 100,170, 290,20);
	assert( itsSurfaceName != NULL );

	itsXMinInput =
		new JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 100,200, 80,20);
	assert( itsXMinInput != NULL );

	itsXMaxInput =
		new JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 200,200, 80,20);
	assert( itsXMaxInput != NULL );

	itsXCountInput =
		new JXIntegerInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 310,200, 80,20);
	assert( itsXCountInput != NULL );

	itsYMinInput =
		new JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 100,230, 80,20);
	assert( itsYMinInput != NULL );

	itsYMaxInput =
		new JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 200,230, 80,20);
	assert( itsYMaxInput != NULL );

	itsYCountInput =
		new JXIntegerInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 310,230, 80,20);
	assert( itsYCountInput != NULL );

// end JXLayout

	window->SetTitle("Plot 3D function");
	SetButtons(okButton, cancelButton);

	itsSurfaceName->ShareEditMenu(itsExprWidget->GetEditMenu());

	itsXMinInput->SetValue(-5.0);
	itsXMinInput->ShareEditMenu(itsExprWidget->GetEditMenu());

	itsXMaxInput->SetValue(5.0);
	itsXMaxInput->ShareEditMenu(itsExprWidget->GetEditMenu());

	itsXCountInput->SetValue(20);
	itsXCountInput->SetLimits(2, 100);
	itsXCountInput->ShareEditMenu(itsExprWidget->GetEditMenu());

	itsYMinInput->SetValue(-5.0);
	itsYMinInput->ShareEditMenu(itsExprWidget->GetEditMenu());

	itsYMaxInput->SetValue(5.0);
	itsYMaxInput->ShareEditMenu(itsExprWidget->GetEditMenu());

	itsYCountInput->SetValue(20);
	itsYCountInput->SetLimits(2, 100);
	itsYCountInput->ShareEditMenu(itsExprWidget->GetEditMenu());
}

/******************************************************************************
 GetSettings

 ******************************************************************************/

void
THX3DPlotFunctionDialog::GetSettings
	(
	const JFunction**	f,
	JString*			surfaceName,
	JFloat*				xMin,
	JFloat*				xMax,
	JSize*				xCount,
	JFloat*				yMin,
	JFloat*				yMax,
	JSize*				yCount
	)
	const
{
	*f           = itsExprWidget->GetFunction();
	*surfaceName = itsSurfaceName->GetText();

	if (surfaceName->IsEmpty())
		{
		*surfaceName = (**f).Print();
		}

	JFloat min, max;

	JBoolean ok = itsXMinInput->GetValue(&min);
	assert( ok );

	ok = itsXMaxInput->GetValue(&max);
	assert( ok );

	*xMin = JMin(min, max);
	*xMax = JMax(min, max);

	JInteger count;
	ok = itsXCountInput->GetValue(&count);
	assert( ok );
	*xCount = count;

	ok = itsYMinInput->GetValue(&min);
	assert( ok );

	ok = itsYMaxInput->GetValue(&max);
	assert( ok );

	*yMin = JMin(min, max);
	*yMax = JMax(min, max);

	ok = itsYCountInput->GetValue(&count);
	assert( ok );
	*yCount = count;
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
THX3DPlotFunctionDialog::OKToDeactivate()
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
