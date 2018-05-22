/******************************************************************************
TestButtonsDialog.cpp

	BASE CLASS = JXDialogDirector

	Written by John Lindal.

******************************************************************************/

#include "TestButtonsDialog.h"
#include "SmileyBitmaps.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXColorManager.h>

#include <JXTextButton.h>
#include <JXImageButton.h>

#include <JXTextCheckbox.h>
#include <JXImageCheckbox.h>
#include <JXAtLeastOneCBGroup.h>

#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXImageRadioButton.h>
#include <jXGlobals.h>
#include <jAssert.h>

static unsigned char okButtonData[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x40, 0x10, 0x03, 0x80, 0xc1,
	0x40, 0x08, 0x03, 0x60, 0x00, 0x43, 0x08, 0x03, 0x20, 0x00, 0x42, 0x04,
	0x03, 0x10, 0x00, 0x44, 0x02, 0x03, 0x10, 0x00, 0x44, 0x01, 0x03, 0x08,
	0x00, 0x48, 0x01, 0x03, 0x08, 0x00, 0xc8, 0x00, 0x03, 0x08, 0x00, 0x48,
	0x00, 0x03, 0x08, 0x00, 0xc8, 0x00, 0x03, 0x08, 0x00, 0x48, 0x01, 0x03,
	0x10, 0x00, 0x44, 0x01, 0x03, 0x10, 0x00, 0x44, 0x02, 0x03, 0x20, 0x00,
	0x42, 0x04, 0x00, 0x60, 0x00, 0x43, 0x08, 0x00, 0x80, 0xc1, 0x40, 0x08,
	0x03, 0x00, 0x3e, 0x40, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static const JConstBitmap okButtonBitmap = { 40,20, okButtonData };


static unsigned char checkboxData[] =
{
	0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0xe0, 0x03, 0x00, 0xf0, 0x03,
	0x00, 0xf8, 0x03, 0x00, 0xfc, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x1e, 0x00,
	0x00, 0x0f, 0x00, 0x00, 0x07, 0x00, 0x8e, 0x03, 0x00, 0x9f, 0x03, 0x00,
	0xfe, 0x01, 0x00, 0xf8, 0x01, 0x00, 0xf0, 0x01, 0x00, 0xe0, 0x00, 0x00,
	0xc0, 0x00, 0x00, 0x40, 0x00, 0x00
};
static const JConstBitmap checkboxBitmap = { 18,18, checkboxData };

/******************************************************************************
Constructor

******************************************************************************/

TestButtonsDialog::TestButtonsDialog
	(
	JXWindowDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow();
}

/******************************************************************************
Destructor

******************************************************************************/

TestButtonsDialog::~TestButtonsDialog()
{
}

/******************************************************************************
BuildWindow (private)

******************************************************************************/

void
TestButtonsDialog::BuildWindow()
{
	JXImageRadioButton* brb[ kSmileyBitmapCount ];

// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 270,330, JString::empty);
	assert( window != NULL );

	JXStaticText* rg1Label =
		jnew JXStaticText(JGetString("rg1Label::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 150,20, 104,20);
	assert( rg1Label != NULL );
	rg1Label->SetFontSize(JGetDefaultFontSize()-2);
	rg1Label->SetToLabel();

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,290, 70,30);
	assert( cancelButton != NULL );
	cancelButton->SetFontName(JGetString("TimesFontName::TestButtonsDialog::JXLayout"));
	const JFontStyle cancelButton_style(kJTrue, kJFalse, 0, kJFalse, JColorManager::GetRedColor());
	cancelButton->SetFontStyle(cancelButton_style);

	JXImageButton* okButton =
		jnew JXImageButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 160,290, 70,30);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::TestButtonsDialog::shortcuts::JXLayout"));

	JXTextCheckbox* tcb =
		jnew JXTextCheckbox(JGetString("tcb::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,25, 120,20);
	assert( tcb != NULL );
	tcb->SetNormalColor(JColorManager::GetRedColor());
	tcb->SetPushedColor(JColorManager::GetBlueColor());

	JXImageCheckbox* bcb =
		jnew JXImageCheckbox(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,85, 22,22);
	assert( bcb != NULL );

	itsRG1 =
		jnew JXRadioGroup(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 150,40, 104,134);
	assert( itsRG1 != NULL );

	itsRG2 =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,155, 102,36);
	assert( itsRG2 != NULL );

	brb[0] =
		jnew JXImageRadioButton(1, itsRG2,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 5,5, 22,22);
	assert( brb[0] != NULL );

	brb[1] =
		jnew JXImageRadioButton(2, itsRG2,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 27,5, 22,22);
	assert( brb[1] != NULL );

	brb[2] =
		jnew JXImageRadioButton(3, itsRG2,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 49,5, 22,22);
	assert( brb[2] != NULL );

	brb[3] =
		jnew JXImageRadioButton(4, itsRG2,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 71,5, 22,22);
	assert( brb[3] != NULL );

	JXTextRadioButton* trb1 =
		jnew JXTextRadioButton(1, JGetString("trb1::TestButtonsDialog::JXLayout"), itsRG1,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 80,20);
	assert( trb1 != NULL );

	JXTextRadioButton* trb2 =
		jnew JXTextRadioButton(2, JGetString("trb2::TestButtonsDialog::JXLayout"), itsRG1,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 80,20);
	assert( trb2 != NULL );

	JXTextRadioButton* trb3 =
		jnew JXTextRadioButton(3, JGetString("trb3::TestButtonsDialog::JXLayout"), itsRG1,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 80,20);
	assert( trb3 != NULL );

	JXStaticText* imageLabel =
		jnew JXStaticText(JGetString("imageLabel::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,110, 90,20);
	assert( imageLabel != NULL );
	imageLabel->SetFontSize(JGetDefaultFontSize()-2);
	const JFontStyle imageLabel_style(kJFalse, kJFalse, 0, kJFalse, JColorManager::GetGreenColor());
	imageLabel->SetFontStyle(imageLabel_style);
	imageLabel->SetToLabel();

	JXTextCheckbox* dtcb =
		jnew JXTextCheckbox(JGetString("dtcb::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 120,20);
	assert( dtcb != NULL );
	dtcb->SetNormalColor(JColorManager::GetRedColor());
	dtcb->SetPushedColor(JColorManager::GetBlueColor());

	JXTextRadioButton* dtrb =
		jnew JXTextRadioButton(4, JGetString("dtrb::TestButtonsDialog::JXLayout"), itsRG1,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,100, 80,20);
	assert( dtrb != NULL );

	itsEnable1CB =
		jnew JXTextCheckbox(JGetString("itsEnable1CB::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,230, 80,20);
	assert( itsEnable1CB != NULL );
	itsEnable1CB->SetFontSize(JGetDefaultFontSize()-2);

	JXStaticText* cbGroupLabel =
		jnew JXStaticText(JGetString("cbGroupLabel::TestButtonsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,210, 240,20);
	assert( cbGroupLabel != NULL );
	cbGroupLabel->SetToLabel();

	its1CB =
		jnew JXTextCheckbox(JGetString("its1CB::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,250, 80,20);
	assert( its1CB != NULL );

	itsEnable2CB =
		jnew JXTextCheckbox(JGetString("itsEnable2CB::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,230, 80,20);
	assert( itsEnable2CB != NULL );
	itsEnable2CB->SetFontSize(JGetDefaultFontSize()-2);

	its2CB =
		jnew JXTextCheckbox(JGetString("its2CB::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,250, 80,20);
	assert( its2CB != NULL );

	itsEnable3CB =
		jnew JXTextCheckbox(JGetString("itsEnable3CB::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,230, 80,20);
	assert( itsEnable3CB != NULL );
	itsEnable3CB->SetFontSize(JGetDefaultFontSize()-2);

	its3CB =
		jnew JXTextCheckbox(JGetString("its3CB::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,250, 80,20);
	assert( its3CB != NULL );

	JXStaticText* rg2Label =
		jnew JXStaticText(JGetString("rg2Label::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,134, 102,20);
	assert( rg2Label != NULL );
	rg2Label->SetFontSize(JGetDefaultFontSize()-2);
	rg2Label->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::TestButtonsDialog"));
	SetButtons(okButton, cancelButton);

	dtcb->Deactivate();
	dtrb->Deactivate();

	okButton->SetBitmap(okButtonBitmap, JColorManager::GetBlueColor());
	bcb->SetBitmap(checkboxBitmap, JColorManager::GetGreenColor());
	bcb->SetHint(JGetString("ImageCBHint::TestButtonsDialog"));

	static const JColorID kRadioButtonColor[] =
	{
		JColorManager::GetWhiteColor(),
		JColorManager::GetRedColor(),
		JColorManager::GetBlueColor(),
		JColorManager::GetBlackColor()
	};

	static const JUtf8Byte* kRadioButtonHintID[] =
	{
		"ImageRB1Hint::TestButtonsDialog",
		"ImageRB2Hint::TestButtonsDialog",
		"ImageRB3Hint::TestButtonsDialog",
		"ImageRB4Hint::TestButtonsDialog"
	};

	for (JIndex i=0; i<kSmileyBitmapCount; i++)
		{
		brb[i]->SetBitmap(kSmileyBitmap[i], kRadioButtonColor[i]);
		brb[i]->SetHint(JGetString(kRadioButtonHintID[i]));
		}

	ListenTo(itsRG1);

	itsEnable1CB->SetState(kJTrue);
	ListenTo(itsEnable1CB);
	itsEnable2CB->SetState(kJTrue);
	ListenTo(itsEnable2CB);
	itsEnable3CB->SetState(kJTrue);
	ListenTo(itsEnable3CB);

	JXAtLeastOneCBGroup* cbGroup = jnew JXAtLeastOneCBGroup(3, its1CB, its2CB, its3CB);
	assert( cbGroup != NULL );
}

/******************************************************************************
Receive (protected)

******************************************************************************/

void
TestButtonsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsRG1 && message.Is(JXRadioGroup::kSelectionChanged))
		{
		const JXRadioGroup::SelectionChanged* selection =
			dynamic_cast<const JXRadioGroup::SelectionChanged*>(&message);
		assert( selection != NULL );
		if (selection->GetID() == 2)
			{
			(JGetUserNotification())->DisplayMessage(
				JGetString("GoodChoice::TestButtonsDialog"));
			}
		}
	else if (sender == itsEnable1CB && message.Is(JXCheckbox::kPushed))
		{
		its1CB->SetActive(itsEnable1CB->IsChecked());
		}
	else if (sender == itsEnable2CB && message.Is(JXCheckbox::kPushed))
		{
		its2CB->SetActive(itsEnable2CB->IsChecked());
		}
	else if (sender == itsEnable3CB && message.Is(JXCheckbox::kPushed))
		{
		its3CB->SetActive(itsEnable3CB->IsChecked());
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
OKToDeactivate (virtual protected)

******************************************************************************/

JBoolean
TestButtonsDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}

	if (!Cancelled() && itsRG2->GetSelectedItem() == kSmileyBitmapCount)
		{
		(JGetUserNotification())->DisplayMessage(JGetString("Encouragement::TestButtonsDialog"));
		}

	return kJTrue;
}
