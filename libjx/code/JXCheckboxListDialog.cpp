/******************************************************************************
 JXCheckboxListDialog.cpp

	Dynamically constructed dialog window to ask the user to pick from
	a list of choices.

	BASE CLASS = JXDialogDirector

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#include <JXCheckboxListDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXStaticText.h>
#include <jGlobals.h>
#include <JMinMax.h>
#include <jAssert.h>

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

	shortcutList can be NULL.

 ******************************************************************************/

JXCheckboxListDialog::JXCheckboxListDialog
	(
	JXDirector*					supervisor,
	const JString&				windowTitle,
	const JString&				prompt,
	const JPtrArray<JString>&	choiceList,
	const JPtrArray<JString>*	shortcutList
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsCBList = jnew JPtrArray<JXCheckbox>(JPtrArrayT::kForgetAll);
	assert( itsCBList != NULL );

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

JBoolean
JXCheckboxListDialog::GetSelectedItems
	(
	JArray<JIndex>* indexList
	)
	const
{
	indexList->RemoveAll();

	const JSize count = itsCBList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (itsCBList->GetElement(i)->IsChecked())
			{
			indexList->AppendElement(i);
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
	itsCBList->GetElement(index)->SetState(kJTrue);
}

/******************************************************************************
 SelectAllItems

 ******************************************************************************/

void
JXCheckboxListDialog::SelectAllItems()
{
	const JSize count = itsCBList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		itsCBList->GetElement(i)->SetState(kJTrue);
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
	const JSize actionCount = choiceList.GetElementCount();

	JXWindow* window = jnew JXWindow(this, 10,10, windowTitle);
	assert( window != NULL );

	JCoordinate y = kFirstItemTop;

	// instructions

	JXStaticText* instrText =
		jnew JXStaticText(prompt, window,
						  JXWidget::kFixedLeft, JXWidget::kFixedTop,
						  kHMarginWidth,y, 0,0);
	assert( instrText != NULL );

	y += instrText->GetFrameHeight() + kItemVDelta;

	// choices

	JCoordinate wmin = 0;
	for (JIndex i=1; i<=actionCount; i++)
		{
		JXTextCheckbox* cb =
			jnew JXTextCheckbox(*(choiceList.GetElement(i)), window,
								JXWidget::kFixedLeft, JXWidget::kFixedTop,
								kCBHLMarginWidth, y + (i-1) * kItemVSeparation,
								10, kTextHeight);
		assert( cb != NULL );

		if (shortcutList != NULL)
			{
			cb->SetShortcuts(*(shortcutList->GetElement(i)));
			}

		itsCBList->Append(cb);
		wmin = JMax(cb->GetPreferredWidth(), wmin);
		}

	// all choices should be the same width

	for (JIndex i=1; i<=actionCount; i++)
		{
		(itsCBList->GetElement(i))->SetSize(wmin, kTextHeight);
		}

	wmin += kCBHLMarginWidth + kCBHRMarginWidth;
	wmin  = JMax(wmin, 2*kHMarginWidth + instrText->GetFrameWidth());

	y += actionCount * kItemVSeparation + kButtonVDelta;

	// OK and Cancel buttons

	wmin = JMax(wmin, 3*kMinButtonHMargin + 2*kButtonWidth);

	const JCoordinate buttonX = (wmin - 2*kButtonWidth)/3;

	JXTextButton* cancelButton =
		jnew JXTextButton("Cancel", window,
						 JXWidget::kFixedLeft, JXWidget::kFixedTop,
						 buttonX,y, kButtonWidth,kTextHeight);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton("OK", window,
						 JXWidget::kFixedLeft, JXWidget::kFixedTop,
						 wmin - buttonX - (kButtonWidth+2), y-1,
						 kButtonWidth+2, kTextHeight+2);
	assert( okButton != NULL );
	okButton->SetShortcuts("^M");

	SetButtons(okButton, cancelButton);

	// window size

	window->SetSize(wmin, y + kItemVSeparation);
}
