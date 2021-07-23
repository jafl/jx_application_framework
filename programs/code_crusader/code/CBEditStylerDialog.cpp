/******************************************************************************
 CBEditStylerDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBEditStylerDialog.h"
#include "CBStylerTable.h"
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

/******************************************************************************
 Constructor

 ******************************************************************************/

CBEditStylerDialog::CBEditStylerDialog
	(
	const JString&							windowTitle,
	const bool								active,
	const JUtf8Byte**						typeNames,
	const JArray<JFontStyle>&				typeStyles,
	const JArray<CBStylerBase::WordStyle>&	wordList,
	const CBTextFileType					fileType
	)
	:
	JXDialogDirector(JXGetApplication(), true)
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
	const JString&							windowTitle,
	const bool								active,
	const JUtf8Byte**						typeNames,
	const JArray<JFontStyle>&				typeStyles,
	const JArray<CBStylerBase::WordStyle>&	wordList,
	const CBTextFileType					fileType
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,390, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet1 =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,110, 240,110);
	assert( scrollbarSet1 != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,360, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 250,360, 70,20);
	assert( okButton != nullptr );

	auto* instrText =
		jnew JXStaticText(JGetString("instrText::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 330,50);
	assert( instrText != nullptr );

	auto* newWordButton =
		jnew JXTextButton(JGetString("newWordButton::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,240, 70,20);
	assert( newWordButton != nullptr );
	newWordButton->SetShortcuts(JGetString("newWordButton::CBEditStylerDialog::shortcuts::JXLayout"));

	auto* removeButton =
		jnew JXTextButton(JGetString("removeButton::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 280,270, 70,20);
	assert( removeButton != nullptr );
	removeButton->SetShortcuts(JGetString("removeButton::CBEditStylerDialog::shortcuts::JXLayout"));

	auto* scrollbarSet2 =
		jnew JXScrollbarSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,230, 240,110);
	assert( scrollbarSet2 != nullptr );

	itsActiveCB =
		jnew JXTextCheckbox(JGetString("itsActiveCB::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,15, 220,20);
	assert( itsActiveCB != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBEditStylerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 145,360, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditStylerDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(windowTitle);
	SetButtons(okButton, cancelButton);

	itsActiveCB->SetState(active);
	instrText->GetText()->SetText(JGetString("Instructions::CBEditStylerDialog"));
	ListenTo(itsHelpButton);

	// create tables

	itsTypeTable =
		jnew CBStylerTable(typeNames, typeStyles, scrollbarSet1,
						  scrollbarSet1->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsTypeTable != nullptr );
	itsTypeTable->FitToEnclosure();

	itsWordTable =
		jnew CBStylerTable(fileType, wordList, newWordButton, removeButton,
						  scrollbarSet2, scrollbarSet2->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsWordTable != nullptr );
	itsWordTable->FitToEnclosure();

	// adjust window size

	JCoordinate rowBorderWidth;
	JColorID rowBorderColor;
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

	UseModalPlacement(false);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
}

/******************************************************************************
 GetData

	colorList can be nullptr.

 ******************************************************************************/

void
CBEditStylerDialog::GetData
	(
	bool*				active,
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
		(JXGetHelpManager())->ShowSection("CBStylerHelp");
		#endif
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
