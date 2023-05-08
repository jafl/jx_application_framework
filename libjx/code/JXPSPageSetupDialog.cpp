/******************************************************************************
 JXPSPageSetupDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include "JXPSPageSetupDialog.h"
#include "JXPSPrinter.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXStaticText.h"
#include "JXTextMenu.h"
#include "JXRadioGroup.h"
#include "JXImageRadioButton.h"
#include "jXGlobals.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JConstBitmap.h>
#include <jx-af/jcore/jAssert.h>

// mappings from dialog window to JPSPrinter options

static const JPSPrinter::PaperType kIndexToPaperType[] =
{
	JPSPrinter::kUSLetter, JPSPrinter::kUSLegal, JPSPrinter::kUSExecutive,
	JPSPrinter::kA4Letter, JPSPrinter::kB5Letter
};

static const JUtf8Byte* kPaperMenuStr =
	"US Letter%r|US Legal%r|US Executive%r|A4 Letter%r|B5 Letter%r";
static const JSize kPaperTypeCount =
	sizeof(kIndexToPaperType)/sizeof(JPSPrinter::PaperType);

static const JPSPrinter::ImageOrientation kIndexToOrient[] =
{
	JPSPrinter::kPortrait, JPSPrinter::kLandscape
};
static const JSize kOrientCount =
	sizeof(kIndexToOrient)/sizeof(JPSPrinter::ImageOrientation);

// bitmaps

static unsigned char kPortraitData[] =
{
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x00,
   0x10, 0x02, 0x80, 0x01, 0x38, 0x02, 0x8e, 0x02, 0x7c, 0x02, 0x91, 0x04,
   0xfe, 0x02, 0x91, 0x0f, 0x38, 0x02, 0x11, 0x08, 0x38, 0x02, 0x0a, 0x08,
   0x38, 0xc2, 0x71, 0x08, 0x38, 0x22, 0x80, 0x08, 0x38, 0x22, 0x80, 0x08,
   0x38, 0x22, 0x80, 0x08, 0x38, 0xa2, 0xa0, 0x08, 0x38, 0xa2, 0xa0, 0x08,
   0x38, 0xa2, 0xa0, 0x08, 0x38, 0xa2, 0xa0, 0x08, 0x00, 0xa2, 0xa0, 0x08,
   0x28, 0xe2, 0xee, 0x08, 0x00, 0x82, 0x2a, 0x08, 0x28, 0x82, 0x2a, 0x08,
   0x00, 0x82, 0x2a, 0x08, 0x28, 0x82, 0x2a, 0x08, 0x00, 0x82, 0x2a, 0x08,
   0x28, 0x82, 0x2a, 0x08, 0x00, 0x82, 0x3b, 0x08, 0x00, 0x02, 0x00, 0x08,
   0x00, 0xfe, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const JConstBitmap kPortraitBitmap = { 30,30, kPortraitData };

static unsigned char kLandscapeData[] =
{
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x00,
   0x10, 0x02, 0x80, 0x01, 0x38, 0x02, 0x80, 0x02, 0x7c, 0x02, 0x80, 0x04,
   0xfe, 0x02, 0x80, 0x0f, 0x38, 0x02, 0x00, 0x08, 0x38, 0x02, 0x00, 0x08,
   0x38, 0x02, 0x00, 0x08, 0x38, 0xfa, 0x07, 0x08, 0x38, 0x0a, 0x08, 0x08,
   0x38, 0xfe, 0x08, 0x08, 0x38, 0x02, 0xe8, 0x08, 0x38, 0x02, 0x10, 0x09,
   0x38, 0x02, 0x00, 0x09, 0x38, 0x02, 0x10, 0x09, 0x00, 0x02, 0xe8, 0x08,
   0x28, 0xfe, 0x08, 0x08, 0x00, 0x0a, 0x08, 0x08, 0x28, 0xfa, 0x07, 0x08,
   0x00, 0x02, 0x00, 0x08, 0x28, 0x02, 0x00, 0x08, 0x00, 0x02, 0x00, 0x08,
   0x28, 0x02, 0x00, 0x08, 0x00, 0x02, 0x00, 0x08, 0x00, 0x02, 0x00, 0x08,
   0x00, 0xfe, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const JConstBitmap kLandscapeBitmap = { 30,30, kLandscapeData };

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXPSPageSetupDialog*
JXPSPageSetupDialog::Create
	(
	const JPSPrinter::PaperType			paper,
	const JPSPrinter::ImageOrientation	orient
	)
{
	auto* dlog = jnew JXPSPageSetupDialog;
	assert( dlog != nullptr );
	dlog->BuildWindow(paper, orient);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXPSPageSetupDialog::JXPSPageSetupDialog()
	:
	JXModalDialogDirector()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPSPageSetupDialog::~JXPSPageSetupDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXPSPageSetupDialog::BuildWindow
	(
	const JPSPrinter::PaperType			paper,
	const JPSPrinter::ImageOrientation	orient
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 240,160, JString::empty);
	assert( window != nullptr );

	auto* orientationLabel =
		jnew JXStaticText(JGetString("orientationLabel::JXPSPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,85, 75,20);
	assert( orientationLabel != nullptr );
	orientationLabel->SetToLabel();

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXPSPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 140,130, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JXPSPageSetupDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXPSPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,130, 70,20);
	assert( cancelButton != nullptr );

	itsOrientation =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 95,65, 94,54);
	assert( itsOrientation != nullptr );

	auto* portraitRB =
		jnew JXImageRadioButton(1, itsOrientation,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 5,5, 40,40);
	assert( portraitRB != nullptr );

	auto* landscapeRB =
		jnew JXImageRadioButton(2, itsOrientation,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 45,5, 40,40);
	assert( landscapeRB != nullptr );

	itsPaperTypeMenu =
		jnew JXTextMenu(JGetString("itsPaperTypeMenu::JXPSPageSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,20, 180,30);
	assert( itsPaperTypeMenu != nullptr );

// end JXLayout

	SetObjects(okButton, cancelButton, itsPaperTypeMenu, paper,
			   itsOrientation, portraitRB, landscapeRB, orient);
}

/******************************************************************************
 SetObjects (protected)

 ******************************************************************************/

