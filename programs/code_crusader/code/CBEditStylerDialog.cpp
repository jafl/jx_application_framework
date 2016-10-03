/******************************************************************************
 CBEditStylerDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBEditStylerDialog.h"
#include "CBStylerTable.h"

#if defined CODE_CRUSADER
#include "cbHelpText.h"
#endif

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXHelpManager.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jAssert.h>

const JCoordinate kMaxTypeRowCount = 10;

static const JCharacter* kInstructionsID = "Instructions::CBEditStylerDialog";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBEditStylerDialog::CBEditStylerDialog
	(
	const JCharacter*						windowTitle,
	const JBoolean							active,
	const JCharacter**						typeNames,
	const JArray<JFontStyle>&				typeStyles,
	const JArray<CBStylerBase::WordStyle>&	wordList,
	const CBTextFileType					fileType
	)
	:
	JXDialogDirector(JXGetApplication(), kJTrue)
{
	BuildWindow(windowTitle, active, typeNames, typeStyles, wordList, fileType);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEditStylerDialog::~CBEditStylerDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBEditStylerDialog::BuildWindow
	(
	const JCharacter*						windowTitle,
	const JBoolean							active,
	const JCharacter**						typeNames,
	const JArray<JFontStyle>&				typeStyles,
	const JArray<CBStylerBase::WordStyle>&	wordList,
	const CBTextFileType					fileType
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 370,390, "");
	assert( window != NULL );

	JXScrollbarSet* scrollbarSet1 =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,110, 240,110);
	assert( scrollbarSet1 != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,360, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 250,360, 70,20);
	assert( okButton != NULL );

	JXStaticText* instrText =
		new JXStaticText(JGetString("instrText::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 330,50);
	assert( instrText != NULL );

	JXTextButton* newWordButton =
		new JXTextButton(JGetString("newWordButton::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,240, 70,20);
	assert( newWordButton != NULL );
	newWordButton->SetShortcuts(JGetString("newWordButton::CBEditStylerDialog::shortcuts::JXLayout"));

	JXTextButton* removeButton =
		new JXTextButton(JGetString("removeButton::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,270, 70,20);
	assert( removeButton != NULL );
	removeButton->SetShortcuts(JGetString("removeButton::CBEditStylerDialog::shortcuts::JXLayout"));

	JXScrollbarSet* scrollbarSet2 =
		new JXScrollbarSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,230, 240,110);
	assert( scrollbarSet2 != NULL );

	itsActiveCB =
		new JXTextCheckbox(JGetString("itsActiveCB::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,15, 220,20);
	assert( itsActiveCB != NULL );

	itsHelpButton =
		new JXTextButton(JGetString("itsHelpButton::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 145,360, 70,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditStylerDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(windowTitle);
	SetButtons(okButton, cancelButton);

	itsActiveCB->SetState(active);
	instrText->SetText(JGetString(kInstructionsID));
	ListenTo(itsHelpButton);

	// create tables

	itsTypeTable =
		new CBStylerTable(typeNames, typeStyles, scrollbarSet1,
						  scrollbarSet1->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsTypeTable != NULL );
	itsTypeTable->FitToEnclosure();

	itsWordTable =
		new CBStylerTable(fileType, wordList, newWordButton, removeButton,
						  scrollbarSet2, scrollbarSet2->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsWordTable != NULL );
	itsWordTable->FitToEnclosure();

	// adjust window size

	JCoordinate rowBorderWidth;
	JColorIndex rowBorderColor;
	itsTypeTable->GetRowBorderInfo(&rowBorderWidth, &rowBorderColor);

	const JCoordinate bdh =
		JMin(itsTypeTable->GetBoundsHeight(),
			 kMaxTypeRowCount * itsTypeTable->GetDefaultRowHeight() +
			 (kMaxTypeRowCount-1) * rowBorderWidth);
	const JCoordinate aph = itsTypeTable->GetApertureHeight();
	window->AdjustSize(0, bdh - aph);

	scrollbarSet1->SetSizing(JXWidget::kHElastic, JXWidget::kFixedTop);
	scrollbarSet2->SetSizing(JXWidget::kHElastic, JXWidget::kVElastic);

	okButton->SetSizing(JXWidget::kFixedLeft, JXWidget::kFixedBottom);
	cancelButton->SetSizing(JXWidget::kFixedLeft, JXWidget::kFixedBottom);
	itsHelpButton->SetSizing(JXWidget::kFixedLeft, JXWidget::kFixedBottom);

	newWordButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
	removeButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);

	UseModalPlacement(kJFalse);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
}

/******************************************************************************
 GetData

	colorList can be NULL.

 ******************************************************************************/

void
CBEditStylerDialog::GetData
	(
	JBoolean*				active,
	JArray<JFontStyle>*		typeStyles,
	JStringMap<JFontStyle>*	wordStyles
	)
	const
{
	*active = itsActiveCB->IsChecked();

	itsTypeTable->GetData(typeStyles);
	itsWordTable->GetData(wordStyles);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBEditStylerDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		#if defined CODE_CRUSADER
		(JXGetHelpManager())->ShowSection(kCBStylerHelpName);
		#endif
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
