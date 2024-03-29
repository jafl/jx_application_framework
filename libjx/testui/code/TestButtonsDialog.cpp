/******************************************************************************
TestButtonsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Written by John Lindal.

******************************************************************************/

#include "TestButtonsDialog.h"
#include "SmileyBitmaps.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXStaticText.h>

#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXImageButton.h>

#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXImageCheckbox.h>
#include <jx-af/jx/JXAtLeastOneCBGroup.h>

#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXImageRadioButton.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

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

TestButtonsDialog::TestButtonsDialog()
	:
	JXModalDialogDirector()
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

	auto* window = jnew JXWindow(this, 270,330, JGetString("WindowTitle::TestButtonsDialog::JXLayout"));

	auto* rg1Label =
		jnew JXStaticText(JGetString("rg1Label::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 150,20, 104,20);
	rg1Label->SetToLabel(false);

	auto* tcb =
		jnew JXTextCheckbox(JGetString("tcb::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,25, 120,20);
	assert( tcb != nullptr );

	itsRG1 =
		jnew JXRadioGroup(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 150,40, 104,134);

	auto* dtcb =
		jnew JXTextCheckbox(JGetString("dtcb::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 120,20);
	assert( dtcb != nullptr );

	auto* trb1 =
		jnew JXTextRadioButton(1, JGetString("trb1::TestButtonsDialog::JXLayout"), itsRG1,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 80,20);
	assert( trb1 != nullptr );

	auto* trb2 =
		jnew JXTextRadioButton(2, JGetString("trb2::TestButtonsDialog::JXLayout"), itsRG1,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 80,20);
	assert( trb2 != nullptr );

	auto* bcb =
		jnew JXImageCheckbox(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,90, 20,20);
	assert( bcb != nullptr );

	auto* imageLabel =
		jnew JXStaticText(JGetString("imageLabel::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,110, 100,20);
	imageLabel->SetToLabel(false);

	auto* trb3 =
		jnew JXTextRadioButton(3, JGetString("trb3::TestButtonsDialog::JXLayout"), itsRG1,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 80,20);
	assert( trb3 != nullptr );

	auto* rg2Label =
		jnew JXStaticText(JGetString("rg2Label::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,134, 102,20);
	rg2Label->SetToLabel(false);

	auto* dtrb =
		jnew JXTextRadioButton(4, JGetString("dtrb::TestButtonsDialog::JXLayout"), itsRG1,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,100, 80,20);
	assert( dtrb != nullptr );

	itsRG2 =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,155, 102,36);

	brb[0] =
		jnew JXImageRadioButton(1, itsRG2,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 5,5, 22,22);

	brb[1] =
		jnew JXImageRadioButton(2, itsRG2,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 27,5, 22,22);

	brb[2] =
		jnew JXImageRadioButton(3, itsRG2,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 49,5, 22,22);

	brb[3] =
		jnew JXImageRadioButton(4, itsRG2,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 71,5, 22,22);

	auto* cbGroupLabel =
		jnew JXStaticText(JGetString("cbGroupLabel::TestButtonsDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,210, 240,20);
	cbGroupLabel->SetToLabel(false);

	itsEnable1CB =
		jnew JXTextCheckbox(JGetString("itsEnable1CB::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,230, 80,20);

	itsEnable2CB =
		jnew JXTextCheckbox(JGetString("itsEnable2CB::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,230, 80,20);

	itsEnable3CB =
		jnew JXTextCheckbox(JGetString("itsEnable3CB::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,230, 80,20);

	its1CB =
		jnew JXTextCheckbox(JGetString("its1CB::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,250, 80,20);

	its2CB =
		jnew JXTextCheckbox(JGetString("its2CB::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,250, 80,20);

	its3CB =
		jnew JXTextCheckbox(JGetString("its3CB::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,250, 80,20);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::TestButtonsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,290, 70,30);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXImageButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 160,290, 70,30);
	assert( okButton != nullptr );

// end JXLayout

	SetButtons(okButton, cancelButton);

	cancelButton->SetFontName("Times");
	const JFontStyle cancelButton_style(true, false, 0, false, JColorManager::GetRedColor());
	cancelButton->SetFontStyle(cancelButton_style);

	imageLabel->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	const JFontStyle imageLabel_style(false, false, 0, false, JColorManager::GetGreenColor());
	imageLabel->SetFontStyle(imageLabel_style);

	tcb->SetNormalColor(JColorManager::GetRedColor());
	tcb->SetPushedColor(JColorManager::GetBlueColor());

	itsEnable1CB->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	itsEnable2CB->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	itsEnable3CB->SetFontSize(JFontManager::GetDefaultFontSize()-2);

	rg1Label->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	rg2Label->SetFontSize(JFontManager::GetDefaultFontSize()-2);

	dtcb->SetNormalColor(JColorManager::GetRedColor());
	dtcb->SetPushedColor(JColorManager::GetBlueColor());
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

	for (JUnsignedOffset i=0; i<kSmileyBitmapCount; i++)
	{
		brb[i]->SetBitmap(kSmileyBitmap[i], kRadioButtonColor[i]);
		brb[i]->SetHint(JGetString(kRadioButtonHintID[i]));
	}

	ListenTo(itsRG1, std::function([](const JXRadioGroup::SelectionChanged& msg)
	{
		if (msg.GetID() == 2)
		{
			JGetUserNotification()->DisplayMessage(
				JGetString("GoodChoice::TestButtonsDialog"));
		}
	}));

	itsEnable1CB->SetState(true);
	ListenTo(itsEnable1CB, std::function([this](const JXCheckbox::Pushed& msg)
	{
		its1CB->SetActive(msg.IsChecked());
	}));

	itsEnable2CB->SetState(true);
	ListenTo(itsEnable2CB, std::function([this](const JXCheckbox::Pushed& msg)
	{
		its2CB->SetActive(msg.IsChecked());
	}));

	itsEnable3CB->SetState(true);
	ListenTo(itsEnable3CB, std::function([this](const JXCheckbox::Pushed& msg)
	{
		its3CB->SetActive(msg.IsChecked());
	}));

	jnew JXAtLeastOneCBGroup(3, its1CB, its2CB, its3CB);
}

/******************************************************************************
OKToDeactivate (virtual protected)

******************************************************************************/

bool
TestButtonsDialog::OKToDeactivate()
{
	if (!JXModalDialogDirector::OKToDeactivate())
	{
		return false;
	}

	if (!Cancelled() && itsRG2->GetSelectedItem() == kSmileyBitmapCount)
	{
		JGetUserNotification()->DisplayMessage(JGetString("Encouragement::TestButtonsDialog"));
	}

	return true;
}
