/******************************************************************************
 JXCheckboxListDialog.cpp

	Dynamically constructed dialog window to ask the user to pick from
	a list of choices.

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#include "JXCheckboxListDialog.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXTextCheckbox.h"
#include "JXStaticText.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kFirstItemTop     = 20;
const JCoordinate kItemVDelta       = 10;
const JCoordinate kTextHeight       = 20;
const JCoordinate kItemVSeparation  = kTextHeight + kItemVDelta;
const JCoordinate kButtonVDelta     = 20;
const JCoordinate kButtonWidth      = 60;
const JCoordinate kHMarginWidth     = 20;
const JCoordinate kCBHLMarginWidth  = 10 + kHMarginWidth;
const JCoordinate kCBHRMarginWidth  = 15 + kHMarginWidth;
const JCoordinate kMinButtonHMargin = 10;

/******************************************************************************
 Constructor

	shortcutList can be nullptr.

 ******************************************************************************/

JXCheckboxListDialog::JXCheckboxListDialog
	(
	const JString&				windowTitle,
	const JString&				prompt,
	const JPtrArray<JString>&	choiceList,
	const JPtrArray<JString>*	shortcutList
	)
	:
	JXModalDialogDirector()
{
	itsCBList = jnew JPtrArray<JXCheckbox>(JPtrArrayT::kForgetAll);
	assert( itsCBList != nullptr );

	BuildWindow(windowTitle, prompt, choiceList, shortcutList);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCheckboxListDialog::~JXCheckboxListDialog()
{
	jdelete itsCBList;
}

/******************************************************************************
 GetSelectedItems

 ******************************************************************************/

bool
JXCheckboxListDialog::GetSelectedItems
	(
	JArray<JIndex>* indexList
	)
	const
{
	indexList->RemoveAll();

	const JSize count = itsCBList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (itsCBList->GetItem(i)->IsChecked())
		{
			indexList->AppendItem(i);
		}
	}

	return ! indexList->IsEmpty();
}

/******************************************************************************
 SelectItem

 ******************************************************************************/

void
JXCheckboxListDialog::SelectItem
	(
	const JIndex index
	)
{
	itsCBList->GetItem(index)->SetState(true);
}

/******************************************************************************
 SelectAllItems

 ******************************************************************************/

void
JXCheckboxListDialog::SelectAllItems()
{
	for (auto* cb : *itsCBList)
	{
		cb->SetState(true);
	}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
JXCheckboxListDialog::BuildWindow
	(
	const JString&				windowTitle,
	const JString&				prompt,
	const JPtrArray<JString>&	choiceList,
	const JPtrArray<JString>*	shortcutList
	)
{
	const JSize actionCount = choiceList.GetItemCount();

	auto* window = jnew JXWindow(this, 10,10, windowTitle);

	JCoordinate y = kFirstItemTop;

	// instructions

	auto* instrText =
		jnew JXStaticText(prompt, window,
						  JXWidget::kFixedLeft, JXWidget::kFixedTop,
						  kHMarginWidth,y, 0,0);

	y += instrText->GetFrameHeight() + kItemVDelta;

	// choices

	JCoordinate wmin = 0;
	for (JIndex i=1; i<=actionCount; i++)
	{
		auto* cb =
			jnew JXTextCheckbox(*(choiceList.GetItem(i)), window,
								JXWidget::kFixedLeft, JXWidget::kFixedTop,
								kCBHLMarginWidth, y + (i-1) * kItemVSeparation,
								10, kTextHeight);
		assert( cb != nullptr );

		if (shortcutList != nullptr)
		{
			cb->SetShortcuts(*(shortcutList->GetItem(i)));
		}

		itsCBList->Append(cb);
		wmin = JMax(cb->GetPreferredWidth(), wmin);
	}

	// all choices should be the same width

	for (auto* cb : *itsCBList)
	{
		cb->SetSize(wmin, kTextHeight);
	}

	wmin += kCBHLMarginWidth + kCBHRMarginWidth;
	wmin  = JMax(wmin, 2*kHMarginWidth + instrText->GetFrameWidth());

	y += actionCount * kItemVSeparation + kButtonVDelta;

	// OK and Cancel buttons

	wmin = JMax(wmin, 3*kMinButtonHMargin + 2*kButtonWidth);

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
