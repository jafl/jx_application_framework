/******************************************************************************
 CBAboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBAboutDialog.h"
#include "CBAboutDialogIconTask.h"
#include "cbHelpText.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXHelpManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

CBAboutDialog::CBAboutDialog
	(
	JXDirector*			supervisor,
	const JCharacter*	prevVersStr
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	itsIsUpgradeFlag(kJFalse)
{
	BuildWindow(prevVersStr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBAboutDialog::~CBAboutDialog()
{
	delete itsAnimTask;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include <new_planet_software.xpm>

void
CBAboutDialog::BuildWindow
	(
	const JCharacter* prevVersStr
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 430,180, "");
	assert( window != NULL );

	JXImageWidget* jccIcon =
		new JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,20, 40,40);
	assert( jccIcon != NULL );

	JXStaticText* textWidget =
		new JXStaticText(JGetString("textWidget::CBAboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,20, 330,110);
	assert( textWidget != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::CBAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,150, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CBAboutDialog::shortcuts::JXLayout"));

	itsHelpButton =
		new JXTextButton(JGetString("itsHelpButton::CBAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,150, 60,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBAboutDialog::shortcuts::JXLayout"));

	itsCreditsButton =
		new JXTextButton(JGetString("itsCreditsButton::CBAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,150, 60,20);
	assert( itsCreditsButton != NULL );

	JXImageWidget* npsIcon =
		new JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,75, 65,65);
	assert( npsIcon != NULL );

// end JXLayout

	window->SetTitle("About");
	SetButtons(okButton, NULL);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	// text

	JString text = CBGetVersionStr();
	if (!JStringEmpty(prevVersStr))
		{
		const JCharacter* map[] =
			{
			"vers", prevVersStr
			};
		text += JGetString("UpgradeNotice::CBAboutDialog");
		(JGetStringManager())->Replace(&text, map, sizeof(map));
		itsHelpButton->SetLabel(JGetString("ChangeButtonLabel::CBAboutDialog"));
		itsIsUpgradeFlag = kJTrue;
		}
	}
	textWidget->SetText(text);

	// Code Crusader icon

	itsAnimTask = new CBAboutDialogIconTask(jccIcon);
	assert( itsAnimTask != NULL );
	itsAnimTask->Start();

	// NPS icon

	JXImage* image = new JXImage(GetDisplay(), new_planet_software);
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
CBAboutDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		if (itsIsUpgradeFlag)
			{
			(JXGetHelpManager())->ShowSection(kCBChangeLogName);
			}
		else
			{
			(JXGetHelpManager())->ShowSection(kCBOverviewHelpName);
			}
		EndDialog(kJTrue);
		}

	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kCBCreditsName);
		EndDialog(kJTrue);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
