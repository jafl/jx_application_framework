/******************************************************************************
 GPMAboutDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright © 2001 by New Planet Software. All rights reserved.

 ******************************************************************************/

#include "GPMAboutDialog.h"
#include "gpmGlobals.h"
#include "gpmHelpText.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXHelpManager.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kUpgradeNoticeID     = "UpgradeNotice::GPMAboutDialog";
static const JCharacter* kChangeButtonLabelID = "ChangeButtonLabel::GPMAboutDialog";

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

GPMAboutDialog::GPMAboutDialog
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

GPMAboutDialog::~GPMAboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "gpm_about_icon.xpm"
#include <new_planet_software.xpm>

void
GPMAboutDialog::BuildWindow
	(
	const JCharacter* prevVersStr
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 430,180, "");
    assert( window != NULL );

    JXStaticText* textWidget =
        new JXStaticText(JGetString("textWidget::GPMAboutDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 90,20, 330,110);
    assert( textWidget != NULL );

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::GPMAboutDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,150, 60,20);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::GPMAboutDialog::shortcuts::JXLayout"));

    itsHelpButton =
        new JXTextButton(JGetString("itsHelpButton::GPMAboutDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,150, 60,20);
    assert( itsHelpButton != NULL );

    JXImageWidget* gpmIcon =
        new JXImageWidget(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,20, 40,40);
    assert( gpmIcon != NULL );

    itsCreditsButton =
        new JXTextButton(JGetString("itsCreditsButton::GPMAboutDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,150, 60,20);
    assert( itsCreditsButton != NULL );

    JXImageWidget* npsIcon =
        new JXImageWidget(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,75, 65,65);
    assert( npsIcon != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GPMAboutDialog"));
	SetButtons(okButton, NULL);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	JXImage* image = new JXImage(GetDisplay(), gpm_about_icon);
	assert( image != NULL );
	gpmIcon->SetImage(image, kJTrue);

	image = new JXImage(GetDisplay(), new_planet_software);
	assert( image != NULL );
	npsIcon->SetImage(image, kJTrue);

	JString text = GPMGetVersionStr();
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

	const JSize bdh = textWidget->GetBoundsHeight();
	const JSize aph = textWidget->GetApertureHeight();
	if (bdh > aph)
		{
		window->AdjustSize(0, bdh - aph);	// safe to calculate once bdh > aph
		}
}

/******************************************************************************
 Receive (virtual protected)

	Handle itsHelpButton.

 ******************************************************************************/

void
GPMAboutDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		if (itsIsUpgradeFlag)
			{
			(JXGetHelpManager())->ShowSection(kGPMChangeLogName);
			}
		else
			{
			(JXGetHelpManager())->ShowSection(kGPMOverviewHelpName);
			}
		EndDialog(kJTrue);
		}

	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kGPMCreditsName);
		EndDialog(kJTrue);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
