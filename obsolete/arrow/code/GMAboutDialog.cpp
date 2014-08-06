/******************************************************************************
 GMAboutDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GMAboutDialog.h"
#include "GMAboutDialogIconTask.h"
#include "GMGlobals.h"
#include "GHelpText.h"

#include <JXApplication.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXHelpManager.h>

#include <jAssert.h>

// string ID's

static const JCharacter* kUpgradeNoticeID     = "UpgradeNotice::GMAboutDialog";
static const JCharacter* kChangeButtonLabelID = "ChangeButtonLabel::GMAboutDialog";

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

GMAboutDialog::GMAboutDialog
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

GMAboutDialog::~GMAboutDialog()
{
	delete itsAnimTask;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include <new_planet_software.xpm>

void
GMAboutDialog::BuildWindow
	(
	const JCharacter* prevVersStr
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 460,240, "");
	assert( window != NULL );

	JXStaticText* textWidget =
		new JXStaticText(JGetString("textWidget::GMAboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,20, 330,170);
	assert( textWidget != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::GMAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 340,210, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::GMAboutDialog::shortcuts::JXLayout"));

	itsHelpButton =
		new JXTextButton(JGetString("itsHelpButton::GMAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 195,210, 60,20);
	assert( itsHelpButton != NULL );

	JXImageWidget* arrowIcon =
		new JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,15, 100,100);
	assert( arrowIcon != NULL );

	itsCreditsButton =
		new JXTextButton(JGetString("itsCreditsButton::GMAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,210, 60,20);
	assert( itsCreditsButton != NULL );

	JXImageWidget* npsIcon =
		new JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,125, 65,65);
	assert( npsIcon != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GMAboutDialog"));
	SetButtons(okButton, NULL);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	itsAnimTask = new GMAboutDialogIconTask(arrowIcon);
	assert( itsAnimTask != NULL );
	itsAnimTask->Start();

	JXImage* image = new JXImage(GetDisplay(), new_planet_software);
	assert( image != NULL );
	npsIcon->SetImage(image, kJTrue);

	JString text = GMGetVersionStr();
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
 Receive (protected)

	Handle itsHelpButton.

 ******************************************************************************/

void
GMAboutDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		if (itsIsUpgradeFlag)
			{
			(JXGetHelpManager())->ShowSection(kGChangeLogName);
			}
		else
			{
			(JXGetHelpManager())->ShowSection(kGOverviewHelpName);
			}
		EndDialog(kJTrue);
		}

	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kGCreditsName);
		EndDialog(kJTrue);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
