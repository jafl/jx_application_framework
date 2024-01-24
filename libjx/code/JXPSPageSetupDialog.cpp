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

	auto* window = jnew JXWindow(this, 240,160, JGetString("WindowTitle::JXPSPageSetupDialog::JXLayout"));

	itsPaperTypeMenu =
		jnew JXTextMenu(JGetString("itsPaperTypeMenu::JXPSPageSetupDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,20, 180,30);

	itsOrientation =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 95,65, 94,54);

	auto* portraitRB =
		jnew JXImageRadioButton(1, itsOrientation,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 5,5, 40,40);
#ifndef _H_jx_print_portrait
#define _H_jx_print_portrait
#include "jx_print_portrait.xpm"
#endif
	portraitRB->SetXPM(jx_print_portrait);

	auto* landscapeRB =
		jnew JXImageRadioButton(2, itsOrientation,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 45,5, 40,40);
#ifndef _H_jx_print_landscape
#define _H_jx_print_landscape
#include "jx_print_landscape.xpm"
#endif
	landscapeRB->SetXPM(jx_print_landscape);

	auto* orientationLabel =
		jnew JXStaticText(JGetString("orientationLabel::JXPSPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,85, 75,20);
	orientationLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXPSPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,130, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXPSPageSetupDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 139,129, 72,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::JXPSPageSetupDialog::JXLayout"));

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

	portraitRB->SetHint(JGetString("PortraitHint::JXPSPageSetupDialog"));
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
