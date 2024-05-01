/******************************************************************************
 JX2DCurveOptionsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "JX2DCurveOptionsDialog.h"
#include "JX2DCurveNameList.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JX2DCurveOptionsDialog::JX2DCurveOptionsDialog
	(
	const JArray<J2DCurveInfo>&	array,
	const JArray<bool>&			hasXErrors,
	const JArray<bool>&			hasYErrors,
	const JArray<bool>&			isFunction,
	const JArray<bool>&			isScatter,
	const JIndex				startIndex
	)
	:
	JXModalDialogDirector(),
	itsCurrentIndex(startIndex)
{
	assert( array.IndexValid(startIndex) );

	itsCurveInfo = jnew JArray<J2DCurveInfo>;

	const JSize count = array.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		J2DCurveInfo info = array.GetItem(i);
		info.name = jnew JString(*info.name);
		itsCurveInfo->AppendItem(info);
	}

	itsHasXErrors = jnew JArray<bool>(hasXErrors);
	itsHasYErrors = jnew JArray<bool>(hasYErrors);
	itsIsFunction = jnew JArray<bool>(isFunction);
	itsIsScatter  = jnew JArray<bool>(isScatter);

	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JX2DCurveOptionsDialog::~JX2DCurveOptionsDialog()
{
	const JSize count = itsCurveInfo->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		J2DCurveInfo info = itsCurveInfo->GetItem(i);
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

	auto* window = jnew JXWindow(this, 370,260, JGetString("WindowTitle::JX2DCurveOptionsDialog::JXLayout"));

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 140,240);
	assert( scrollbarSet != nullptr );

	itsNameList =
		jnew JX2DCurveNameList(*itsCurveInfo, itsCurrentIndex, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 140,240);

	itsShowCurveCB =
		jnew JXTextCheckbox(JGetString("itsShowCurveCB::JX2DCurveOptionsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,20, 100,20);
	itsShowCurveCB->SetShortcuts(JGetString("itsShowCurveCB::shortcuts::JX2DCurveOptionsDialog::JXLayout"));

	itsCurveStyleRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 170,50, 184,104);

	itsShowPointsRB =
		jnew JXTextRadioButton(kShowPoints, JGetString("itsShowPointsRB::JX2DCurveOptionsDialog::JXLayout"), itsCurveStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 170,20);
	itsShowPointsRB->SetShortcuts(JGetString("itsShowPointsRB::shortcuts::JX2DCurveOptionsDialog::JXLayout"));

	itsShowSymbolsRB =
		jnew JXTextRadioButton(kShowSymbols, JGetString("itsShowSymbolsRB::JX2DCurveOptionsDialog::JXLayout"), itsCurveStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 170,20);
	itsShowSymbolsRB->SetShortcuts(JGetString("itsShowSymbolsRB::shortcuts::JX2DCurveOptionsDialog::JXLayout"));

	itsShowLinesRB =
		jnew JXTextRadioButton(kShowLines, JGetString("itsShowLinesRB::JX2DCurveOptionsDialog::JXLayout"), itsCurveStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 170,20);
	itsShowLinesRB->SetShortcuts(JGetString("itsShowLinesRB::shortcuts::JX2DCurveOptionsDialog::JXLayout"));

	itsShowBothRB =
		jnew JXTextRadioButton(kShowBoth, JGetString("itsShowBothRB::JX2DCurveOptionsDialog::JXLayout"), itsCurveStyleRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 170,20);
	itsShowBothRB->SetShortcuts(JGetString("itsShowBothRB::shortcuts::JX2DCurveOptionsDialog::JXLayout"));

	itsShowXErrorsCB =
		jnew JXTextCheckbox(JGetString("itsShowXErrorsCB::JX2DCurveOptionsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,170, 110,20);
	itsShowXErrorsCB->SetShortcuts(JGetString("itsShowXErrorsCB::shortcuts::JX2DCurveOptionsDialog::JXLayout"));

	itsShowYErrorsCB =
		jnew JXTextCheckbox(JGetString("itsShowYErrorsCB::JX2DCurveOptionsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,190, 110,20);
	itsShowYErrorsCB->SetShortcuts(JGetString("itsShowYErrorsCB::shortcuts::JX2DCurveOptionsDialog::JXLayout"));

	itsCancelButton =
		jnew JXTextButton(JGetString("itsCancelButton::JX2DCurveOptionsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 170,230, 70,20);
	itsCancelButton->SetShortcuts(JGetString("itsCancelButton::shortcuts::JX2DCurveOptionsDialog::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JX2DCurveOptionsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,230, 70,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::JX2DCurveOptionsDialog::JXLayout"));

// end JXLayout

	SetButtons(okButton, nullptr);

	ListenTo(itsCancelButton);
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
		const auto* info = dynamic_cast<const JX2DCurveNameList::NameSelected*>(&message);
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
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 SaveSettings (private)

 ******************************************************************************/

void
JX2DCurveOptionsDialog::SaveSettings()
{
	J2DCurveInfo info  = itsCurveInfo->GetItem(itsCurrentIndex);
	info.show          = itsShowCurveCB->IsChecked();
	info.xerrors       = itsShowXErrorsCB->IsChecked();
	info.yerrors       = itsShowYErrorsCB->IsChecked();
	DecodeCurveStyle(&info);
	itsCurveInfo->SetItem(itsCurrentIndex, info);
}

/******************************************************************************
 AdjustWidgets (private)

 ******************************************************************************/

void
JX2DCurveOptionsDialog::AdjustWidgets()
{
	const J2DCurveInfo info = itsCurveInfo->GetItem(itsCurrentIndex);

	itsShowCurveCB->SetState(info.show);

	JIndex style;
	if (!itsIsScatter->GetItem(itsCurrentIndex))
	{
		style = EncodeCurveStyle(info);
		itsShowPointsRB->Activate();
		itsShowLinesRB->Deactivate();
		itsShowSymbolsRB->Activate();
		itsShowBothRB->Deactivate();
	}
	else if (itsIsFunction->GetItem(itsCurrentIndex))
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
	itsShowXErrorsCB->SetVisible(itsHasXErrors->GetItem(itsCurrentIndex));

	itsShowYErrorsCB->SetState(info.yerrors);
	itsShowYErrorsCB->SetVisible(itsHasYErrors->GetItem(itsCurrentIndex));
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
	if (!JXModalDialogDirector::OKToDeactivate())
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
