/******************************************************************************
 CMAboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "CMAboutDialog.h"
#include "cmHelpText.h"
#include "cmGlobals.h"
#include <JXApplication.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXHelpManager.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kUpgradeNoticeID     = "UpgradeNotice::CMAboutDialog";
static const JCharacter* kChangeButtonLabelID = "ChangeButtonLabel::CMAboutDialog";

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

CMAboutDialog::CMAboutDialog
	(
	JXDirector*			supervisor,
	const JCharacter*	prevVersStr
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsIsUpgradeFlag = kJFalse;

	BuildWindow(prevVersStr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMAboutDialog::~CMAboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "medic_about.xpm"
#include <new_planet_software.xpm>

void
CMAboutDialog::BuildWindow
	(
	const JCharacter* prevVersStr
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 430,180, "");
	assert( window != NULL );

	JXImageWidget* medicIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,20, 40,40);
	assert( medicIcon != NULL );

	JXStaticText* textWidget =
		jnew JXStaticText(JGetString("textWidget::CMAboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,20, 330,110);
	assert( textWidget != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CMAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,150, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CMAboutDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CMAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,150, 60,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CMAboutDialog::shortcuts::JXLayout"));

	itsCreditsButton =
		jnew JXTextButton(JGetString("itsCreditsButton::CMAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,150, 60,20);
	assert( itsCreditsButton != NULL );

	JXImageWidget* npsIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,75, 65,65);
	assert( npsIcon != NULL );

// end JXLayout

	window->SetTitle("About");
	SetButtons(okButton, NULL);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	// text

	JString text = CMGetVersionStr();
	if (!JStringEmpty(prevVersStr))
		{
		const JCharacter* map[] =
			{
			"vers", prevVersStr
			};
		text += JGetString(kUpgradeNoticeID);
		(JGetStringManager())->Replace(&text, map, sizeof(map));
		itsHelpButton->SetLabel(JGetString(kChangeButtonLabelID));
		itsIsUpgradeFlag = kJTrue;
		}
	textWidget->SetText(text);

	// Code Medic icon

	JXImage* image = jnew JXImage(GetDisplay(), medic_about);
	assert(image != NULL);
	medicIcon->SetImage(image, kJTrue);

	// NPS icon

	image = jnew JXImage(GetDisplay(), new_planet_software);
	assert( image != NULL );
	npsIcon->SetImage(image, kJTrue);

	// adjust window to fit text

	const JSize bdh = textWidget->GetBoundsHeight();
	const JSize aph = textWidget->GetApertureHeight();
	if (bdh > aph)
		{
		window->AdjustSize(0, bdh - aph);	// safe to calculate once bdh > aph
		}
}

/******************************************************************************
 Receive (protected)

	Handle extra buttons.

 ******************************************************************************/

void
CMAboutDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		if (itsIsUpgradeFlag)
			{
			(JXGetHelpManager())->ShowSection(kCMChangeLogName);
			}
		else
			{
			(JXGetHelpManager())->ShowSection(kCMOverviewHelpName);
			}
		EndDialog(kJTrue);
		}

	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kCMCreditsName);
		EndDialog(kJTrue);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
