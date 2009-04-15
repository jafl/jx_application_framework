/******************************************************************************
 JX2DCurveOptionsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JX2DCurveOptionsDialog.h>
#include <JX2DCurveNameList.h>
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
	const JArray<JBoolean>&		hasXErrors,
	const JArray<JBoolean>&		hasYErrors,
	const JArray<JBoolean>&		isFunction,
	const JArray<JBoolean>&		isScatter,
	const JIndex				startIndex
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	itsCurrentIndex(startIndex)
{
	assert( array.IndexValid(startIndex) );

	itsCurveInfo = new JArray<J2DCurveInfo>;
	assert(itsCurveInfo != NULL);

	const JSize count = array.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		J2DCurveInfo info = array.GetElement(i);
		info.name = new JString(*info.name);
		assert(info.name != NULL);
		itsCurveInfo->AppendElement(info);
		}

	itsHasXErrors = new JArray<JBoolean>(hasXErrors);
	assert( itsHasXErrors != NULL );

	itsHasYErrors = new JArray<JBoolean>(hasYErrors);
	assert( itsHasYErrors != NULL );

	itsIsFunction = new JArray<JBoolean>(isFunction);
	assert( itsIsFunction != NULL );

	itsIsScatter = new JArray<JBoolean>(isScatter);
	assert( itsIsScatter != NULL );

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
		delete info.name;
		}
	delete itsCurveInfo;
	delete itsHasXErrors;
	delete itsHasYErrors;
	delete itsIsFunction;
	delete itsIsScatter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JX2DCurveOptionsDialog::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 370,260, "");
    assert( window != NULL );
    SetWindow(window);

    JXScrollbarSet* scrollbarSet =
        new JXScrollbarSet(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 140,240);
    assert( scrollbarSet != NULL );

    itsCurveStyleRG =
        new JXRadioGroup(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 170,50, 184,104);
    assert( itsCurveStyleRG != NULL );

    itsShowCurveCB =
        new JXTextCheckbox(JGetString("itsShowCurveCB::JX2DCurveOptionsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,20, 100,20);
    assert( itsShowCurveCB != NULL );
    itsShowCurveCB->SetShortcuts(JGetString("itsShowCurveCB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

    itsShowLinesRB =
        new JXTextRadioButton(kShowLines, JGetString("itsShowLinesRB::JX2DCurveOptionsDialog::JXLayout"), itsCurveStyleRG,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 170,20);
    assert( itsShowLinesRB != NULL );
    itsShowLinesRB->SetShortcuts(JGetString("itsShowLinesRB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

    itsShowBothRB =
        new JXTextRadioButton(kShowBoth, JGetString("itsShowBothRB::JX2DCurveOptionsDialog::JXLayout"), itsCurveStyleRG,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 170,20);
    assert( itsShowBothRB != NULL );
    itsShowBothRB->SetShortcuts(JGetString("itsShowBothRB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

    itsShowSymbolsRB =
        new JXTextRadioButton(kShowSymbols, JGetString("itsShowSymbolsRB::JX2DCurveOptionsDialog::JXLayout"), itsCurveStyleRG,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 170,20);
    assert( itsShowSymbolsRB != NULL );
    itsShowSymbolsRB->SetShortcuts(JGetString("itsShowSymbolsRB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

    itsShowPointsRB =
        new JXTextRadioButton(kShowPoints, JGetString("itsShowPointsRB::JX2DCurveOptionsDialog::JXLayout"), itsCurveStyleRG,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 170,20);
    assert( itsShowPointsRB != NULL );
    itsShowPointsRB->SetShortcuts(JGetString("itsShowPointsRB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JX2DCurveOptionsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 279,229, 72,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

    itsCancelButton =
        new JXTextButton(JGetString("itsCancelButton::JX2DCurveOptionsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 170,230, 70,20);
    assert( itsCancelButton != NULL );
    itsCancelButton->SetShortcuts(JGetString("itsCancelButton::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

    itsShowXErrorsCB =
        new JXTextCheckbox(JGetString("itsShowXErrorsCB::JX2DCurveOptionsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,170, 110,20);
    assert( itsShowXErrorsCB != NULL );
    itsShowXErrorsCB->SetShortcuts(JGetString("itsShowXErrorsCB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

    itsShowYErrorsCB =
        new JXTextCheckbox(JGetString("itsShowYErrorsCB::JX2DCurveOptionsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,190, 110,20);
    assert( itsShowYErrorsCB != NULL );
    itsShowYErrorsCB->SetShortcuts(JGetString("itsShowYErrorsCB::JX2DCurveOptionsDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle("Curve Options");
	SetButtons(okButton, NULL);
	UseModalPlacement(kJFalse);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

	ListenTo(itsCancelButton);

	itsNameList =
		new JX2DCurveNameList(*itsCurveInfo, itsCurrentIndex,
				scrollbarSet, scrollbarSet->GetScrollEnclosure(),
				JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsNameList != NULL );
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
		const JX2DCurveNameList::NameSelected* info =
			dynamic_cast(const JX2DCurveNameList::NameSelected*, &message);
		assert(info != NULL);
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
			EndDialog(kJFalse);
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
		info->symbols = kJFalse;
		info->lines   = kJFalse;
		}
	else if (style == kShowSymbols)
		{
		info->symbols = kJTrue;
		info->lines   = kJFalse;
		}
	else if (style == kShowLines)
		{
		info->symbols = kJFalse;
		info->lines   = kJTrue;
		}
	else if (style == kShowBoth)
		{
		info->symbols = kJTrue;
		info->lines   = kJTrue;
		}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
JX2DCurveOptionsDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}
	else
		{
		SaveSettings();
		return kJTrue;
		}
}
