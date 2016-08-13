/******************************************************************************
 GFGAboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 2002 by New Planet Software. All rights reserved.

 ******************************************************************************/

#include "GFGAboutDialog.h"
#include "gfgGlobals.h"
#include "gfgHelpText.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXHelpManager.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kUpgradeNoticeID     = "UpgradeNotice::GFGAboutDialog";
static const JCharacter* kChangeButtonLabelID = "ChangeButtonLabel::GFGAboutDialog";

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

GFGAboutDialog::GFGAboutDialog
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

GFGAboutDialog::~GFGAboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "gfg_about_icon.xpm"

void
GFGAboutDialog::BuildWindow
	(
	const JCharacter* prevVersStr
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 370,120, "");
	assert( window != NULL );

	JXStaticText* textWidget =
		new JXStaticText(JGetString("textWidget::GFGAboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,20, 280,50);
	assert( textWidget != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::GFGAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 260,90, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::GFGAboutDialog::shortcuts::JXLayout"));

	itsHelpButton =
		new JXTextButton(JGetString("itsHelpButton::GFGAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 155,90, 60,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::GFGAboutDialog::shortcuts::JXLayout"));

	JXImageWidget* imageWidget =
		new JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
	assert( imageWidget != NULL );

	itsCreditsButton =
		new JXTextButton(JGetString("itsCreditsButton::GFGAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,90, 60,20);
	assert( itsCreditsButton != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GFGAboutDialog"));
	SetButtons(okButton, NULL);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	JXImage* image = new JXImage(GetDisplay(), gfg_about_icon);
	assert( image != NULL );
	imageWidget->SetImage(image, kJTrue);

	JString text = GFGGetVersionStr();
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
GFGAboutDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		if (itsIsUpgradeFlag)
			{
			(JXGetHelpManager())->ShowSection(kGFGChangeLogName);
			}
		else
			{
			(JXGetHelpManager())->ShowSection(kGFGOverviewHelpName);
			}
		EndDialog(kJTrue);
		}

	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kGFGCreditsName);
		EndDialog(kJTrue);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