void
JXPSPageSetupDialog::SetObjects
	(
	JXTextButton*						okButton,
	JXTextButton*						cancelButton,
	JXTextMenu*							paperTypeMenu,
	const JPSPrinter::PaperType			paper,
	JXRadioGroup*						orientationRG,
	JXImageRadioButton*					portraitRB,
	JXImageRadioButton*					landscapeRB,
	const JPSPrinter::ImageOrientation	orient
	)
{
JIndex i;

	itsPaperTypeMenu = paperTypeMenu;
	itsOrientation   = orientationRG;

	(paperTypeMenu->GetWindow())->SetTitle(JGetString("WindowTitle::JXPSPageSetupDialog"));
	SetButtons(okButton, cancelButton);

	itsPaperTypeMenu->SetMenuItems(kPaperMenuStr);
	itsPaperTypeMenu->SetUpdateAction(JXMenu::kDisableNone);

	ListenTo(itsPaperTypeMenu, std::function([this](const JXMenu::NeedsUpdate&)
	{
		itsPaperTypeMenu->CheckItem(itsPaperType);
	}));

	ListenTo(itsPaperTypeMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsPaperType = msg.GetIndex();
	}));

	bool foundType = false;
	for (i=1; i<=kPaperTypeCount; i++)
	{
		if (kIndexToPaperType[i-1] == paper)
		{
			itsPaperType = i;
			itsPaperTypeMenu->SetToPopupChoice(true, itsPaperType);
			foundType = true;
			break;
		}
	}
	assert( foundType );

	bool foundOrient = false;
	for (i=1; i<=kOrientCount; i++)
	{
		if (kIndexToOrient[i-1] == orient)
		{
			itsOrientation->SelectItem(i);
			foundOrient = true;
			break;
		}
	}
	assert( foundOrient );

	portraitRB->SetBitmap(kPortraitBitmap);
	portraitRB->SetHint(JGetString("PortraitHint::JXPSPageSetupDialog"));

	landscapeRB->SetBitmap(kLandscapeBitmap);
	landscapeRB->SetHint(JGetString("LandscapeHint::JXPSPageSetupDialog"));
}

/******************************************************************************
 SetParameters (virtual)

	Derived classes can override this to extract extra information.

 ******************************************************************************/

bool
JXPSPageSetupDialog::SetParameters
	(
	JXPSPrinter* p
	)
	const
{
	const JPSPrinter::PaperType newPaperType =
		kIndexToPaperType[ itsPaperType-1 ];

	const JPSPrinter::ImageOrientation newOrientation =
		kIndexToOrient[ itsOrientation->GetSelectedItem()-1 ];

	const bool changed =
		newPaperType != p->GetPaperType() ||
						   newOrientation != p->GetOrientation();

	p->SetPaperType(newPaperType);
	p->SetOrientation(newOrientation);
	return changed;
}
