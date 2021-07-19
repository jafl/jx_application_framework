/******************************************************************************
 JX2DCurveOptionsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "JX2DCurveOptionsDialog.h"
#include "JX2DCurveNameList.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXStaticText.h>
#include <JXTextCheckbox.h>
#include <JXScrollbarSet.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JX2DCurveOptionsDialog::JX2DCurveOptionsDialog
	(
	JXWindowDirector*			supervisor,
	const JArray<J2DCurveInfo>&	array,
	const JArray<bool>&		hasXErrors,
	const JArray<bool>&		hasYErrors,
	const JArray<bool>&		isFunction,
	const JArray<bool>&		isScatter,
	const JIndex				startIndex
	)
	:
	JXDialogDirector(supervisor, true),
	itsCurrentIndex(startIndex)
{
	assert( array.IndexValid(startIndex) );

	itsCurveInfo = jnew JArray<J2DCurveInfo>;
	assert(itsCurveInfo != nullptr);

	const JSize count = array.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		J2DCurveInfo info = array.GetElement(i);
		info.name = jnew JString(*info.name);
		assert(info.name != nullptr);
		itsCurveInfo->AppendElement(info);
		}

	itsHasXErrors = jnew JArray<bool>(hasXErrors);
	assert( itsHasXErrors != nullptr );

	itsHasYErrors = jnew JArray<bool>(hasYErrors);
	assert( itsHasYErrors != nullptr );

	itsIsFunction = jnew JArray<bool>(isFunction);
	assert( itsIsFunction != nullptr );

	itsIsScatter = jnew JArray<bool>(isScatter);
	assert( itsIsScatter != nullptr );

	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JX2DCurveOptionsDialog::~JX2DCurveOptionsDialog()
{
	const JSize count = itsCurveInfo->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		J2DCurveInfo info = itsCurveInfo->GetElement(i);
		jdelete info.name;
		}
	jdelete itsCurveInfo;
	jdelete itsHasXErrors;
	jdelete itsHasYErrors;
	jdelete itsIsFunction;
	jdelete itsIsScatter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JX2DCurveOptionsDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,260, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 140,240);
	assert( scrollbarSet != nullptr );

	itsCurveStyleRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 170,50, 184,104);
	assert( itsCurveStyleRG != nullptr );

	itsShowCurveCB =
		jnew JXTextCheckbox(JGetString("itsShowCurveCB::JX2DCurveOptionsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,20, 100,20);
	assert( itsShowCurveCB != nullptr );
	itsShowCurveCB->SetShortcuts(JGetString("itsShowCurveCB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

	itsShowLinesRB =
		jnew JXTextRadioButton(kShowLines, JGetString("itsShowLinesRB::JX2DCurveOptionsDialog::JXLayout"), itsCurveStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 170,20);
	assert( itsShowLinesRB != nullptr );
	itsShowLinesRB->SetShortcuts(JGetString("itsShowLinesRB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

	itsShowBothRB =
		jnew JXTextRadioButton(kShowBoth, JGetString("itsShowBothRB::JX2DCurveOptionsDialog::JXLayout"), itsCurveStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 170,20);
	assert( itsShowBothRB != nullptr );
	itsShowBothRB->SetShortcuts(JGetString("itsShowBothRB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

	itsShowSymbolsRB =
		jnew JXTextRadioButton(kShowSymbols, JGetString("itsShowSymbolsRB::JX2DCurveOptionsDialog::JXLayout"), itsCurveStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 170,20);
	assert( itsShowSymbolsRB != nullptr );
	itsShowSymbolsRB->SetShortcuts(JGetString("itsShowSymbolsRB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

	itsShowPointsRB =
		jnew JXTextRadioButton(kShowPoints, JGetString("itsShowPointsRB::JX2DCurveOptionsDialog::JXLayout"), itsCurveStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 170,20);
	assert( itsShowPointsRB != nullptr );
	itsShowPointsRB->SetShortcuts(JGetString("itsShowPointsRB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JX2DCurveOptionsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,230, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

	itsCancelButton =
		jnew JXTextButton(JGetString("itsCancelButton::JX2DCurveOptionsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 170,230, 70,20);
	assert( itsCancelButton != nullptr );
	itsCancelButton->SetShortcuts(JGetString("itsCancelButton::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

	itsShowXErrorsCB =
		jnew JXTextCheckbox(JGetString("itsShowXErrorsCB::JX2DCurveOptionsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,170, 110,20);
	assert( itsShowXErrorsCB != nullptr );
	itsShowXErrorsCB->SetShortcuts(JGetString("itsShowXErrorsCB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

	itsShowYErrorsCB =
		jnew JXTextCheckbox(JGetString("itsShowYErrorsCB::JX2DCurveOptionsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,190, 110,20);
	assert( itsShowYErrorsCB != nullptr );
	itsShowYErrorsCB->SetShortcuts(JGetString("itsShowYErrorsCB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JX2DCurveOptionsDialog"));
	SetButtons(okButton, nullptr);

	ListenTo(itsCancelButton);

	itsNameList =
		jnew JX2DCurveNameList(*itsCurveInfo, itsCurrentIndex,
				scrollbarSet, scrollbarSet->GetScrollEnclosure(),
				JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsNameList != nullptr );
	itsNameList->FitToEnclosure();
	ListenTo(itsNameList);

	AdjustWidgets();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JX2DCurveOptionsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsNameList && message.Is(JX2DCurveNameList::kNameSelected))
		{
		const auto* info =
			dynamic_cast<const JX2DCurveNameList::NameSelected*>(&message);
		assert(info != nullptr);
		SaveSettings();
		itsCurrentIndex = info->GetIndex();
		AdjustWidgets();
		}

	else if (sender == itsCancelButton && message.Is(JXButton::kPushed))
		{
		if (itsNameList->IsEditing())
			{
			itsNameList->CancelEditing();
			}
		else
			{
			EndDialog(false);
			}
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 SaveSettings (private)

 ******************************************************************************/

