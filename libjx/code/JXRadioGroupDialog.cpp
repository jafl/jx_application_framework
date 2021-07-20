/******************************************************************************
 JXRadioGroupDialog.cpp

	Dynamically constructed dialog window to ask the user to pick from
	a list of choices.

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JXRadioGroupDialog.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXRadioGroup.h"
#include "JXTextRadioButton.h"
#include "JXStaticText.h"
#include <jGlobals.h>
#include <JMinMax.h>
#include <jAssert.h>

const JCoordinate kFirstItemTop     = 20;
const JCoordinate kItemVDelta       = 10;
const JCoordinate kTextHeight       = 20;
const JCoordinate kItemVSeparation  = kTextHeight + kItemVDelta;
const JCoordinate kRGButtonVDelta   = 20;
const JCoordinate kButtonWidth      = 60;
const JCoordinate kHMarginWidth     = 20;
const JCoordinate kRGHLMarginWidth  = 10;
const JCoordinate kRGHRMarginWidth  = 15;
const JCoordinate kMinButtonHMargin = 10;

/******************************************************************************
 Constructor

	shortcutList can be nullptr.

 ******************************************************************************/

JXRadioGroupDialog::JXRadioGroupDialog
	(
	JXDirector*					supervisor,
	const JString&				windowTitle,
	const JString&				prompt,
	const JPtrArray<JString>&	choiceList,
	const JPtrArray<JString>*	shortcutList
	)
	:
	JXDialogDirector(supervisor, true)
{
	BuildWindow(windowTitle, prompt, choiceList, shortcutList);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXRadioGroupDialog::~JXRadioGroupDialog()
{
}

/******************************************************************************
 GetSelectedItem

 ******************************************************************************/

JIndex
JXRadioGroupDialog::GetSelectedItem()
	const
{
	return itsRG->GetSelectedItem();
}

/******************************************************************************
 SelectItem

 ******************************************************************************/

void
JXRadioGroupDialog::SelectItem
	(
	const JIndex index
	)
{
	itsRG->SelectItem(index);
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
JXRadioGroupDialog::BuildWindow
	(
	const JString&				windowTitle,
	const JString&				prompt,
	const JPtrArray<JString>&	choiceList,
	const JPtrArray<JString>*	shortcutList
	)
{
	const JSize actionCount = choiceList.GetElementCount();

	auto* window = jnew JXWindow(this, 10,10, windowTitle);
	assert( window != nullptr );

	JCoordinate y = kFirstItemTop;

	// instructions

	auto* instrText =
		jnew JXStaticText(prompt, window,
						 JXWidget::kFixedLeft, JXWidget::kFixedTop,
						 kHMarginWidth,y, 0,0);
	assert( instrText != nullptr );

	y += instrText->GetFrameHeight() + kItemVDelta;

	// radio group

	const JCoordinate kInitRGWidth = 10;	// arbitrary, >0

	itsRG =
		jnew JXRadioGroup(window, JXWidget::kFixedLeft, JXWidget::kFixedTop,
						 kHMarginWidth,y,
						 kInitRGWidth, kItemVDelta + actionCount * kItemVSeparation);
	assert( itsRG != nullptr );

	// choices

	JCoordinate wmin = 0;
	JPtrArray<JXRadioButton> buttonList(JPtrArrayT::kForgetAll, actionCount);
	for (JIndex i=1; i<=actionCount; i++)
		{
		auto* button =
			jnew JXTextRadioButton(i, *choiceList.GetElement(i), itsRG,
								  JXWidget::kFixedLeft, JXWidget::kFixedTop,
								  kRGHLMarginWidth, kItemVDelta + (i-1) * kItemVSeparation,
								  10,kTextHeight);
		assert( button != nullptr );

		if (shortcutList != nullptr)
			{
			button->SetShortcuts(*(shortcutList->GetElement(i)));
			}

		buttonList.Append(button);
		wmin = JMax(button->GetPreferredWidth(), wmin);
		}

	// all choices should be the same width

	for (JXRadioButton* rb : buttonList)
		{
		rb->SetSize(wmin, kTextHeight);
		}

	wmin += kRGHLMarginWidth + kRGHRMarginWidth;
	itsRG->AdjustSize(wmin - kInitRGWidth, 0);

	const JCoordinate wminInstr = instrText->GetFrameWidth();
	if (wmin < wminInstr)
		{
		const JCoordinate delta = (wminInstr - wmin)/2;
		itsRG->Move(delta, 0);
		wmin = wminInstr;
		}

	y = (itsRG->GetFrame()).bottom + kRGButtonVDelta;

	// OK and Cancel buttons

	wmin += 2*kHMarginWidth;
	const JCoordinate wminButton = 3*kMinButtonHMargin + 2*kButtonWidth;
	if (wmin < wminButton)
		{
		const JCoordinate delta = (wminButton - wmin)/2;
		instrText->Move(delta, 0);
		itsRG->Move(delta, 0);
		wmin = wminButton;
		}

	const JCoordinate buttonX = (wmin - 2*kButtonWidth)/3;

	auto* cancelButton =
		jnew JXTextButton(JGetString("CancelLabel::JXGlobal"), window,
						 JXWidget::kFixedLeft, JXWidget::kFixedTop,
						 buttonX,y, kButtonWidth,kTextHeight);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("OKLabel::JXGlobal"), window,
						 JXWidget::kFixedLeft, JXWidget::kFixedTop,
						 wmin - buttonX - (kButtonWidth+2), y-1,
						 kButtonWidth+2, kTextHeight+2);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("OKShortcut::JXGlobal"));

	SetButtons(okButton, cancelButton);

	// window size

	window->SetSize(wmin, y + kItemVSeparation);
}
