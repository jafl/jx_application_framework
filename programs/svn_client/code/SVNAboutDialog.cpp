/******************************************************************************
 SVNAboutDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "SVNAboutDialog.h"
#include "svnGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXHelpManager.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kUpgradeNoticeID     = "UpgradeNotice::SVNAboutDialog";
static const JCharacter* kChangeButtonLabelID = "ChangeButtonLabel::SVNAboutDialog";

/******************************************************************************
 Constructor

	If prevVersStr is not empty, we change the Help button to "Changes".

 ******************************************************************************/

SVNAboutDialog::SVNAboutDialog
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

SVNAboutDialog::~SVNAboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "svn_about_icon.xpm"
#include <new_planet_software.xpm>

void
SVNAboutDialog::BuildWindow
	(
	const JCharacter* prevVersStr
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 430,200, "");
	assert( window != nullptr );

	JXStaticText* textWidget =
		jnew JXStaticText(JGetString("textWidget::SVNAboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,20, 330,130);
	assert( textWidget != nullptr );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::SVNAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 320,170, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::SVNAboutDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::SVNAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 185,170, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::SVNAboutDialog::shortcuts::JXLayout"));

	JXImageWidget* imageWidget =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 25,20, 40,40);
	assert( imageWidget != nullptr );

	itsCreditsButton =
		jnew JXTextButton(JGetString("itsCreditsButton::SVNAboutDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,170, 60,20);
	assert( itsCreditsButton != nullptr );

	JXImageWidget* npsIcon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,75, 65,65);
	assert( npsIcon != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::SVNAboutDialog"));
	SetButtons(okButton, nullptr);

	ListenTo(itsHelpButton);
	ListenTo(itsCreditsButton);

	JXImage* image = jnew JXImage(GetDisplay(), svn_about_icon);
	assert( image != nullptr );
	imageWidget->SetImage(image, kJTrue);

	JString text = SVNGetVersionStr();
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

	// NPS icon

	image = jnew JXImage(GetDisplay(), new_planet_software);
	assert( image != nullptr );
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
 Receive (virtual protected)

	Handle itsHelpButton.

 ******************************************************************************/

void
SVNAboutDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		if (itsIsUpgradeFlag)
			{
			(JXGetHelpManager())->ShowChangeLog();
			}
		else
			{
			(JXGetHelpManager())->ShowTOC();
			}
		EndDialog(kJTrue);
		}

	else if (sender == itsCreditsButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowCredits();
		EndDialog(kJTrue);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