void
JX2DCurveOptionsDialog::SaveSettings()
{
	J2DCurveInfo info  = itsCurveInfo->GetElement(itsCurrentIndex);
	info.show          = itsShowCurveCB->IsChecked();
	info.xerrors       = itsShowXErrorsCB->IsChecked();
	info.yerrors       = itsShowYErrorsCB->IsChecked();
	DecodeCurveStyle(&info);
	itsCurveInfo->SetElement(itsCurrentIndex, info);
}

/******************************************************************************
 AdjustWidgets (private)

 ******************************************************************************/

void
JX2DCurveOptionsDialog::AdjustWidgets()
{
	const J2DCurveInfo info = itsCurveInfo->GetElement(itsCurrentIndex);

	itsShowCurveCB->SetState(info.show);

	JIndex style;
	if (!itsIsScatter->GetElement(itsCurrentIndex))
		{
		style = EncodeCurveStyle(info);
		itsShowPointsRB->Activate();
		itsShowLinesRB->Deactivate();
		itsShowSymbolsRB->Activate();
		itsShowBothRB->Deactivate();
		}
	else if (itsIsFunction->GetElement(itsCurrentIndex))
		{
		style = kShowLines;
		itsShowPointsRB->Deactivate();
		itsShowLinesRB->Activate();
		itsShowSymbolsRB->Deactivate();
		itsShowBothRB->Deactivate();
		}
	else
		{
		style = EncodeCurveStyle(info);
		itsShowPointsRB->Activate();
		itsShowLinesRB->Activate();
		itsShowSymbolsRB->Activate();
		itsShowBothRB->Activate();
		}
	itsCurveStyleRG->SelectItem(style);

	itsShowXErrorsCB->SetState(info.xerrors);
	itsShowXErrorsCB->SetVisible(itsHasXErrors->GetElement(itsCurrentIndex));

	itsShowYErrorsCB->SetState(info.yerrors);
	itsShowYErrorsCB->SetVisible(itsHasYErrors->GetElement(itsCurrentIndex));
}

/******************************************************************************
 EncodeCurveStyle (private)

 ******************************************************************************/

JIndex
JX2DCurveOptionsDialog::EncodeCurveStyle
	(
	const J2DCurveInfo& info
	)
{
	if (info.lines && info.symbols)
		{
		return kShowBoth;
		}
	else if (info.lines)
		{
		return kShowLines;
		}
	else if (info.symbols)
		{
		return kShowSymbols;
		}
	else
		{
		return kShowPoints;
		}
}

/******************************************************************************
 DecodeCurveStyle (private)

 ******************************************************************************/

void
JX2DCurveOptionsDialog::DecodeCurveStyle
	(
	J2DCurveInfo* info
	)
{
	const JIndex style = itsCurveStyleRG->GetSelectedItem();
	if (style == kShowPoints)
		{
		info->symbols = false;
		info->lines   = false;
		}
	else if (style == kShowSymbols)
		{
		info->symbols = true;
		info->lines   = false;
		}
	else if (style == kShowLines)
		{
		info->symbols = false;
		info->lines   = true;
		}
	else if (style == kShowBoth)
		{
		info->symbols = true;
		info->lines   = true;
		}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
JX2DCurveOptionsDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return false;
		}
	else if (Cancelled())
		{
		return true;
		}
	else
		{
		SaveSettings();
		return true;
		}
}
